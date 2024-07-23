/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "frame_Transformation.h"

#include "render.h"
#include "copyBuffer.h"
#include "imageBuffer.h"
#include "fractal.h"
#include "user_data.h"

#include <xmmintrin.h>

// Precision used for transformations
typedef fp64 fpTran;

// static size_t calculate_Dst_Buf_overlap_with_Src_Buf(
// 	dim32_t src_ResX, dim32_t src_ResY,
// 	fpCord dst_x00, fpCord dst_y00,
// 	fpCord dst_x11, fpCord dst_y11,
// 	fpCord dst_x01, fpCord dst_y01,
// 	fpCord dst_x10, fpCord dst_y10,
// 	fpCord src_x00, fpCord src_y00,
// 	fpCord src_x11, fpCord src_y11,
// 	fpCord src_x01, fpCord src_y01,
// 	fpCord src_x10, fpCord src_y10
// ) {
// 	return 0;
// }

/**
 * @brief Generic Method. Runs faster, but leaves pixels gaps and skips pixels near the edge of the frame.
*/
__attribute__((unused)) static int frame_Transform_Generic_Fast(
	const ImageBuffer& image,
	BufferBox& blit,
	const ABS_Mandelbrot& FRAC
) {
	// nano64_t startTime = getNanoTime();
	
	//size_t plotted_pixels = 0;

	const uint32_t* image_buf = (const uint32_t*)image.vram;
	uint32_t* blit_buf = (uint32_t*)blit.vram;

	/* Pre calculated constants */
		const fpTran Recip_Image_DimX = (fpTran)1.0 / (fpTran)(image.resX - 1);
		const fpTran Recip_Image_DimY = (fpTran)1.0 / (fpTran)(image.resY - 1);

		const fpTran Image_Cord_X00 = (fpTran)(image.x00 - FRAC.r);
		const fpTran Image_Cord_Y00 = (fpTran)(image.y00 - FRAC.i);
		const fpTran Image_Cord_X01_sub_X00 = (fpTran)(image.x01 - image.x00);
		const fpTran Image_Cord_Y01_sub_Y00 = (fpTran)(image.y01 - image.y00);
		// const fpTran Image_Cord_X10 = (fpTran)(image.x10 - FRAC.r);
		// const fpTran Image_Cord_Y10 = (fpTran)(image.y10 - FRAC.i);
		// const fpTran Image_Cord_X11_sub_X10 = (fpTran)(image.x11 - image.x10);
		// const fpTran Image_Cord_Y11_sub_Y10 = (fpTran)(image.y11 - image.y10);

		const fpTran Image_Cord_X10_sub_X00 = (fpTran)(image.x10 - image.x00);
		const fpTran Image_Cord_Y10_sub_Y00 = (fpTran)(image.y10 - image.y00);

		/* Cancel Out to Zero, probably applicable to quadralaterals */
		// const fpTran Image_Cord_X11subX10_sub_X01subX00 = (fpTran)(Image_Cord_X11_sub_X10 - Image_Cord_X01_sub_X00);
		// const fpTran Image_Cord_Y11subY10_sub_Y01subY00 = (fpTran)(Image_Cord_Y11_sub_Y10 - Image_Cord_Y01_sub_Y00);

		const fpTran Zoom_Value_mult_ResZ_div_2 = pow((fpTran)10.0, (fpTran)FRAC.zoom) * (
			(blit.resX >= blit.resY) ?
			((fpTran)(blit.resY - 1) / (fpTran)2.0) :
			((fpTran)(blit.resX - 1) / (fpTran)2.0)
		);
		const fpTran Rot_Cos = cos((fpTran)FRAC.rot);
		const fpTran Rot_Sin = sin((fpTran)FRAC.rot);

		const fpTran     Rot_Sin_mult_ZVmRZd2_div_Stretch_X = ( Rot_Sin * Zoom_Value_mult_ResZ_div_2) / (fpTran)FRAC.sX;
		const fpTran neg_Rot_Sin_mult_ZVmRZd2_div_Stretch_Y = (-Rot_Sin * Zoom_Value_mult_ResZ_div_2) / (fpTran)FRAC.sY;
		const fpTran     Rot_Cos_mult_ZVmRZd2_div_Stretch_X = ( Rot_Cos * Zoom_Value_mult_ResZ_div_2) / (fpTran)FRAC.sX;
		const fpTran neg_Rot_Cos_mult_ZVmRZd2_div_Stretch_Y = (-Rot_Cos * Zoom_Value_mult_ResZ_div_2) / (fpTran)FRAC.sY;
	
	/* Pixel Jumps */

		const fpTran Horiz_JumpX = Recip_Image_DimX * (Image_Cord_X10_sub_X00 *     Rot_Cos_mult_ZVmRZd2_div_Stretch_X + Image_Cord_Y10_sub_Y00 *     Rot_Sin_mult_ZVmRZd2_div_Stretch_X);
		const fpTran Horiz_JumpY = Recip_Image_DimX * (Image_Cord_Y10_sub_Y00 * neg_Rot_Cos_mult_ZVmRZd2_div_Stretch_Y - Image_Cord_X10_sub_X00 * neg_Rot_Sin_mult_ZVmRZd2_div_Stretch_Y);
		const fpTran Verti_JumpX = Recip_Image_DimY * (Image_Cord_X01_sub_X00 *     Rot_Cos_mult_ZVmRZd2_div_Stretch_X + Image_Cord_Y01_sub_Y00 *     Rot_Sin_mult_ZVmRZd2_div_Stretch_X);
		const fpTran Verti_JumpY = Recip_Image_DimY * (Image_Cord_Y01_sub_Y00 * neg_Rot_Cos_mult_ZVmRZd2_div_Stretch_Y - Image_Cord_X01_sub_X00 * neg_Rot_Sin_mult_ZVmRZd2_div_Stretch_Y);

		const int32_t JumpX_Sign = (Horiz_JumpX * -Verti_JumpX >= (fpTran)0.0) ? 1 : -1;
		const int32_t JumpY_Sign = (Horiz_JumpY *  Verti_JumpY >= (fpTran)0.0) ? 1 : -1;
		const int32_t JumpX = (int32_t)ceil(hypot(Horiz_JumpX, Verti_JumpX)) * JumpX_Sign;
		const int32_t JumpY = (int32_t)ceil(hypot(Horiz_JumpY, Verti_JumpY)) * JumpY_Sign;

		constexpr size_t Pixel_Size = 1;

		const int32_t Repeat_X = (JumpX == 0) ? 1 : JumpX;
		const int32_t Repeat_Y = (JumpY == 0) ? 1 : JumpY;
		const size_t Repeat_U = (size_t)abs(Repeat_X);
		const size_t Repeat_V = (size_t)abs(Repeat_Y);

		const int32_t Minimum_PosX = 0; // (Repeat_X >= 0) ? 0 : -(Repeat_X - 1);
		const int32_t Minimum_PosY = 0; // (Repeat_Y >= 0) ? 0 : -(Repeat_Y - 1);
		const int32_t Maximum_PosX = blit.resX - ((Repeat_X == 0) ? 0 : (abs(Repeat_X) - 1));
		const int32_t Maximum_PosY = blit.resY - ((Repeat_Y == 0) ? 0 : (abs(Repeat_Y) - 1));
		
		__attribute__((unused)) const size_t blit_Size = getBufferBoxSize(&blit) / IMAGE_BUFFER_CHANNELS;
		const size_t blit_Pitch = getBufferBoxPitch(&blit) / IMAGE_BUFFER_CHANNELS;
		const size_t blit_Pitch_SIMD = getBufferBoxPitch(&blit);
		// const size_t blit_Pixel_Jump = (Repeat_X >= 0) ? Pixel_Size : -Pixel_Size;
		// const size_t blit_Pitch_Jump = (Repeat_Y >= 0) ? (blit_Pitch - (Repeat_U * Pixel_Size)) : -(blit_Pitch - (Repeat_U * Pixel_Size));
		const size_t blit_Pixel_Jump = Pixel_Size;
		const size_t blit_Pitch_Jump = (blit_Pitch - (Repeat_U * Pixel_Size));


		// Ensures that the pixel jumps are positive to simplify logic
		const fpTran PosX_Offset_plus_ResX_div_2 = (fpTran)((Repeat_X >= 1) ? 0 : (Repeat_X + 1)) + ((fpTran)(blit.resX - 1) / (fpTran)2.0);
		const fpTran PosY_Offset_plus_ResY_div_2 = (fpTran)((Repeat_Y >= 1) ? 0 : (Repeat_Y + 1)) + ((fpTran)(blit.resY - 1) / (fpTran)2.0);

		// printfInterval(0.4, "\nPixel Jump: X{%.5f,%.5f} Y{%.5f,%.5f} Repeat{%d,%d} Min{%d,%d} Max{%d,%d}",
		// 	Horiz_JumpX, Horiz_JumpY, Verti_JumpX, Verti_JumpY,
		// 	Repeat_X, Repeat_Y, Minimum_PosX, Minimum_PosY, Maximum_PosX, Maximum_PosY
		// );
		
	//size_t image_offset = 0;
	fpTran Y_Value = (fpTran)0.0;

	const uint32_t* image_ptr = image_buf;

	#define Generic_Fast_Transform_Start() \
		for (dim32_t y = 0; y < image.resY; y++) {\
			/* Calculates which two points to interpolate between in the next loop */\
			const fpTran X0_Cord = Image_Cord_X00 + Y_Value * Image_Cord_X01_sub_X00;\
			const fpTran Y0_Cord = Image_Cord_Y00 + Y_Value * Image_Cord_Y01_sub_Y00;\
			fpTran X_Value = (fpTran)0.0;\
			\
			for (dim32_t x = 0; x < image.resX; x++) {\
				/* Calculates the X and Y cordinates of what pixel the Src buffer maps to on the Dst buffer */\
				fpTran X_Cord = X0_Cord + X_Value * Image_Cord_X10_sub_X00;\
				fpTran Y_Cord = Y0_Cord + X_Value * Image_Cord_Y10_sub_Y00;\
				\
				int32_t posX = (int32_t)(X_Cord *     Rot_Cos_mult_ZVmRZd2_div_Stretch_X + Y_Cord *     Rot_Sin_mult_ZVmRZd2_div_Stretch_X + PosX_Offset_plus_ResX_div_2);\
				int32_t posY = (int32_t)(Y_Cord * neg_Rot_Cos_mult_ZVmRZd2_div_Stretch_Y - X_Cord * neg_Rot_Sin_mult_ZVmRZd2_div_Stretch_Y + PosY_Offset_plus_ResY_div_2);\
				\
				/* Does the same thing, but is much slower */\
					/* int32_t posX; int32_t posY; */\
					/* coordinate_to_pixel(X_Cord, Y_Cord, posX, posY, FRAC, blit.resX, blit.resY); */\
				\
				/* Copies the pixel data from src to dst if the pixel is in bounds */\
				if (posX >= Minimum_PosX && posX < Maximum_PosX && posY >= Minimum_PosY && posY < Maximum_PosY) {


	#define Generic_Fast_Transform_End() \
				}\
				image_ptr++;\
				X_Value += Recip_Image_DimX;\
			}\
			Y_Value += Recip_Image_DimY;\
		}
	
	enum Generic_Fast_Transform_Enum {
		MT_Xn_Yn, MT_X1_Yn,
		MT_Xn_Y1, MT_X1_Y1,
		/* SIMD Methods */
		MT_X2_Yn_SSE2,
		MT_X4_Yn_SSE2
	};
	int_enum MT_Value = (int_enum)(
		((Repeat_U > 1) ? 0 : (Repeat_U)) +
		((Repeat_V > 1) ? 0 : (2 * Repeat_V))
	);
	if (
		(MT_Value == MT_Xn_Yn) &&
		(get_Available_CPU_Instruction().SSE_Family.SSE2 == true)
	) {
		switch (Repeat_U) {
			case 2:
				MT_Value = MT_X2_Yn_SSE2;
				break;
			case 4:
				MT_Value = MT_X4_Yn_SSE2;
				break;
		}
	}

	// blit_buf <= blit_ptr <= blit_buf + blit_size
	switch (MT_Value) {
		case MT_Xn_Yn: default: { Generic_Fast_Transform_Start();
			uint32_t* blit_ptr = &blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)];
			const uint32_t plot_color = *image_ptr;
			for (size_t v = 0; v < Repeat_V; v++) {
				for (size_t u = 0; u < Repeat_U; u++) {
					*blit_ptr = plot_color;
					blit_ptr += blit_Pixel_Jump;
				}
				blit_ptr += blit_Pitch_Jump;
			}
			Generic_Fast_Transform_End(); } break;
		case MT_X1_Yn: { Generic_Fast_Transform_Start();
			uint32_t* blit_ptr = &blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)];
			const uint32_t plot_color = *image_ptr;
			for (size_t v = 0; v < Repeat_V; v++) {
				*blit_ptr = plot_color;
				blit_ptr += blit_Pitch;
			}
			Generic_Fast_Transform_End(); } break;
		case MT_Xn_Y1: { Generic_Fast_Transform_Start();
			uint32_t* blit_ptr = &blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)];
			const uint32_t plot_color = *image_ptr;
			for (size_t u = 0; u < Repeat_U; u++) {
				*blit_ptr = plot_color;
				blit_ptr += blit_Pixel_Jump;
			}
			Generic_Fast_Transform_End(); } break;
		case MT_X1_Y1: { Generic_Fast_Transform_Start();
			blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)] = *image_ptr;
			Generic_Fast_Transform_End(); } break;
		/* SIMD Methods */
		case MT_X2_Yn_SSE2: { Generic_Fast_Transform_Start();
			const __m128i pixel_color = _mm_castps_si128(_mm_load1_ps((float*)(void*)image_ptr));
			void* blit_ptr = &blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)];
			for (size_t v = 0; v < Repeat_V; v++) {
				_mm_storeu_si64(blit_ptr, pixel_color);
				blit_ptr = (void*)((uint8_t*)blit_ptr + blit_Pitch_SIMD);
			}
		Generic_Fast_Transform_End(); } break;
		case MT_X4_Yn_SSE2: { Generic_Fast_Transform_Start();
			const __m128i pixel_color = _mm_castps_si128(_mm_load1_ps((float*)(void*)image_ptr));
			void* blit_ptr = &blit_buf[((size_t)posY * blit_Pitch) + ((size_t)posX * Pixel_Size)];
			for (size_t v = 0; v < Repeat_V; v++) {
				_mm_storeu_si128((__m128i*)blit_ptr, pixel_color);
				blit_ptr = (void*)((uint8_t*)blit_ptr + blit_Pitch_SIMD);
			}
		Generic_Fast_Transform_End(); } break;
	}

	#undef Generic_Fast_Transform_Start
	#undef Generic_Fast_Transform_End

	// nano64_t finishTime = getNanoTime();
	// printfInterval(0.4,"\nTime: %.3lf | %zux%zu", NANO_TO_SECONDS(finishTime - startTime) * 1.0e3, Repeat_U, Repeat_V);
	
	return 0;
}

int frame_Transform(
	const ImageBuffer& image,
	BufferBox& blit,
	const ABS_Mandelbrot& FRAC,
	__attribute__((unused)) const User_Rendering_Settings& config
) {
	if (image.vram == nullptr) { printError("const ImageBuffer& image.vram is nullptr"); return -1; }
	if (image.allocated() == false) { printError("const ImageBuffer& image is not allocated"); return -1; }

	if (validateBufferBox(&blit) == false) {
		printError("Invalid blit BufferBox");
		return -1;
	}
	int ret_val = 0;

	ret_val = frame_Transform_Generic_Fast(image, blit, FRAC);

	return ret_val;
}