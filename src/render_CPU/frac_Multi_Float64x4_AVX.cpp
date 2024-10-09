/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"
#include "frac_Multi_Float64x4_AVX.h"
#include "Float64x4/Float64x4.hpp"
#include "Float64x4/Float64x4_AVX.h"

#ifdef ENABLE_AVX_RENDERING

#ifndef __AVX__
	#error "__AVX__ is required to enable AVX rendering"
#endif

#include <immintrin.h>

/* 
	Directory:
	*	Debug Tools
	*	Function Definition AVX FP64x4
	*	Fractal Rendering AVX FP64x4
*/

/* Debug Tools */
	/* Floats */
		#define printf_m256x4_fp64x4(x) \
			printFlush("\n" STR_M(x) ": %lf %lf %lf %lf %lf %lf %lf %lf", \
				((fp64*)((void*)(&x)))[0], ((fp64*)((void*)(&x)))[1], ((fp64*)((void*)(&x)))[2], ((fp64*)((void*)(&x)))[3], \
				((fp64*)((void*)(&x)))[4], ((fp64*)((void*)(&x)))[5], ((fp64*)((void*)(&x)))[6], ((fp64*)((void*)(&x)))[7]\
			);

		#define printfInterval_m256x4_fp64x4(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %lf %lf %lf %lf %lf %lf %lf %lf", \
				((fp64*)((void*)(&x)))[0], ((fp64*)((void*)(&x)))[1], ((fp64*)((void*)(&x)))[2], ((fp64*)((void*)(&x)))[3], \
				((fp64*)((void*)(&x)))[4], ((fp64*)((void*)(&x)))[5], ((fp64*)((void*)(&x)))[6], ((fp64*)((void*)(&x)))[7]\
			);

	/* Integers */
		#define printf_m256x4_u64x4(x) \
			printFlush("\n" STR_M(x) ": %016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX", \
				((uint64_t*)((void*)(&x)))[0], ((uint64_t*)((void*)(&x)))[1], ((uint64_t*)((void*)(&x)))[2], ((uint64_t*)((void*)(&x)))[3], \
				((uint64_t*)((void*)(&x)))[4], ((uint64_t*)((void*)(&x)))[5], ((uint64_t*)((void*)(&x)))[6], ((uint64_t*)((void*)(&x)))[7]\
			);

		#define printfInterval_m256x4_u64x4(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX", \
				((uint64_t*)((void*)(&x)))[0], ((uint64_t*)((void*)(&x)))[1], ((uint64_t*)((void*)(&x)))[2], ((uint64_t*)((void*)(&x)))[3], \
				((uint64_t*)((void*)(&x)))[4], ((uint64_t*)((void*)(&x)))[5], ((uint64_t*)((void*)(&x)))[6], ((uint64_t*)((void*)(&x)))[7]\
			);

