/*
**	Author: zerico2005 (2024)
**	Project: 
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "RenderBuffer.hpp"

#include "BufferCopy.hpp"

/* class RenderBuffer */
/* public */
	/* Constructors */
		RenderBuffer::RenderBuffer() {
			delete_Buffer();
		}
		RenderBuffer::RenderBuffer(
			int32_t resX, int32_t resY,
			size_t channels, Color_Bit color_bit,
			size_t padding
		) {
			delete_Buffer();
			init_Buffer(
				resX, resY,
				channels, color_bit,
				padding
			);
		}
		RenderBuffer::~RenderBuffer() {
			delete_Buffer();
		}

	/* Functions */
		bool RenderBuffer::query_BufferInitialized() const {
			return (
				(Buffer == nullptr) || (AllocatedSize == 0) ||
				(ResX <= 0) || (ResY <= 0) ||
				(PixelSize == 0) || (Pitch == 0)
			) ? false : true;
		}
		void RenderBuffer::clear_Buffer() {
			if (Buffer == nullptr || AllocatedSize == 0) { return; }
			memset(Buffer,0,AllocatedSize);
		}
		void RenderBuffer::free_Buffer() {
			free(Buffer);
			Buffer = nullptr;
			AllocatedSize = 0;
		}
		void RenderBuffer::delete_Buffer() {
			free_Buffer();
			ResX = 0;
			ResY = 0;
			PixelSize = 0;
			Pitch = 0;

			Padding = 0;
			Channels = 0;

			memset(&GraphicsColor,0,sizeof(GraphicsColor));
			Color_Mode = Color_8bit;
		}

		bool RenderBuffer::init_Buffer(
			int32_t resX, int32_t resY,
			size_t channels, Color_Bit color_bit,
			size_t padding
		) {
			size_t pixelSize = channels * get_ChannelSize(color_bit);
			size_t pitch = (size_t)resX * pixelSize;
			// Validates parameters
			if (
				(resX <= 0 || resY <= 0) ||
				(channels == 0 || channels > 4) || 
				(color_bit <= 0 || color_bit >= Color_Length)
			) {
				return false;
			}
			if (padding != 0 && pitch % padding != 0) { // Rounds up for padding
				pitch += padding - (pitch % padding);
			}
			if (reallocate_Buffer(pitch * (size_t)resY) == false) {
				return false;
			}
			ResX = resX;
			ResY = resY;
			PixelSize = pixelSize;
			Padding = padding;
			Channels = channels;
			Pitch = pitch;
			return true;
		}
		bool RenderBuffer::resize_Buffer(
			int32_t resX, int32_t resY,
			bool onlyReallocateBufferOnSizeIncrease
		) {
			if (resX <= 0 || resY <= 0) { return false; }
			size_t pitch = (size_t)resX * PixelSize;
			if (Padding != 0 && pitch % Padding != 0) { // Rounds up for padding
				pitch += Padding - (pitch % Padding);
			}
			if (
				(pitch * (size_t)resY != AllocatedSize) &&
				(pitch * (size_t)resY > AllocatedSize || onlyReallocateBufferOnSizeIncrease == false)
			) {
				if (reallocate_Buffer(pitch * (size_t)resY) == false) {
					return false;
				}
			}
			ResX = resX;
			ResY = resY;
			Pitch = pitch;
			return true;
		}

		bool RenderBuffer::trim_Buffer() {
			return reallocate_Buffer(Pitch * (size_t)ResY);
		}
	
	/* Copy */
		void RenderBuffer::export_Buffer(
			uint8_t* dstBuf,
			int32_t resX, int32_t resY,
			size_t pixelSize, size_t pitch
		) {
			Buffer_Data dstData; Buffer_Data srcData;
			set_Buffer_Data(dstData, (uint32_t)resX, (uint32_t)resY, pixelSize, pitch);
			set_Buffer_Data(srcData, (uint32_t)ResX, (uint32_t)ResY, PixelSize, Pitch);
			BufferCopy(dstBuf,Buffer,dstData,srcData,true);
		}
		void RenderBuffer::import_Buffer(
			const uint8_t* srcBuf,
			int32_t resX, int32_t resY,
			size_t pixelSize, size_t pitch
		) {
			Buffer_Data dstData; Buffer_Data srcData;
			set_Buffer_Data(dstData, (uint32_t)ResX, (uint32_t)ResY, PixelSize, Pitch);
			set_Buffer_Data(srcData, (uint32_t)resX, (uint32_t)resY, pixelSize, pitch);
			BufferCopy(Buffer,srcBuf,dstData,srcData,true);
		}
		
	/* Variables */
		size_t RenderBuffer::get_AllocatedSize() const { return AllocatedSize; }
		size_t RenderBuffer::get_BufferSize() const {
			return Pitch * (size_t)ResY;
		}
		int32_t RenderBuffer::get_ResX() const { return ResX; }
		int32_t RenderBuffer::get_ResY() const { return ResY; }
		void RenderBuffer::get_Resolution(int32_t& resX, int32_t& resY) const {
			resX = ResX; resY = ResY;
		}
		size_t RenderBuffer::get_PixelSize() const { return PixelSize; }
		size_t RenderBuffer::get_Channels() const { return Channels; }
		RenderBuffer::Color_Bit RenderBuffer::get_ColorMode() const { return Color_Mode; }
		size_t RenderBuffer::get_Pitch() const { return Pitch; }
		size_t RenderBuffer::get_Padding() const { return Pitch - ((size_t)ResX * PixelSize); }
		
		void RenderBuffer::get_RawData(
			uint8_t** buffer, size_t* allocatedSize,
			int32_t* resX, int32_t* resY,
			size_t* pixelSize, size_t* pitch,
			size_t* padding, size_t* channels
		) {
			if (buffer != nullptr) { *buffer = Buffer; }
			if (allocatedSize != nullptr) { *allocatedSize = AllocatedSize; }
			if (resX != nullptr) { *resX = ResX; }
			if (resY != nullptr) { *resY = ResY; }
			if (pixelSize != nullptr) { *pixelSize = PixelSize; }
			if (pitch != nullptr) { *pitch = Pitch; }
			if (padding != nullptr) { *padding = Padding; }
			if (channels != nullptr) { *channels = Channels; }
		}

	/* Graphics */
		constexpr uint16_t RGBA_8bit_16bit_conversion = UINT16_MAX / UINT8_MAX; // (65535 / 255) == 257
		void RenderBuffer::set_GraphicsColor_RGB_8bit(
			uint8_t red, uint8_t green, uint8_t blue,
			uint8_t alpha
		) {
			switch (Color_Mode) {
				default:
				case Color_8bit:
					GraphicsColor.RGBA_8bit[0] = red;
					GraphicsColor.RGBA_8bit[1] = green;
					GraphicsColor.RGBA_8bit[2] = blue;
					GraphicsColor.RGBA_8bit[3] = alpha;
					return;
				case Color_16bit:
					GraphicsColor.RGBA_16bit[0] = (uint16_t)red * RGBA_8bit_16bit_conversion;
					GraphicsColor.RGBA_16bit[1] = (uint16_t)green * RGBA_8bit_16bit_conversion;
					GraphicsColor.RGBA_16bit[2] = (uint16_t)blue * RGBA_8bit_16bit_conversion;
					GraphicsColor.RGBA_16bit[3] = (uint16_t)alpha * RGBA_8bit_16bit_conversion;
					return;
				case Color_fp32:
					GraphicsColor.RGBA_fp32[0] = (fp32)red / (fp32)UINT8_MAX;
					GraphicsColor.RGBA_fp32[1] = (fp32)green / (fp32)UINT8_MAX;
					GraphicsColor.RGBA_fp32[2] = (fp32)blue / (fp32)UINT8_MAX;
					GraphicsColor.RGBA_fp32[3] = (fp32)alpha / (fp32)UINT8_MAX;
					return;
			}
		}
		void RenderBuffer::set_GraphicsColor_RGB_16bit(
			uint16_t red, uint16_t green, uint16_t blue,
			uint16_t alpha
		) {
			switch (Color_Mode) {
				default:
				case Color_8bit:
					GraphicsColor.RGBA_8bit[0] = (uint8_t)(red / RGBA_8bit_16bit_conversion);
					GraphicsColor.RGBA_8bit[1] = (uint8_t)(green / RGBA_8bit_16bit_conversion);
					GraphicsColor.RGBA_8bit[2] = (uint8_t)(blue / RGBA_8bit_16bit_conversion);
					GraphicsColor.RGBA_8bit[3] = (uint8_t)(alpha / RGBA_8bit_16bit_conversion);
					return;
				case Color_16bit:
					GraphicsColor.RGBA_16bit[0] = red;
					GraphicsColor.RGBA_16bit[1] = green;
					GraphicsColor.RGBA_16bit[2] = blue;
					GraphicsColor.RGBA_16bit[3] = alpha;
					return;
				case Color_fp32:
					GraphicsColor.RGBA_fp32[0] = (fp32)red / (fp32)UINT16_MAX;
					GraphicsColor.RGBA_fp32[1] = (fp32)green / (fp32)UINT16_MAX;
					GraphicsColor.RGBA_fp32[2] = (fp32)blue / (fp32)UINT16_MAX;
					GraphicsColor.RGBA_fp32[3] = (fp32)alpha / (fp32)UINT16_MAX;
					return;
			}
		}
		void RenderBuffer::set_GraphicsColor_RGB_fp32(
			fp32 red, fp32 green, fp32 blue,
			fp32 alpha
		) {
			valueClamp(red, 0.0f, 1.0f);
			valueClamp(green, 0.0f, 1.0f);
			valueClamp(blue, 0.0f, 1.0f);
			valueClamp(alpha, 0.0f, 1.0f);
			switch (Color_Mode) {
				default:
				case Color_8bit:
					GraphicsColor.RGBA_8bit[0] = (uint8_t)(red * (fp32)UINT8_MAX);
					GraphicsColor.RGBA_8bit[1] = (uint8_t)(green * (fp32)UINT8_MAX);
					GraphicsColor.RGBA_8bit[2] = (uint8_t)(blue * (fp32)UINT8_MAX);
					GraphicsColor.RGBA_8bit[3] = (uint8_t)(alpha * (fp32)UINT8_MAX);
					return;
				case Color_16bit:
					GraphicsColor.RGBA_16bit[0] = (uint16_t)(red * (fp32)UINT16_MAX);
					GraphicsColor.RGBA_16bit[1] = (uint16_t)(green * (fp32)UINT16_MAX);
					GraphicsColor.RGBA_16bit[2] = (uint16_t)(blue * (fp32)UINT16_MAX);
					GraphicsColor.RGBA_16bit[3] = (uint16_t)(alpha * (fp32)UINT16_MAX);
					return;
				case Color_fp32:
					GraphicsColor.RGBA_fp32[0] = red;
					GraphicsColor.RGBA_fp32[1] = green;
					GraphicsColor.RGBA_fp32[2] = blue;
					GraphicsColor.RGBA_fp32[3] = alpha;
					return;
			}
		}
		void RenderBuffer::set_GraphicsColor_HSV(
			fp32 hue, fp32 saturation, fp32 value,
			fp32 alpha
		) {
			valueClamp(hue, 0.0f, 1.0f);
			valueClamp(saturation, 0.0f, 1.0f);
			valueClamp(value, 0.0f, 1.0f);
			valueClamp(alpha, 0.0f, 1.0f);
			switch (Color_Mode) {
				default:
				case Color_8bit:
					getRGBfromHSV(
						&GraphicsColor.RGBA_8bit[0],
						&GraphicsColor.RGBA_8bit[1],
						&GraphicsColor.RGBA_8bit[2],
						hue, saturation, value
					);
					GraphicsColor.RGBA_8bit[3] = (uint8_t)(alpha * (fp32)UINT8_MAX);
					return;
				case Color_16bit:
					getRGBfromHSV(
						&GraphicsColor.RGBA_16bit[0],
						&GraphicsColor.RGBA_16bit[1],
						&GraphicsColor.RGBA_16bit[2],
						hue, saturation, value
					);
					GraphicsColor.RGBA_16bit[3] = (uint16_t)(alpha * (fp32)UINT16_MAX);
					return;
				case Color_fp32:
					getRGBfromHSV(
						&GraphicsColor.RGBA_fp32[0],
						&GraphicsColor.RGBA_fp32[1],
						&GraphicsColor.RGBA_fp32[2],
						hue, saturation, value
					);
					GraphicsColor.RGBA_fp32[3] = alpha;
					return;
			}
		}
		bool RenderBuffer::pointInBounds(int32_t posX, int32_t posY) {
			return (
				(posX < 0) || (posY < 0) ||
				(posX >= ResX) || (posY >= ResY)
			) ? false : true;
		}
		void RenderBuffer::fill_Buffer() {
			if (query_BufferInitialized() == false) { return; }
			size_t offset = 0;
			for (int32_t y = 0; y < ResY; y++) {
				patternMemcpy(&Buffer[offset], (size_t)ResX * PixelSize, &GraphicsColor, PixelSize);
				offset += Pitch;
			}
		}
		void RenderBuffer::plot_Point(int32_t posX, int32_t posY) {
			if (query_BufferInitialized() == false) { return; }
			if (pointInBounds(posX, posY) == false) { return; }
			size_t offset = calculate_PixelOffset(posX, posY);
			memcpy(&Buffer[offset], &GraphicsColor, PixelSize);
		}

		void RenderBuffer::plot_Horizontal(
			int32_t posX, int32_t posY,
			int32_t lenX
		) {
			if (query_BufferInitialized() == false) { return; }
			if (pointInBounds(posX, posY) == false) { return; }
			if ((lenX <= 0) || (posX + lenX >= ResX)) { return; }
			size_t offset = calculate_PixelOffset(posX, posY);
			patternMemcpy(&Buffer[offset], (size_t)lenX * PixelSize, &GraphicsColor, PixelSize);
		}
		void RenderBuffer::plot_Vertical(
			int32_t posX, int32_t posY,
			int32_t lenY
		) {
			if (query_BufferInitialized() == false) { return; }
			if (pointInBounds(posX, posY) == false) { return; }
			if ((lenY <= 0) || (posY + lenY >= ResY)) { return; }
			size_t offset = calculate_PixelOffset(posX, posY);
			for (int32_t y = 0; y < lenY; y++) {
				memcpy(&Buffer[offset],&GraphicsColor,PixelSize);
				offset += Pitch;
			}
		}
		void RenderBuffer::plot_Rectangle(
			int32_t posX, int32_t posY,
			int32_t lenX, int32_t lenY
		) {
			if (query_BufferInitialized() == false) { return; }
			if (pointInBounds(posX, posY) == false) { return; }
			if (
				(lenY <= 0) || (posY + lenY >= ResY) ||
				(lenX <= 0) || (posY + lenY >= ResY)
			) { return; }
			plot_Horizontal(posX, posY, lenX); // Top
			plot_Horizontal(posX, posY + lenY, lenX); // Bottom
			plot_Vertical(posX, posY + 1, lenY - 2); // Left
			plot_Vertical(posX + lenX, posY + 1, lenY - 2); // Right
		}
		void RenderBuffer::fill_Rectangle(
			int32_t posX, int32_t posY,
			int32_t lenX, int32_t lenY
		) {
			if (query_BufferInitialized() == false) { return; }
			if (pointInBounds(posX, posY) == false) { return; }
			if (
				(lenY <= 0) || (posY + lenY >= ResY) ||
				(lenX <= 0) || (posY + lenY >= ResY)
			) { return; }
			size_t offset = calculate_PixelOffset(posX, posY);
			for (int32_t y = 0; y < lenY; y++) {
				patternMemcpy(&Buffer[offset], (size_t)lenX * PixelSize, &GraphicsColor, PixelSize);
				offset += Pitch;
			}
		}

