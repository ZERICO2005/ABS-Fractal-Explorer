/*
**	Author: zerico2005 (2024)
**	Project: 
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"

class RenderBuffer {
	public:
	enum Color_Bit {
		Color_8bit, Color_16bit, Color_fp32,
		Color_Length
	};
	/* Constructors */
		RenderBuffer();
		RenderBuffer(
			int32_t resX, int32_t resY,
			size_t channels, Color_Bit color_bit,
			size_t padding = 0
		);
		~RenderBuffer();
	
	/* Functions */
		// Returns true if the RenderBuffer is properly initialized
		bool query_BufferInitialized() const;
		// Clears the buffer to 0
		void clear_Buffer();
		// Frees the buffer while retaining other variables
		void free_Buffer();
		// Frees the buffer and clears all other variables to 0
		void delete_Buffer();

		// `Frees and `calloc()` the buffer. Returns false if an error occurs
		bool init_Buffer(
			int32_t resX, int32_t resY,
			size_t channels, Color_Bit color_bit,
			size_t padding = 0
		);

		// `realloc()` the buffer if initialized. Returns false if an error occurs
		bool resize_Buffer(
			int32_t resX, int32_t resY,
			bool onlyReallocateBufferOnSizeIncrease = false
		);

		// (Advanced) `realloc()` buffer if AllocatedSize > BufferSize. Returns false if an error occurs
		bool trim_Buffer();

	/* Copy */
		// Writes buffer to dst
		void export_Buffer(
			uint8_t* dstBuf,
			int32_t resX, int32_t resY,
			size_t pixelSize, size_t pitch
		);
		// Reads buffer from src
		void import_Buffer(
			const uint8_t* srcBuf,
			int32_t resX, int32_t resY,
			size_t pixelSize, size_t pitch
		);
		
		// (Advanced) makes two RenderBuffers swap pointers
		void swap_RenderBuffer(RenderBuffer& renderBuffer);

	/* Variables */
		// Returns the allocated size of the RenderBuffer, which is equal to or greater than the buffer size unless the Buffer has been freed.
		size_t get_AllocatedSize() const;
		size_t get_BufferSize() const;
		int32_t get_ResX() const;
		int32_t get_ResY() const;
		void get_Resolution(int32_t& resX, int32_t& resY) const;
		size_t get_PixelSize() const;
		size_t get_Channels() const;
		Color_Bit get_ColorMode() const;
		size_t get_Pitch() const;
		// Returns the amount of bytes used for padding (Pitch - ResX * PixelSize)		
		size_t get_Padding() const;
		
		// (Advanced) Retrive the raw data, parameters may be nullptr
		void get_RawData(
			uint8_t** buffer, size_t* allocatedSize,
			int32_t* resX, int32_t* resY,
			size_t* pixelSize, size_t* pitch,
			size_t* padding = nullptr, size_t* channels = nullptr
		);

	/* Graphics */
		void set_GraphicsColor_RGB_8bit(
			uint8_t red, uint8_t green, uint8_t blue,
			uint8_t alpha = 0xFF
		);
		void set_GraphicsColor_RGB_16bit(
			uint16_t red, uint16_t green, uint16_t blue,
			uint16_t alpha = 0xFFFF
		);
		void set_GraphicsColor_RGB_fp32(
			fp32 red, fp32 green, fp32 blue,
			fp32 alpha = 1.0f
		);
		void set_GraphicsColor_HSV(
			fp32 hue, fp32 saturation, fp32 value,
			fp32 alpha = 1.0f
		);
		bool pointInBounds(int32_t posX, int32_t posY);
		// Fills the buffer with a solid color. Also see `clear_Buffer()`
		void fill_Buffer();
		void plot_Point(int32_t x, int32_t y);
		
		void plot_Horizontal(
			int32_t posX, int32_t posY,
			int32_t lenX
		);
		void plot_Vertical(
			int32_t posX, int32_t posY,
			int32_t lenY
		);
		// Plots the outline of a rectangle
		void plot_Rectangle(
			int32_t posX, int32_t posY,
			int32_t lenX, int32_t lenY
		);
		// Plots a filled rectangle
		void fill_Rectangle(
			int32_t posX, int32_t posY,
			int32_t lenX, int32_t lenY
		);

	private:
		uint8_t* Buffer;
		size_t AllocatedSize;
		
		int32_t ResX;
		int32_t ResY;
		size_t PixelSize;
		size_t Pitch;

		size_t Padding;
		size_t Channels;
		
		// `realloc()` the buffer. Calls `calloc` if the buffer is unitialized. Returns false if allocation failed.
		bool reallocate_Buffer(size_t allocatedSize);
		size_t get_ChannelSize(Color_Bit color_mode);
	/* Graphics */
		union ColorUnion {
			uint8_t RGBA_8bit[4];
			uint16_t RGBA_16bit[4];
			fp32 RGBA_fp32[4];
		} GraphicsColor;
		Color_Bit Color_Mode;
		inline size_t calculate_PixelOffset(int32_t posX, int32_t posY);
};