/* Function Definition AVX FP64 */

	#define color_pow_2_AVX_FP64x4(x) ((x) * (x))

	static void CPU_Interior_Coloring_AVX_FP64x4(
		fp64* outputColor, size_t index,
		const PreCalc_Param<Float64x4, fp64>& param,
		Float64x4 low
	) {
		outputColor[index + 0] += (fp64)color_pow_2_AVX_FP64x4(param.Interior_R_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos((fp64)log(low.val[0]) * param.Interior_R_Freq + param.Interior_R_Phase_mult_TAU)));
		outputColor[index + 1] += (fp64)color_pow_2_AVX_FP64x4(param.Interior_G_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos((fp64)log(low.val[0]) * param.Interior_G_Freq + param.Interior_G_Phase_mult_TAU)));
		outputColor[index + 2] += (fp64)color_pow_2_AVX_FP64x4(param.Interior_B_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos((fp64)log(low.val[0]) * param.Interior_B_Freq + param.Interior_B_Phase_mult_TAU)));
		outputColor[index + 3] += (fp64)color_pow_2_AVX_FP64x4(param.Interior_Alpha);
	}
	static void CPU_Exterior_Coloring_AVX_FP64x4(
		fp64* outputColor, size_t index,
		const PreCalc_Param<Float64x4, fp64>& param,
		uint32_t itr, Float64x4 zs, fp64 inverse_log2_power
	) {
		// TAU = 2 * PI
		fp64 smooth = log1p(fmax(0.0, (fp64)itr - log2((fp64)log2(zs.val[0]) / 2.0) * inverse_log2_power));
		outputColor[index + 0] += (fp64)color_pow_2_AVX_FP64x4(param.Exterior_R_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_R_Freq_mult_TAU * smooth + param.Exterior_R_Phase_mult_TAU)));
		outputColor[index + 1] += (fp64)color_pow_2_AVX_FP64x4(param.Exterior_G_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_G_Freq_mult_TAU * smooth + param.Exterior_G_Phase_mult_TAU)));
		outputColor[index + 2] += (fp64)color_pow_2_AVX_FP64x4(param.Exterior_B_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_B_Freq_mult_TAU * smooth + param.Exterior_B_Phase_mult_TAU)));
		outputColor[index + 3] += (fp64)color_pow_2_AVX_FP64x4(param.Exterior_Alpha);
	}
	
	#define Block_Init_AVX_FP64x4();

	#define Block_BeginLoop_AVX_FP64x4(); \
		constexpr size_t SIMD_Spacing = 4; /* 4 Registers in AVX FP64 */\
		\
		/* Load Precalculated constants */\
		const __m256dx4 realCord       = _mm256x4_set1_pdx4(param.realCord     );\
		const __m256dx4 imagCord       = _mm256x4_set1_pdx4(param.imagCord     );\
		const __m256dx4 realJulia      = _mm256x4_set1_pdx4(param.realJulia    );\
		const __m256dx4 imagJulia      = _mm256x4_set1_pdx4(param.imagJulia    );\
		const __m256dx4 rotSin_PC      = _mm256x4_set1_pdx4(param.rotSin_PC    );\
		const __m256dx4 rotCos_PC      = _mm256x4_set1_pdx4(param.rotCos_PC    );\
		const __m256dx4 breakoutValue  = _mm256x4_set1_pdx4(param.breakoutValue);\
		/* numX, numY, numZ, and numW are constants used to caluculate the cordinates */\
		const __m256dx4 numY           = _mm256x4_set1_pdx4(param.numY          );\
		const __m256dx4 numX           = _mm256x4_set1_pdx4(param.numX          );\
		const __m256dx4 recip_numZ     = _mm256x4_set1_pdx4(param.recip_numZ    );\
		const __m256dx4 neg_recip_numW = _mm256x4_set1_pdx4(param.neg_recip_numW);\
		\
		/* Init Loop */\
		size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS; /* Determines the starting image offset/index */\
		int32_t y = (int32_t)(p0 / (size_t)param.Image_ResX); /* Determines the starting y offset/index */\
		int32_t x = (int32_t)(p0 % (size_t)param.Image_ResX); /* Determines the starting x offset/index */\
		x *= param.sample; /* Scales x by the samples per pixel */\
		y *= param.sample; /* Scales x by the samples per pixel */\
		for (; y < param.Cord_ResY; y += param.sample) {\
			for (; x < param.Cord_ResX; x += (int32_t)SIMD_Spacing * param.sample) {\
				/* returns once the image offset/index reaches the end offset/index */\
				if (p0 >= p1 || ABORT_RENDERING == true) {\
					return;\
				}\
				size_t valuesToWrite = SIMD_Spacing;\
				if (((param.Cord_ResX - x) / param.sample) < (int32_t)SIMD_Spacing && ((param.Cord_ResX - x) / param.sample) > 0) {\
					valuesToWrite = (size_t)((param.Cord_ResX - x) / param.sample);\
				}\
				fp64 outputColor[IMAGE_BUFFER_CHANNELS * SIMD_Spacing] = {0.0};\
				/* Calculates 4 pixels (with super sampling) at a time */\
				for (int32_t v = 0; v < param.sample; v++) {\
					/* Calculates y cordinate-value */\
					Float64x4 y_val = (Float64x4)(fp64)y;\
					__m256dx4 yCord = _mm256x4_set1_pdx4(y_val);\
					yCord = _mm256x4_sub_pdx4(yCord, numY);\
					yCord = _mm256x4_mul_pdx4(yCord, neg_recip_numW);\
					\
					for (int32_t u = 0; u < param.sample; u++) {\
						/* Calculates the 4 x cordinate-values */\
						Float64x4 x_val[4] = {\
							(Float64x4)(fp64)(x + 3 * param.sample),\
							(Float64x4)(fp64)(x + 2 * param.sample),\
							(Float64x4)(fp64)(x + 1 * param.sample),\
							(Float64x4)(fp64)(x + 0 * param.sample) \
						};\
						__m256dx4 xCord = _mm256x4_set_pdx4(\
							x_val[0],\
							x_val[1],\
							x_val[2],\
							x_val[3]\
						);\
						xCord = _mm256x4_sub_pdx4(xCord, numX);\
						xCord = _mm256x4_mul_pdx4(xCord, recip_numZ);\
						\
						/* Transforms the x and y cordinate-values into the cordinates */\
						/* `cr` and `ci` are swapped with `zr` and `zi` if `param.juliaSet == true` */\
						__m256dx4 cr = (!param.juliaSet) ? _mm256x4_add_pdx4(_mm256x4_sub_pdx4(_mm256x4_mul_pdx4(xCord, rotCos_PC), _mm256x4_mul_pdx4(yCord, rotSin_PC)), realCord) : realJulia;\
						__m256dx4 ci = (!param.juliaSet) ? _mm256x4_add_pdx4(_mm256x4_add_pdx4(_mm256x4_mul_pdx4(yCord, rotCos_PC), _mm256x4_mul_pdx4(xCord, rotSin_PC)), imagCord) : imagJulia;\
						__m256dx4 zr = ( param.juliaSet) ? _mm256x4_add_pdx4(_mm256x4_sub_pdx4(_mm256x4_mul_pdx4(xCord, rotCos_PC), _mm256x4_mul_pdx4(yCord, rotSin_PC)), realCord) : realJulia;\
						__m256dx4 zi = ( param.juliaSet) ? _mm256x4_add_pdx4(_mm256x4_add_pdx4(_mm256x4_mul_pdx4(yCord, rotCos_PC), _mm256x4_mul_pdx4(xCord, rotSin_PC)), imagCord) : imagJulia;\
						\
						__m256dx4 low = _mm256x4_set1_pd(4.0);\
						__m256dx4 zs = _mm256x4_setzero_pdx4();\
						__m256dx4 __attribute__((unused)) temp_zr = _mm256x4_setzero_pdx4();\
						__m256dx4 current_value_mask = _mm256x4_set1_epi64x((long long)0xFFFFFFFFFFFFFFFF);\
						for (int i = (int)SIMD_Spacing - 1; i > (int)valuesToWrite - 1; i--) {\
							((Float64x4*)((void*)(&current_value_mask)))[i] = 0.0;\
						}\
						for (uint32_t itr = 0; itr < param.maxItr; itr++) {

	#define Block_EndLoop_AVX_FP64x4(); \
							/* Tracks the lowest value */\
							low = _mm256x4_min_pdx4(zs, low);\
							\
							/* Excludes any Z^2 values that have already exceeded the breakout value by setting them to 0.0 */\
							zs = _mm256x4_and_pdx4(zs, current_value_mask);\
							\
							/* Gets a mask with all the Z^2 values that exceeded the breakout value */\
							int break_mask = _mm256_movemask_pd(_mm256_cmpgt_pdx4(zs, breakoutValue));\
							\
							if (break_mask != 0) {\
								/* Z^2 values that exceeded the breakout value will use `Exterior_Coloring` */\
								for (int i = 0; i < (int)SIMD_Spacing; i++) {\
									if (break_mask & (1 << i)) {\
										fp64* zs_arr = (fp64*)((void*)(&zs));\
										fp64* cvmask_arr = (fp64*)((void*)(&current_value_mask));\
										Float64x4 zs_val;\
										zs_val.val[0] = zs_arr[i +  0];\
										zs_val.val[1] = zs_arr[i +  4];\
										zs_val.val[2] = zs_arr[i +  8];\
										zs_val.val[3] = zs_arr[i + 12];\
										CPU_Exterior_Coloring_AVX_FP64x4(\
											outputColor, (size_t)i * IMAGE_BUFFER_CHANNELS,\
											param,\
											itr, zs_val, param.inverse_log2_power\
										);\
										/* Removes the Z^2 value from the list of Z^2 values that have Not exceeded the breakout value */\
										cvmask_arr[i] = 0.0;\
										cvmask_arr[i + 4] = 0.0;\
										cvmask_arr[i + 8] = 0.0;\
										cvmask_arr[i + 12] = 0.0;\
									}\
								}\
								if (_mm256_movemask_pd(_mm256_cmpeq_pdx4(current_value_mask, _mm256x4_setzero_pdx4())) == 0) {\
									break; /* If all the Z^2 values have exceeded the breakout value, break the loop */\
								}\
							}\
						}\
						/* Checks if any values did Not exceed the breakout value */\
						int inside_value_mask = _mm256_movemask_pd(_mm256_cmpneq_pdx4(current_value_mask, _mm256x4_setzero_pdx4()));\
						if (inside_value_mask != 0) {\
							/* Z^2 values that did Not exceed the breakout value will use `Interior_Coloring` */\
							for (int i = 0; i < (int)SIMD_Spacing; i++) {\
								if (inside_value_mask & (1 << i)) {\
									fp64* low_arr = (fp64*)((void*)(&low));\
									Float64x4 low_val;\
									low_val.val[0] = low_arr[i +  0];\
									low_val.val[1] = low_arr[i +  4];\
									low_val.val[2] = low_arr[i +  8];\
									low_val.val[3] = low_arr[i + 12];\
									CPU_Interior_Coloring_AVX_FP64x4(\
										outputColor, (size_t)i * IMAGE_BUFFER_CHANNELS,\
										param,\
										low_val\
									);\
								}\
							}\
						}\
						x++;\
					}\
					x -= param.sample;\
					y++;\
				}\
				y -= param.sample;\
				\
				/* Code to write the colors to the image buffer */\
				\
				/* Divide by alpha and normalize colors */\
				for (size_t i = 0; i < SIMD_Spacing * IMAGE_BUFFER_CHANNELS; i += IMAGE_BUFFER_CHANNELS) {\
					if (outputColor[i + 3] != 0.0) {\
						outputColor[i + 0] = sqrt(outputColor[i + 0] / outputColor[i + 3]);\
						outputColor[i + 1] = sqrt(outputColor[i + 1] / outputColor[i + 3]);\
						outputColor[i + 2] = sqrt(outputColor[i + 2] / outputColor[i + 3]);\
						outputColor[i + 3] = sqrt(outputColor[i + 3] / param.alphaDiv);\
					}\
					outputColor[i + 0] *= 255.0;\
					outputColor[i + 1] *= 255.0;\
					outputColor[i + 2] *= 255.0;\
					outputColor[i + 3] *= 255.0;\
				}\
				for (size_t i = 0; i < valuesToWrite * IMAGE_BUFFER_CHANNELS; i += IMAGE_BUFFER_CHANNELS) {\
					/* Writes in the equivilant of SDL_PIXELFORMAT_ABGR8888 */\
					data[dataPtr] = (uint8_t)outputColor[i + 0]; dataPtr++;\
					data[dataPtr] = (uint8_t)outputColor[i + 1]; dataPtr++;\
					data[dataPtr] = (uint8_t)outputColor[i + 2]; dataPtr++;\
					data[dataPtr] = (uint8_t)outputColor[i + 3]; dataPtr++;\
				}\
				/* Increases the image offset/index */\
				p0 += valuesToWrite;\
			}\
			x = 0;\
		}