/* private */
	/* Allocation */
	bool RenderBuffer::reallocate_Buffer(size_t newSize) {
		if (newSize == 0) {
			free_Buffer();
			return false;
		}
		if (Buffer == nullptr || AllocatedSize == 0) {
			free_Buffer();
			Buffer = (uint8_t*)calloc(newSize, sizeof(uint8_t*));
			if (Buffer == nullptr) {
				perror("reallocate_Buffer() failed to calloc()");
				AllocatedSize = 0;
				return false;
			}
			AllocatedSize = newSize;
			return true;
		}
		Buffer = (uint8_t*)realloc(Buffer, newSize);
		if (Buffer == nullptr) {
			perror("reallocate_Buffer() failed to realloc()");
			AllocatedSize = 0;
			return false;
		}
		AllocatedSize = newSize;
		return true;
	}
	size_t RenderBuffer::get_ChannelSize(Color_Bit color_mode) {
		switch (color_mode) {
			default:
			case Color_8bit:
				return sizeof(uint8_t);
				break;
			case Color_16bit:
				return sizeof(uint16_t);
				break;
			case Color_fp32:
				return sizeof(fp32);
				break;
		}
	}

	/* Graphics */
		inline size_t RenderBuffer::calculate_PixelOffset(int32_t posX, int32_t posY) {
			return ((size_t)posY * Pitch) + ((size_t)posX * PixelSize);
		}