/* Fractal Rendering AVX FP64 */

	void quadraticRender_AVX_FP64x4(FractalParameters(Float64x4, fp64)) {

		Block_Init_AVX_FP64x4();
			__m256dx4 zr1, zr2, zi1, zi2;

			bool f[8]; // 8 bit formula
			for (uint8_t q = 0; q < 8; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			
			// Bits 0-2 will flip signage
			const __m256d s1 = (f[0]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s2 = (f[1]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s3 = (f[2]) ? _mm256_set1_pd(-2.0) : _mm256_set1_pd(2.0);

			// Bits 3-7 will apply fabs() via a mask
			const __m256dx4 zr1_mask = (f[3]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi1_mask = (f[4]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr2_mask = (f[5]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi2_mask = (f[6]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr_mask  = (f[7]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
		Block_BeginLoop_AVX_FP64x4();
			// Applies fabs() to zr and zi
			zr1 = _mm256x4_and_pdx4(zr1_mask, zr);
			zr2 = _mm256x4_and_pdx4(zr2_mask, zr);
			zi1 = _mm256x4_and_pdx4(zi1_mask, zi);
			zi2 = _mm256x4_and_pdx4(zi2_mask, zi);
			
			// Calculates the new zr and zi
			zr = _mm256x4_add_pdx4(_mm256x4_and_pdx4(zr_mask, _mm256x4_mul_pd_pdx4(s1,
				_mm256x4_sub_pdx4(
					_mm256x4_mul_pdx4(zr1, zr), 
					_mm256x4_mul_pd_pdx4(s2,
						_mm256x4_mul_pdx4(zi1, zi)
					)
				)
			)), cr);
			zi = _mm256x4_add_pdx4(_mm256x4_mul_pdx4_pd(_mm256x4_mul_pdx4(zr2, zi2), s3), ci);

			zs = _mm256x4_add_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi));
		
		Block_EndLoop_AVX_FP64x4();
	}

	void cubicRender_AVX_FP64x4(FractalParameters(Float64x4, fp64)) {
		
		Block_Init_AVX_FP64x4();

			__m256dx4 zr1, zr2, zr3, zi1, zi2, zi3;
			bool f[14];
			for (uint8_t q = 0; q < 14; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m256d s1 = (f[0]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s2 = (f[1]) ? _mm256_set1_pd(-3.0) : _mm256_set1_pd(3.0);
			const __m256d s3 = (f[2]) ? _mm256_set1_pd(-3.0) : _mm256_set1_pd(3.0);
			const __m256d s4 = (f[3]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s5 = (f[4]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s6 = (f[5]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			
			const __m256dx4 zr1_mask = (f[ 6]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi1_mask = (f[ 7]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr2_mask = (f[ 8]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi2_mask = (f[ 9]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr3_mask = (f[10]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi3_mask = (f[11]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr_mask  = (f[12]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi_mask  = (f[13]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);

		Block_BeginLoop_AVX_FP64x4();

			zr1 = _mm256x4_and_pdx4(zr1_mask, zr);
			zi1 = _mm256x4_and_pdx4(zi1_mask, zi);
			zr2 = _mm256x4_and_pdx4(zr2_mask, zr);
			zi2 = _mm256x4_and_pdx4(zi2_mask, zi);
			zr3 = _mm256x4_and_pdx4(zr3_mask, zr);
			zi3 = _mm256x4_and_pdx4(zi3_mask, zi);

			temp_zr = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s5, _mm256x4_and_pdx4(zr_mask,
				_mm256x4_sub_pdx4(
					_mm256x4_mul_pdx4(_mm256x4_mul_pd_pdx4(s1, zr1), _mm256x4_mul_pdx4(zr , zr)),
					_mm256x4_mul_pdx4(_mm256x4_mul_pd_pdx4(s2, zr2), _mm256x4_mul_pdx4(zi1, zi))
				)
			)), cr);
			zi      = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s6, _mm256x4_and_pdx4(zi_mask,
				_mm256x4_sub_pdx4(
					_mm256x4_mul_pdx4(_mm256x4_mul_pd_pdx4(s3, zr3), _mm256x4_mul_pdx4(zr, zi2)),
					_mm256x4_mul_pdx4(_mm256x4_mul_pd_pdx4(s4, zi3), _mm256x4_mul_pdx4(zi, zi ))
				)
			)), ci);
			zr = temp_zr;

			zs = _mm256x4_add_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi));
		
		Block_EndLoop_AVX_FP64x4();
	}

	void quarticRender_AVX_FP64x4(FractalParameters(Float64x4, fp64)) {

		Block_Init_AVX_FP64x4();

			__m256dx4 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4;
			bool f[17];
			for (uint8_t q = 0; q < 17; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m256d s1 = (f[0]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s2 = (f[1]) ? _mm256_set1_pd(-6.0) : _mm256_set1_pd(6.0);
			const __m256d s3 = (f[2]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s4 = (f[3]) ? _mm256_set1_pd(-4.0) : _mm256_set1_pd(4.0);
			const __m256d s5 = (f[4]) ? _mm256_set1_pd(-4.0) : _mm256_set1_pd(4.0);
			const __m256d s6 = (f[5]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			const __m256d s7 = (f[6]) ? _mm256_set1_pd(-1.0) : _mm256_set1_pd(1.0);
			
			const __m256dx4 zr1_mask = (f[ 7]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi1_mask = (f[ 8]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr2_mask = (f[ 9]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi2_mask = (f[10]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr3_mask = (f[11]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi3_mask = (f[12]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr4_mask = (f[13]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi4_mask = (f[14]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr_mask  = (f[15]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi_mask  = (f[16]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);

		Block_BeginLoop_AVX_FP64x4();

			zr1 = _mm256x4_and_pdx4(zr1_mask, zr);
			zi1 = _mm256x4_and_pdx4(zi1_mask, zi);
			zr2 = _mm256x4_and_pdx4(zr2_mask, zr);
			zi2 = _mm256x4_and_pdx4(zi2_mask, zi);
			zr3 = _mm256x4_and_pdx4(zr3_mask, zr);
			zi3 = _mm256x4_and_pdx4(zi3_mask, zi);
			zr4 = _mm256x4_and_pdx4(zr4_mask, zr);
			zi4 = _mm256x4_and_pdx4(zi4_mask, zi);

			temp_zr = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s6, _mm256x4_and_pdx4(zr_mask,
				_mm256x4_add_pdx4(
					_mm256x4_sub_pdx4(
						_mm256x4_mul_pd_pdx4(s1,
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr1, zr ), _mm256x4_mul_pdx4(zr, zr))
						),
						_mm256x4_mul_pd_pdx4(s2,
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr2, zi1), _mm256x4_mul_pdx4(zr, zi))
						)
					),
					_mm256x4_mul_pd_pdx4(s3,
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zi2, zi), _mm256x4_mul_pdx4(zi, zi))
					)
				)
			)), cr);
			zi      = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s7, _mm256x4_and_pdx4(zi_mask,
				_mm256x4_sub_pdx4(
					_mm256x4_mul_pd_pdx4(s4,
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr3, zi3), _mm256x4_mul_pdx4(zr, zr))
					),
					_mm256x4_mul_pd_pdx4(s5,
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr4, zi4), _mm256x4_mul_pdx4(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm256x4_add_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi));

		Block_EndLoop_AVX_FP64x4();
	}

	void quinticRender_AVX_FP64x4(FractalParameters(Float64x4, fp64)) {

		Block_Init_AVX_FP64x4();

			__m256dx4 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5;
			bool fS[6];
			bool fA[10];
			bool fO[4];
			for (uint8_t q = 0; q < 6; q++) { /* 0-5 */ 
				fS[q] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 6; q < 8; q++) { /* 6-7 */ 
				fO[q - 6] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 8; q < 18; q++) { /* 8-17 */ 
				fA[q - 8] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 18; q < 20; q++) { /* 18-19 */ 
				fO[q - 16] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m256d s1 = (fS[0]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s2 = (fS[1]) ? _mm256_set1_pd(-10.0) : _mm256_set1_pd(10.0);
			const __m256d s3 = (fS[2]) ? _mm256_set1_pd(-5.0 ) : _mm256_set1_pd(5.0 );
			const __m256d s4 = (fS[3]) ? _mm256_set1_pd(-5.0 ) : _mm256_set1_pd(5.0 );
			const __m256d s5 = (fS[4]) ? _mm256_set1_pd(-10.0) : _mm256_set1_pd(10.0);
			const __m256d s6 = (fS[5]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s7 = (fO[0]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s8 = (fO[1]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );

			const __m256dx4 zr1_mask = (fA[0]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi1_mask = (fA[1]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr2_mask = (fA[2]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi2_mask = (fA[3]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr3_mask = (fA[4]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi3_mask = (fA[5]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr4_mask = (fA[6]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi4_mask = (fA[7]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr5_mask = (fA[8]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi5_mask = (fA[9]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr_mask  = (fO[2]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi_mask  = (fO[3]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);

		Block_BeginLoop_AVX_FP64x4();

			zr1 = _mm256x4_and_pdx4(zr1_mask, zr);
			zi1 = _mm256x4_and_pdx4(zi1_mask, zi);
			zr2 = _mm256x4_and_pdx4(zr2_mask, zr);
			zi2 = _mm256x4_and_pdx4(zi2_mask, zi);
			zr3 = _mm256x4_and_pdx4(zr3_mask, zr);
			zi3 = _mm256x4_and_pdx4(zi3_mask, zi);
			zr4 = _mm256x4_and_pdx4(zr4_mask, zr);
			zi4 = _mm256x4_and_pdx4(zi4_mask, zi);
			zr5 = _mm256x4_and_pdx4(zr5_mask, zr);
			zi5 = _mm256x4_and_pdx4(zi5_mask, zi);

			temp_zr = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s7, _mm256x4_and_pdx4(zr_mask ,
				_mm256x4_add_pdx4(
					_mm256x4_sub_pdx4(
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s1, zr1),
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zr , zr))
						),
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s2, _mm256x4_mul_pdx4(zr2, zi1)),
							_mm256x4_mul_pdx4(zi, _mm256x4_mul_pdx4(zr , zr ))
						)
					),
					_mm256x4_mul_pdx4(
						_mm256x4_mul_pd_pdx4(s3, _mm256x4_mul_pdx4(zr3, zi2)),
						_mm256x4_mul_pdx4(zi, _mm256x4_mul_pdx4(zi , zi ))
					)
				)
			)), cr);
			zi      = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s8, _mm256x4_and_pdx4(zi_mask,
				_mm256x4_add_pdx4(
					_mm256x4_sub_pdx4(
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s4 , _mm256x4_mul_pdx4(zr4, zr)),
							_mm256x4_mul_pdx4(zi3, _mm256x4_mul_pdx4(zr , zr))
						),
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s5 , _mm256x4_mul_pdx4(zr5, zr)),
							_mm256x4_mul_pdx4(zi4, _mm256x4_mul_pdx4(zi , zi))
						)
					),
					_mm256x4_mul_pdx4(
						_mm256x4_mul_pd_pdx4(s6 , zi5),
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zi, zi), _mm256x4_mul_pdx4(zi , zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm256x4_add_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi));

		Block_EndLoop_AVX_FP64x4();
	}

	void sexticRender_AVX_FP64x4(FractalParameters(Float64x4, fp64)) {

		Block_Init_AVX_FP64x4();

			__m256dx4 zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6;
			bool fS[7];
			bool fA[12];
			bool fO[4];
			for (uint8_t q = 0; q <= 6; q++) { /* 0-6 */ 
				fS[q] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 7; q <= 8; q++) { /* 7-8 */ 
				fO[q - 7] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 9; q <= 20; q++) { /* 9-20 */ 
				fA[q - 9] = ((param.formula >> q) & 1) ? true : false;
			}
			for (uint8_t q = 21; q <= 22; q++) { /* 21-22 */ 
				fO[q - 19] = ((param.formula >> q) & 1) ? true : false;
			}

			const __m256d s1 = (fS[0]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s2 = (fS[1]) ? _mm256_set1_pd(-15.0) : _mm256_set1_pd(15.0);
			const __m256d s3 = (fS[2]) ? _mm256_set1_pd(-15.0) : _mm256_set1_pd(15.0);
			const __m256d s4 = (fS[3]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s5 = (fS[4]) ? _mm256_set1_pd(-6.0 ) : _mm256_set1_pd(6.0 );
			const __m256d s6 = (fS[5]) ? _mm256_set1_pd(-20.0) : _mm256_set1_pd(20.0);
			const __m256d s7 = (fS[6]) ? _mm256_set1_pd(-6.0 ) : _mm256_set1_pd(6.0 );
			const __m256d s8 = (fO[0]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
			const __m256d s9 = (fO[1]) ? _mm256_set1_pd(-1.0 ) : _mm256_set1_pd(1.0 );
		
			const __m256dx4 zr1_mask = (fA[ 0]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi1_mask = (fA[ 1]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr2_mask = (fA[ 2]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi2_mask = (fA[ 3]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr3_mask = (fA[ 4]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi3_mask = (fA[ 5]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr4_mask = (fA[ 6]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi4_mask = (fA[ 7]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr5_mask = (fA[ 8]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi5_mask = (fA[ 9]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr6_mask = (fA[10]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi6_mask = (fA[11]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zr_mask  = (fO[ 2]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
			const __m256dx4 zi_mask  = (fO[ 3]) ? _mm256x4_set1_epi64x((int64_t)0x7FFFFFFFFFFFFFFF) : _mm256x4_set1_epi64x((int64_t)0xFFFFFFFFFFFFFFFF);
		
		Block_BeginLoop_AVX_FP64x4();

			zr1 = _mm256x4_and_pdx4(zr1_mask, zr);
			zi1 = _mm256x4_and_pdx4(zi1_mask, zi);
			zr2 = _mm256x4_and_pdx4(zr2_mask, zr);
			zi2 = _mm256x4_and_pdx4(zi2_mask, zi);
			zr3 = _mm256x4_and_pdx4(zr3_mask, zr);
			zi3 = _mm256x4_and_pdx4(zi3_mask, zi);
			zr4 = _mm256x4_and_pdx4(zr4_mask, zr);
			zi4 = _mm256x4_and_pdx4(zi4_mask, zi);
			zr5 = _mm256x4_and_pdx4(zr5_mask, zr);
			zi5 = _mm256x4_and_pdx4(zi5_mask, zi);
			zr6 = _mm256x4_and_pdx4(zr6_mask, zr);
			zi6 = _mm256x4_and_pdx4(zi6_mask, zi);

			temp_zr = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s8, _mm256x4_and_pdx4(zr_mask ,
				_mm256x4_sub_pdx4(
					_mm256x4_add_pdx4(
						_mm256x4_sub_pdx4(
							_mm256x4_mul_pdx4(
								_mm256x4_mul_pd_pdx4(s1, _mm256x4_mul_pdx4(zr1, zr)),
								_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zr, zr))
							),
							_mm256x4_mul_pdx4(
								_mm256x4_mul_pd_pdx4(s2, _mm256x4_mul_pdx4(zr2, zi1)),
								_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zr, zi))
							)
						),
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s3, _mm256x4_mul_pdx4(zr3, zi2)),
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zi), _mm256x4_mul_pdx4(zi, zi))
						)
					),
					_mm256x4_mul_pdx4(
						_mm256x4_mul_pd_pdx4(s4, _mm256x4_mul_pdx4(zi3, zi)),
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zi, zi), _mm256x4_mul_pdx4(zi, zi))
					)
				)
			)), cr);
			zi      = _mm256x4_add_pdx4(_mm256x4_mul_pd_pdx4(s9, _mm256x4_and_pdx4(zi_mask,
				_mm256x4_add_pdx4(
					_mm256x4_sub_pdx4(
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s5, _mm256x4_mul_pdx4(zr4, zi4)),
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zr, zr))
						),
						_mm256x4_mul_pdx4(
							_mm256x4_mul_pd_pdx4(s6, _mm256x4_mul_pdx4(zr5, zi5)),
							_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi))
						)
					),
					_mm256x4_mul_pdx4(
						_mm256x4_mul_pd_pdx4(s7, _mm256x4_mul_pdx4(zr6, zi6)),
						_mm256x4_mul_pdx4(_mm256x4_mul_pdx4(zi, zi), _mm256x4_mul_pdx4(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm256x4_add_pdx4(_mm256x4_mul_pdx4(zr, zr), _mm256x4_mul_pdx4(zi, zi));

		Block_EndLoop_AVX_FP64x4();
	}

#endif /* ENABLE_AVX_RENDERING */