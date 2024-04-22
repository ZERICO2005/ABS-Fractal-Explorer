/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"
#include "frac_Multi_SSE2.h"

#ifdef ENABLE_SSE2_RENDERING

#include <xmmintrin.h>

/* 
	Directory:
	*	Debug Tools
	*	Function Definition SSE2 FP64
	*	Function Definition SSE2 FP32
	*	Fractal Rendering SSE2 FP32
	*	Fractal Rendering SSE2 FP64
*/

/* Debug Tools */
	/* Floats */
		#define printf_m128_fp32(x) \
			printFlush("\n" STR_M(x) ": %f %f %f %f", ((fp32*)((void*)(&x)))[0], ((fp32*)((void*)(&x)))[1], ((fp32*)((void*)(&x)))[2], ((fp32*)((void*)(&x)))[3]);

		#define printfInterval_m128_fp32(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %f %f %f %f", ((fp32*)((void*)(&x)))[0], ((fp32*)((void*)(&x)))[1], ((fp32*)((void*)(&x)))[2], ((fp32*)((void*)(&x)))[3]);

		#define printf_m128_fp64(x) \
			printFlush("\n" STR_M(x) ": %lf %lf", ((fp64*)((void*)(&x)))[0], ((fp64*)((void*)(&x)))[1]);

		#define printfInterval_m128_fp64(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %lf %lf", ((fp64*)((void*)(&x)))[0], ((fp64*)((void*)(&x)))[1]);
	/* Integers */
		#define printf_m128_u32(x) \
			printFlush("\n" STR_M(x) ": %08X %08X %08X %08X", ((uint32_t*)((void*)(&x)))[0], ((uint32_t*)((void*)(&x)))[1], ((uint32_t*)((void*)(&x)))[2], ((uint32_t*)((void*)(&x)))[3]);

		#define printfInterval_m128_u32(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %08X %08X %08X %08X", ((uint32_t*)((void*)(&x)))[0], ((uint32_t*)((void*)(&x)))[1], ((uint32_t*)((void*)(&x)))[2], ((uint32_t*)((void*)(&x)))[3]);

		#define printf_m128_u64(x) \
			printFlush("\n" STR_M(x) ": %016X %016X", ((uint64_t*)((void*)(&x)))[0], ((uint64_t*)((void*)(&x)))[1]);

		#define printfInterval_m128_u64(freq, x) \
			printfInterval(freq, "\n" STR_M(x) ": %016X %016X", ((uint64_t*)((void*)(&x)))[0], ((uint64_t*)((void*)(&x)))[1]);

/* Function Definition SSE2 FP64 */

	void CPU_Interior_Coloring_SSE2_FP64(
		fp64& outR, fp64& outG, fp64& outB, fp64& outA,
		const PreCalc_Param<fp64, fp64>& param,
		fp64 low
	) {
		outR += param.Interior_R_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log(low) * param.Interior_R_Freq + param.Interior_R_Phase));
		outG += param.Interior_G_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log(low) * param.Interior_G_Freq + param.Interior_G_Phase));
		outB += param.Interior_B_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log(low) * param.Interior_B_Freq + param.Interior_B_Phase));
		outA += param.Interior_Alpha;
	}
				
	void CPU_Exterior_Coloring_SSE2_FP64(
		fp64& outR, fp64& outG, fp64& outB, fp64& outA,
		const PreCalc_Param<fp64, fp64>& param,
		uint32_t itr, fp64 zs, fp64 inverse_log2_power
	) {
		// TAU = 2 * PI
		fp64 smooth = log1p(fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) * inverse_log2_power));
		outR += param.Exterior_R_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_R_Freq_mult_TAU * smooth + param.Exterior_R_Phase_mult_TAU));
		outG += param.Exterior_G_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_G_Freq_mult_TAU * smooth + param.Exterior_G_Phase_mult_TAU));
		outB += param.Exterior_B_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_B_Freq_mult_TAU * smooth + param.Exterior_B_Phase_mult_TAU));
		outA += param.Exterior_Alpha;
	}
	
	#define Block_Init_SSE2_FP64();

	#define Block_BeginLoop_SSE2_FP64(); \
		constexpr size_t SIMD_Spacing = 2; /* 2 Registers in SSE2 FP64 */\
		\
		/* Load Precalculated constants */\
		const __m128d realCord       = _mm_set_pd1(param.realCord     );\
		const __m128d imagCord       = _mm_set_pd1(param.imagCord     );\
		const __m128d realJulia      = _mm_set_pd1(param.realJulia    );\
		const __m128d imagJulia      = _mm_set_pd1(param.imagJulia    );\
		const __m128d rotSin_PC      = _mm_set_pd1(param.rotSin_PC    );\
		const __m128d rotCos_PC      = _mm_set_pd1(param.rotCos_PC    );\
		const __m128d breakoutValue  = _mm_set_pd1(param.breakoutValue);\
		/* numX, numY, numZ, and numW are constants used to caluculate the cordinates */\
		const __m128d numY           = _mm_set_pd1(param.numY          );\
		const __m128d numX           = _mm_set_pd1(param.numX          );\
		const __m128d recip_numZ     = _mm_set_pd1(param.recip_numZ    );\
		const __m128d neg_recip_numW = _mm_set_pd1(param.neg_recip_numW);\
		/* Constant used to divide Alpha (sample * sample) */\
		const __m128d alphaDiv       = _mm_set_pd1(param.alphaDiv      );\
		/* Color Values */\
		/* const __m128d Exterior_Alpha = _mm_set_pd1(param.Exterior_Alpha); */\
			/* const __m128d Exterior_R_Amp_mult_Exterior_Alpha = _mm_set_pd1(param.Exterior_R_Amp_mult_Exterior_Alpha); */\
			/* const __m128d Exterior_R_Freq_mult_TAU           = _mm_set_pd1(param.Exterior_R_Freq_mult_TAU          ); */\
			/* const __m128d Exterior_R_Phase_mult_TAU          = _mm_set_pd1(param.Exterior_R_Phase_mult_TAU         ); */\
			/* const __m128d Exterior_G_Amp_mult_Exterior_Alpha = _mm_set_pd1(param.Exterior_G_Amp_mult_Exterior_Alpha); */\
			/* const __m128d Exterior_G_Freq_mult_TAU           = _mm_set_pd1(param.Exterior_G_Freq_mult_TAU          ); */\
			/* const __m128d Exterior_G_Phase_mult_TAU          = _mm_set_pd1(param.Exterior_G_Phase_mult_TAU         ); */\
			/* const __m128d Exterior_B_Amp_mult_Exterior_Alpha = _mm_set_pd1(param.Exterior_B_Amp_mult_Exterior_Alpha); */\
			/* const __m128d Exterior_B_Freq_mult_TAU           = _mm_set_pd1(param.Exterior_B_Freq_mult_TAU          ); */\
			/* const __m128d Exterior_B_Phase_mult_TAU          = _mm_set_pd1(param.Exterior_B_Phase_mult_TAU         ); */\
		/* const __m128d Interior_Alpha = _mm_set_pd1(param.Exterior_Alpha); */\
			/* const __m128d Interior_R_Amp_mult_Interior_Alpha = _mm_set_pd1(param.Interior_R_Amp_mult_Interior_Alpha); */\
			/* const __m128d Interior_R_Freq                    = _mm_set_pd1(param.Interior_R_Freq                   ); */\
			/* const __m128d Interior_R_Phase                   = _mm_set_pd1(param.Interior_R_Phase                  ); */\
			/* const __m128d Interior_G_Amp_mult_Interior_Alpha = _mm_set_pd1(param.Interior_G_Amp_mult_Interior_Alpha); */\
			/* const __m128d Interior_G_Freq                    = _mm_set_pd1(param.Interior_G_Freq                   ); */\
			/* const __m128d Interior_G_Phase                   = _mm_set_pd1(param.Interior_G_Phase                  ); */\
			/* const __m128d Interior_B_Amp_mult_Interior_Alpha = _mm_set_pd1(param.Interior_B_Amp_mult_Interior_Alpha); */\
			/* const __m128d Interior_B_Freq                    = _mm_set_pd1(param.Interior_B_Freq                   ); */\
			/* const __m128d Interior_B_Phase                   = _mm_set_pd1(param.Interior_B_Phase                  ); */\
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
				/* Store the output color values */\
					__m128d outR = _mm_setzero_pd();\
					__m128d outG = _mm_setzero_pd();\
					__m128d outB = _mm_setzero_pd();\
					__m128d outA = _mm_setzero_pd();\
				/* Calculates 4 pixels (with super sampling) at a time */\
				for (int32_t v = 0; v < param.sample; v++) {\
					/* Calculates y cordinate-value */\
					__m128d yCord = _mm_set_pd1((fp64)y);\
					yCord = _mm_sub_pd(yCord, numY);\
					yCord = _mm_mul_pd(yCord, neg_recip_numW);\
					\
					for (int32_t u = 0; u < param.sample; u++) {\
						/* Calculates the 4 x cordinate-values */\
						__m128d xCord = _mm_set_pd(\
							(fp64)(x + 1 * param.sample),\
							(fp64)(x + 0 * param.sample)\
						);\
						xCord = _mm_sub_pd(xCord, numX);\
						xCord = _mm_mul_pd(xCord, recip_numZ);\
						\
						/* Transforms the x and y cordinate-values into the cordinates */\
						/* `cr` and `ci` are swapped with `zr` and `zi` if `param.juliaSet == true` */\
						__m128d cr = (!param.juliaSet) ? _mm_add_pd(_mm_sub_pd(_mm_mul_pd(xCord, rotCos_PC), _mm_mul_pd(yCord, rotSin_PC)), realCord) : realJulia;\
						__m128d ci = (!param.juliaSet) ? _mm_add_pd(_mm_add_pd(_mm_mul_pd(yCord, rotCos_PC), _mm_mul_pd(xCord, rotSin_PC)), imagCord) : imagJulia;\
						__m128d zr = (param.juliaSet) ? _mm_add_pd(_mm_sub_pd(_mm_mul_pd(xCord, rotCos_PC), _mm_mul_pd(yCord, rotSin_PC)), realCord) : realJulia;\
						__m128d zi = (param.juliaSet) ? _mm_add_pd(_mm_add_pd(_mm_mul_pd(yCord, rotCos_PC), _mm_mul_pd(xCord, rotSin_PC)), imagCord) : imagJulia;\
						\
						__m128d low = _mm_set_pd1(4.0);\
						__m128d zs = _mm_setzero_pd();\
						__m128d __attribute__((unused)) temp_zr = _mm_setzero_pd();\
						__m128d current_value_mask = _mm_cmpeq_pd(_mm_setzero_pd(), _mm_setzero_pd());\
						for (int i = (int)SIMD_Spacing - 1; i > (int)valuesToWrite - 1; i--) {\
							((fp32*)((void*)(&current_value_mask)))[i] = 0.0f;\
						}\
						for (uint32_t itr = 0; itr < param.maxItr; itr++) {

	#define Block_EndLoop_SSE2_FP64(); \
							/* Tracks the lowest value */\
							low = _mm_min_pd(zs, low);\
							\
							/* Excludes any Z^2 values that have already exceeded the breakout value by setting them to 0.0 */\
							zs = _mm_and_pd(zs, current_value_mask);\
							/* Gets a mask with all the Z^2 values that exceeded the breakout value */\
							int break_mask = _mm_movemask_pd(_mm_cmpgt_pd(zs, breakoutValue));\
							\
							if (break_mask != 0) {\
								/* Z^2 values that exceeded the breakout value will use `Exterior_Coloring` */\
								for (int i = 0; i < (int)SIMD_Spacing; i++) {\
									if (break_mask & (1 << i)) {\
										CPU_Exterior_Coloring_SSE2_FP64(\
											((fp64*)((void*)(&outR)))[i], ((fp64*)((void*)(&outG)))[i], ((fp64*)((void*)(&outB)))[i], ((fp64*)((void*)(&outA)))[i],\
											param,\
											itr, ((fp64*)((void*)(&zs)))[i], param.inverse_log2_power\
										);\
										((fp64*)((void*)(&current_value_mask)))[i] = 0.0; /* Removes the Z^2 value from the list of Z^2 values that have Not exceeded the breakout value */\
									}\
								}\
								if (_mm_movemask_pd(_mm_cmpeq_pd(current_value_mask, _mm_setzero_pd())) == 0) {\
									break; /* If all the Z^2 values have exceeded the breakout value, break the loop */\
								}\
							}\
						}\
						/* Checks if any values did Not exceed the breakout value */\
						int inside_value_mask = _mm_movemask_pd(_mm_cmpneq_pd(current_value_mask, _mm_setzero_pd()));\
						if (inside_value_mask != 0) {\
							/* Z^2 values that did Not exceed the breakout value will use `Interior_Coloring` */\
							for (int i = 0; i < (int)SIMD_Spacing; i++) {\
								if (inside_value_mask & (1 << i)) {\
									CPU_Interior_Coloring_SSE2_FP64(\
										((fp64*)((void*)(&outR)))[i], ((fp64*)((void*)(&outG)))[i], ((fp64*)((void*)(&outB)))[i], ((fp64*)((void*)(&outA)))[i],\
										param,\
										((fp64*)((void*)(&low)))[i]\
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
				/* Divide R, G, and B by Alpha */\
					outR = _mm_div_pd(outR, outA);\
					outG = _mm_div_pd(outG, outA);\
					outB = _mm_div_pd(outB, outA);\
				/* If division by zero occured, set values to 0 */\
					const __m128d div_zero_mask = _mm_cmpeq_pd(outA, _mm_setzero_pd());\
					outR = _mm_andnot_pd(div_zero_mask, outR);\
					outG = _mm_andnot_pd(div_zero_mask, outG);\
					outB = _mm_andnot_pd(div_zero_mask, outB);\
				\
				/* Normilizes alpha to 0.0 - 1.0 */\
					outA = _mm_div_pd(outA, alphaDiv);\
				/* Normilizes alpha to 0.0 - 1.0 */\
					const __m128d color_mult = _mm_set_pd1(255.0);\
					outR = _mm_mul_pd(outR, color_mult);\
					outG = _mm_mul_pd(outG, color_mult);\
					outB = _mm_mul_pd(outB, color_mult);\
					outA = _mm_mul_pd(outA, color_mult);\
				\
				/* Writes colors in the order 3, 2, 1, 0. I am not sure why it had to be reversed */\
				for (size_t i = 0; i < valuesToWrite; i++) {\
					/* Writes in the equivilant of SDL_PIXELFORMAT_ABGR8888 */\
					data[dataPtr] = (uint8_t)((fp64*)((void*)(&outR)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp64*)((void*)(&outG)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp64*)((void*)(&outB)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp64*)((void*)(&outA)))[i]; dataPtr++;\
				}\
				/* Increases the image offset/index */\
				p0 += valuesToWrite;\
			}\
			x = 0;\
		}

/* Function Definition SSE2 FP32 */

	void CPU_Interior_Coloring_SSE2_FP32(
		fp32& outR, fp32& outG, fp32& outB, fp32& outA,
		const PreCalc_Param<fp32,fp32>& param,
		fp32 low
	) {
		outR += param.Interior_R_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.Interior_R_Freq + param.Interior_R_Phase));
		outG += param.Interior_G_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.Interior_G_Freq + param.Interior_G_Phase));
		outB += param.Interior_B_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.Interior_B_Freq + param.Interior_B_Phase));
		outA += param.Interior_Alpha;
	}
				
	void CPU_Exterior_Coloring_SSE2_FP32(
		fp32& outR, fp32& outG, fp32& outB, fp32& outA,
		const PreCalc_Param<fp32,fp32>& param,
		uint32_t itr, fp32 zs, fp32 inverse_log2_power
	) {
		// TAU = 2 * PI
		fp32 smooth = log1pf(fmaxf(0.0f, (fp32)itr - log2f(log2f(zs) / 2.0f) * inverse_log2_power));
		outR += param.Exterior_R_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5f - 0.5f * cosf(param.Exterior_R_Freq_mult_TAU * smooth + param.Exterior_R_Phase_mult_TAU));
		outG += param.Exterior_G_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5f - 0.5f * cosf(param.Exterior_G_Freq_mult_TAU * smooth + param.Exterior_G_Phase_mult_TAU));
		outB += param.Exterior_B_Amp_mult_Exterior_Alpha * param.Exterior_Alpha * (0.5f - 0.5f * cosf(param.Exterior_B_Freq_mult_TAU * smooth + param.Exterior_B_Phase_mult_TAU));
		outA += param.Exterior_Alpha;
	}

	#define Block_Init_SSE2_FP32();

	#define Block_BeginLoop_SSE2_FP32(); \
		constexpr size_t SIMD_Spacing = 4; /* 4 Registers in SSE2 FP32 */\
		\
		/* Load Precalculated constants */\
		const __m128 realCord       = _mm_set_ps1(param.realCord     );\
		const __m128 imagCord       = _mm_set_ps1(param.imagCord     );\
		const __m128 realJulia      = _mm_set_ps1(param.realJulia    );\
		const __m128 imagJulia      = _mm_set_ps1(param.imagJulia    );\
		const __m128 rotSin_PC      = _mm_set_ps1(param.rotSin_PC    );\
		const __m128 rotCos_PC      = _mm_set_ps1(param.rotCos_PC    );\
		const __m128 breakoutValue  = _mm_set_ps1(param.breakoutValue);\
		/* numX, numY, numZ, and numW are constants used to caluculate the cordinates */\
		const __m128 numY           = _mm_set_ps1(param.numY          );\
		const __m128 numX           = _mm_set_ps1(param.numX          );\
		const __m128 recip_numZ     = _mm_set_ps1(param.recip_numZ    );\
		const __m128 neg_recip_numW = _mm_set_ps1(param.neg_recip_numW);\
		/* Constant used to divide Alpha (sample * sample) */\
		const __m128 alphaDiv       = _mm_set_ps1(param.alphaDiv      );\
		/* Color Values */\
		/* const __m128 Exterior_Alpha = _mm_set_ps1(param.Exterior_Alpha); */\
			/* const __m128 Exterior_R_Amp_mult_Exterior_Alpha = _mm_set_ps1(param.Exterior_R_Amp_mult_Exterior_Alpha); */\
			/* const __m128 Exterior_R_Freq_mult_TAU           = _mm_set_ps1(param.Exterior_R_Freq_mult_TAU          ); */\
			/* const __m128 Exterior_R_Phase_mult_TAU          = _mm_set_ps1(param.Exterior_R_Phase_mult_TAU         ); */\
			/* const __m128 Exterior_G_Amp_mult_Exterior_Alpha = _mm_set_ps1(param.Exterior_G_Amp_mult_Exterior_Alpha); */\
			/* const __m128 Exterior_G_Freq_mult_TAU           = _mm_set_ps1(param.Exterior_G_Freq_mult_TAU          ); */\
			/* const __m128 Exterior_G_Phase_mult_TAU          = _mm_set_ps1(param.Exterior_G_Phase_mult_TAU         ); */\
			/* const __m128 Exterior_B_Amp_mult_Exterior_Alpha = _mm_set_ps1(param.Exterior_B_Amp_mult_Exterior_Alpha); */\
			/* const __m128 Exterior_B_Freq_mult_TAU           = _mm_set_ps1(param.Exterior_B_Freq_mult_TAU          ); */\
			/* const __m128 Exterior_B_Phase_mult_TAU          = _mm_set_ps1(param.Exterior_B_Phase_mult_TAU         ); */\
		/* const __m128 Interior_Alpha = _mm_set_ps1(param.Exterior_Alpha); */\
			/* const __m128 Interior_R_Amp_mult_Interior_Alpha = _mm_set_ps1(param.Interior_R_Amp_mult_Interior_Alpha); */\
			/* const __m128 Interior_R_Freq                    = _mm_set_ps1(param.Interior_R_Freq                   ); */\
			/* const __m128 Interior_R_Phase                   = _mm_set_ps1(param.Interior_R_Phase                  ); */\
			/* const __m128 Interior_G_Amp_mult_Interior_Alpha = _mm_set_ps1(param.Interior_G_Amp_mult_Interior_Alpha); */\
			/* const __m128 Interior_G_Freq                    = _mm_set_ps1(param.Interior_G_Freq                   ); */\
			/* const __m128 Interior_G_Phase                   = _mm_set_ps1(param.Interior_G_Phase                  ); */\
			/* const __m128 Interior_B_Amp_mult_Interior_Alpha = _mm_set_ps1(param.Interior_B_Amp_mult_Interior_Alpha); */\
			/* const __m128 Interior_B_Freq                    = _mm_set_ps1(param.Interior_B_Freq                   ); */\
			/* const __m128 Interior_B_Phase                   = _mm_set_ps1(param.Interior_B_Phase                  ); */\
		/* Init Loop */\
		\
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
				/* Store the output color values */\
					__m128 outR = _mm_setzero_ps();\
					__m128 outG = _mm_setzero_ps();\
					__m128 outB = _mm_setzero_ps();\
					__m128 outA = _mm_setzero_ps();\
				/* Calculates 4 pixels (with super sampling) at a time */\
				for (int32_t v = 0; v < param.sample; v++) {\
					/* Calculates y cordinate-value */\
					__m128 yCord = _mm_set_ps1((fp32)y);\
					yCord = _mm_sub_ps(yCord, numY);\
					yCord = _mm_mul_ps(yCord, neg_recip_numW);\
					\
					for (int32_t u = 0; u < param.sample; u++) {\
						/* Calculates the 4 x cordinate-values */\
						__m128 xCord = _mm_set_ps(\
							(fp32)(x + 3 * param.sample),\
							(fp32)(x + 2 * param.sample),\
							(fp32)(x + 1 * param.sample),\
							(fp32)(x + 0 * param.sample)\
						);\
						xCord = _mm_sub_ps(xCord, numX);\
						xCord = _mm_mul_ps(xCord, recip_numZ);\
						\
						/* Transforms the x and y cordinate-values into the cordinates */\
						/* `cr` and `ci` are swapped with `zr` and `zi` if `param.juliaSet == true` */\
						__m128 cr = (!param.juliaSet) ? _mm_add_ps(_mm_sub_ps(_mm_mul_ps(xCord, rotCos_PC), _mm_mul_ps(yCord, rotSin_PC)), realCord) : realJulia;\
						__m128 ci = (!param.juliaSet) ? _mm_add_ps(_mm_add_ps(_mm_mul_ps(yCord, rotCos_PC), _mm_mul_ps(xCord, rotSin_PC)), imagCord) : imagJulia;\
						__m128 zr = (param.juliaSet) ? _mm_add_ps(_mm_sub_ps(_mm_mul_ps(xCord, rotCos_PC), _mm_mul_ps(yCord, rotSin_PC)), realCord) : realJulia;\
						__m128 zi = (param.juliaSet) ? _mm_add_ps(_mm_add_ps(_mm_mul_ps(yCord, rotCos_PC), _mm_mul_ps(xCord, rotSin_PC)), imagCord) : imagJulia;\
						\
						__m128 low = _mm_set_ps1(4.0);\
						__m128 zs = _mm_setzero_ps();\
						__m128 __attribute__((unused)) temp_zr = _mm_setzero_ps();\
						__m128 current_value_mask = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps());\
						for (int i = (int)SIMD_Spacing - 1; i > (int)valuesToWrite - 1; i--) {\
							((fp32*)((void*)(&current_value_mask)))[i] = 0.0f;\
						}\
						for (uint32_t itr = 0; itr < param.maxItr; itr++) {

		#define Block_EndLoop_SSE2_FP32(); \
							/* Tracks the lowest value */\
							low = _mm_min_ps(zs, low);\
							\
							/* Excludes any Z^2 values that have already exceeded the breakout value by setting them to 0.0 */\
							zs = _mm_and_ps(zs, current_value_mask);\
							/* Gets a mask with all the Z^2 values that exceeded the breakout value */\
							int break_mask = _mm_movemask_ps(_mm_cmpgt_ps(zs, breakoutValue));\
							\
							if (break_mask != 0) {\
								/* Z^2 values that exceeded the breakout value will use `Exterior_Coloring` */\
								for (int i = 0; i < (int)SIMD_Spacing; i++) {\
									if (break_mask & (1 << i)) {\
										CPU_Exterior_Coloring_SSE2_FP32(\
											((fp32*)((void*)(&outR)))[i], ((fp32*)((void*)(&outG)))[i], ((fp32*)((void*)(&outB)))[i], ((fp32*)((void*)(&outA)))[i],\
											param,\
											itr, ((fp32*)((void*)(&zs)))[i], param.inverse_log2_power\
										);\
										((fp32*)((void*)(&current_value_mask)))[i] = 0.0; /* Removes the Z^2 value from the list of Z^2 values that have Not exceeded the breakout value */\
									}\
								}\
								if (_mm_movemask_ps(_mm_cmpeq_ps(current_value_mask, _mm_setzero_ps())) == 0) {\
									break; /* If all the Z^2 values have exceeded the breakout value, break the loop */\
								}\
							}\
						}\
						/* Checks if any values did Not exceed the breakout value */\
						int inside_value_mask = _mm_movemask_ps(_mm_cmpneq_ps(current_value_mask, _mm_setzero_ps()));\
						if (inside_value_mask != 0) {\
							/* Z^2 values that did Not exceed the breakout value will use `Interior_Coloring` */\
							for (int i = 0; i < (int)SIMD_Spacing; i++) {\
								if (inside_value_mask & (1 << i)) {\
									CPU_Interior_Coloring_SSE2_FP32(\
										((fp32*)((void*)(&outR)))[i], ((fp32*)((void*)(&outG)))[i], ((fp32*)((void*)(&outB)))[i], ((fp32*)((void*)(&outA)))[i],\
										param,\
										((fp32*)((void*)(&low)))[i]\
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
				/* Divide R, G, and B by Alpha */\
					outR = _mm_div_ps(outR, outA);\
					outG = _mm_div_ps(outG, outA);\
					outB = _mm_div_ps(outB, outA);\
				/* If division by zero occured, set values to 0 */\
					const __m128 div_zero_mask = _mm_cmpeq_ps(outA, _mm_setzero_ps());\
					outR = _mm_andnot_ps(div_zero_mask, outR);\
					outG = _mm_andnot_ps(div_zero_mask, outG);\
					outB = _mm_andnot_ps(div_zero_mask, outB);\
				\
				/* Normilizes alpha to 0.0 - 1.0 */\
					outA = _mm_div_ps(outA, alphaDiv);\
				/* Normilizes alpha to 0.0 - 1.0 */\
					const __m128 color_mult = _mm_set_ps1(255.0);\
					outR = _mm_mul_ps(outR, color_mult);\
					outG = _mm_mul_ps(outG, color_mult);\
					outB = _mm_mul_ps(outB, color_mult);\
					outA = _mm_mul_ps(outA, color_mult);\
				\
				for (size_t i = 0; i < valuesToWrite; i++) {\
					/* Writes in the equivilant of SDL_PIXELFORMAT_ABGR8888 */\
					data[dataPtr] = (uint8_t)((fp32*)((void*)(&outR)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp32*)((void*)(&outG)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp32*)((void*)(&outB)))[i]; dataPtr++;\
					data[dataPtr] = (uint8_t)((fp32*)((void*)(&outA)))[i]; dataPtr++;\
				}\
				/* Increases the image offset/index */\
				p0 += valuesToWrite;\
			}\
			x = 0;\
		}

/* Fractal Rendering SSE2 FP64 */

	void quadraticRender_SSE2_FP64(FractalParameters(fp64, fp64)) {

		Block_Init_SSE2_FP64();

			__m128d zr1, zr2, zi1, zi2;

			bool f[8]; // 8 bit formula
			for (uint8_t q = 0; q < 8; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			
			// Bits 0-2 will flip signage
			const __m128d s1 = (f[0]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			const __m128d s2 = (f[1]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			const __m128d s3 = (f[2]) ? _mm_set_pd1(-2.0) : _mm_set_pd1(2.0);
			
			// Bits 3-7 will apply fabs() via a mask
			const __m128d zr1_mask = (f[3]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi1_mask = (f[4]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr2_mask = (f[5]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi2_mask = (f[6]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr_mask  = (f[7]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);

		Block_BeginLoop_SSE2_FP64();
		
			// Applies fabs() to zr and zi
			zr1 = _mm_andnot_pd(zr1_mask, zr);
			zr2 = _mm_andnot_pd(zr2_mask, zr);
			zi1 = _mm_andnot_pd(zi1_mask, zi);
			zi2 = _mm_andnot_pd(zi2_mask, zi);
			
			// Calculates the new zr and zi
			zr = _mm_add_pd(_mm_andnot_pd(zr_mask, _mm_mul_pd(s1,
				_mm_sub_pd(
					_mm_mul_pd(zr1, zr), 
					_mm_mul_pd(s2,
						_mm_mul_pd(zi1, zi)
					)
				)
			)), cr);
			zi = _mm_add_pd(_mm_mul_pd(_mm_mul_pd(zr2, zi2), s3), ci);

			zs = _mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));
		
		Block_EndLoop_SSE2_FP64();
	}

	void cubicRender_SSE2_FP64(FractalParameters(fp64, fp64)) {
		
		Block_Init_SSE2_FP64();

			__m128d zr1, zr2, zr3, zi1, zi2, zi3;
			bool f[14];
			for (uint8_t q = 0; q < 14; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m128d s1 = (f[0]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			const __m128d s2 = (f[1]) ? _mm_set_pd1(-3.0) : _mm_set_pd1(3.0);
			const __m128d s3 = (f[2]) ? _mm_set_pd1(-3.0) : _mm_set_pd1(3.0);
			const __m128d s4 = (f[3]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			const __m128d s5 = (f[4]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			const __m128d s6 = (f[5]) ? _mm_set_pd1(-1.0) : _mm_set_pd1(1.0);
			
			const __m128d zr1_mask = (f[ 6]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi1_mask = (f[ 7]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr2_mask = (f[ 8]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi2_mask = (f[ 9]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr3_mask = (f[10]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi3_mask = (f[11]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr_mask  = (f[12]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi_mask  = (f[13]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);

		Block_BeginLoop_SSE2_FP64();

			zr1 = _mm_andnot_pd(zr1_mask, zr);
			zi1 = _mm_andnot_pd(zi1_mask, zi);
			zr2 = _mm_andnot_pd(zr2_mask, zr);
			zi2 = _mm_andnot_pd(zi2_mask, zi);
			zr3 = _mm_andnot_pd(zr3_mask, zr);
			zi3 = _mm_andnot_pd(zi3_mask, zi);

			temp_zr = _mm_add_pd(_mm_mul_pd(s5, _mm_andnot_pd(zr_mask,
				_mm_sub_pd(
					_mm_mul_pd(_mm_mul_pd(s1, zr1), _mm_mul_pd(zr , zr)),
					_mm_mul_pd(_mm_mul_pd(s2, zr2), _mm_mul_pd(zi1, zi))
				)
			)), cr);
			zi      = _mm_add_pd(_mm_mul_pd(s6, _mm_andnot_pd(zi_mask,
				_mm_sub_pd(
					_mm_mul_pd(_mm_mul_pd(s3, zr3), _mm_mul_pd(zr, zi2)),
					_mm_mul_pd(_mm_mul_pd(s4, zi3), _mm_mul_pd(zi, zi ))
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));
		
		Block_EndLoop_SSE2_FP64();
	}

	void quarticRender_SSE2_FP64(FractalParameters(fp64, fp64)) {

		Block_Init_SSE2_FP64();

			__m128d zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4;
			bool f[17];
			for (uint8_t q = 0; q < 17; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m128d s1 = (f[0]) ? _mm_set1_pd(-1.0) : _mm_set1_pd(1.0);
			const __m128d s2 = (f[1]) ? _mm_set1_pd(-6.0) : _mm_set1_pd(6.0);
			const __m128d s3 = (f[2]) ? _mm_set1_pd(-1.0) : _mm_set1_pd(1.0);
			const __m128d s4 = (f[3]) ? _mm_set1_pd(-4.0) : _mm_set1_pd(4.0);
			const __m128d s5 = (f[4]) ? _mm_set1_pd(-4.0) : _mm_set1_pd(4.0);
			const __m128d s6 = (f[5]) ? _mm_set1_pd(-1.0) : _mm_set1_pd(1.0);
			const __m128d s7 = (f[6]) ? _mm_set1_pd(-1.0) : _mm_set1_pd(1.0);
			
			const __m128d zr1_mask = (f[ 7]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi1_mask = (f[ 8]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr2_mask = (f[ 9]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi2_mask = (f[10]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr3_mask = (f[11]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi3_mask = (f[12]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr4_mask = (f[13]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi4_mask = (f[14]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr_mask  = (f[15]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi_mask  = (f[16]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);

		Block_BeginLoop_SSE2_FP64();

			zr1 = _mm_andnot_pd(zr1_mask, zr);
			zi1 = _mm_andnot_pd(zi1_mask, zi);
			zr2 = _mm_andnot_pd(zr2_mask, zr);
			zi2 = _mm_andnot_pd(zi2_mask, zi);
			zr3 = _mm_andnot_pd(zr3_mask, zr);
			zi3 = _mm_andnot_pd(zi3_mask, zi);
			zr4 = _mm_andnot_pd(zr4_mask, zr);
			zi4 = _mm_andnot_pd(zi4_mask, zi);

			temp_zr = _mm_add_pd(_mm_mul_pd(s6, _mm_andnot_pd(zr_mask,
				_mm_add_pd(
					_mm_sub_pd(
						_mm_mul_pd(s1,
							_mm_mul_pd(_mm_mul_pd(zr1, zr ), _mm_mul_pd(zr, zr))
						),
						_mm_mul_pd(s2,
							_mm_mul_pd(_mm_mul_pd(zr2, zi1), _mm_mul_pd(zr, zi))
						)
					),
					_mm_mul_pd(s3,
						_mm_mul_pd(_mm_mul_pd(zi2, zi), _mm_mul_pd(zi, zi))
					)
				)
			)), cr);
			zi      = _mm_add_pd(_mm_mul_pd(s7, _mm_andnot_pd(zi_mask,
				_mm_sub_pd(
					_mm_mul_pd(s4,
						_mm_mul_pd(_mm_mul_pd(zr3, zi3), _mm_mul_pd(zr, zr))
					),
					_mm_mul_pd(s5,
						_mm_mul_pd(_mm_mul_pd(zr4, zi4), _mm_mul_pd(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));

		Block_EndLoop_SSE2_FP64();
	}

	void quinticRender_SSE2_FP64(FractalParameters(fp64, fp64)) {

		Block_Init_SSE2_FP64();

			__m128d zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5;
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
			const __m128d s1 = (fS[0]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s2 = (fS[1]) ? _mm_set_pd1(-10.0) : _mm_set_pd1(10.0);
			const __m128d s3 = (fS[2]) ? _mm_set_pd1(-5.0 ) : _mm_set_pd1(5.0 );
			const __m128d s4 = (fS[3]) ? _mm_set_pd1(-5.0 ) : _mm_set_pd1(5.0 );
			const __m128d s5 = (fS[4]) ? _mm_set_pd1(-10.0) : _mm_set_pd1(10.0);
			const __m128d s6 = (fS[5]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s7 = (fO[0]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s8 = (fO[1]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );

			const __m128d zr1_mask = (fA[0]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi1_mask = (fA[1]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr2_mask = (fA[2]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi2_mask = (fA[3]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr3_mask = (fA[4]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi3_mask = (fA[5]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr4_mask = (fA[6]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi4_mask = (fA[7]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr5_mask = (fA[8]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi5_mask = (fA[9]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr_mask  = (fO[2]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi_mask  = (fO[3]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);

		Block_BeginLoop_SSE2_FP64();

			zr1 = _mm_andnot_pd(zr1_mask, zr);
			zi1 = _mm_andnot_pd(zi1_mask, zi);
			zr2 = _mm_andnot_pd(zr2_mask, zr);
			zi2 = _mm_andnot_pd(zi2_mask, zi);
			zr3 = _mm_andnot_pd(zr3_mask, zr);
			zi3 = _mm_andnot_pd(zi3_mask, zi);
			zr4 = _mm_andnot_pd(zr4_mask, zr);
			zi4 = _mm_andnot_pd(zi4_mask, zi);
			zr5 = _mm_andnot_pd(zr5_mask, zr);
			zi5 = _mm_andnot_pd(zi5_mask, zi);

			temp_zr = _mm_add_pd(_mm_mul_pd(s7, _mm_andnot_pd(zr_mask ,
				_mm_add_pd(
					_mm_sub_pd(
						_mm_mul_pd(
							_mm_mul_pd(s1, zr1),
							_mm_mul_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zr , zr))
						),
						_mm_mul_pd(
							_mm_mul_pd(s2, _mm_mul_pd(zr2, zi1)),
							_mm_mul_pd(zi, _mm_mul_pd(zr , zr ))
						)
					),
					_mm_mul_pd(
						_mm_mul_pd(s3, _mm_mul_pd(zr3, zi2)),
						_mm_mul_pd(zi, _mm_mul_pd(zi , zi ))
					)
				)
			)), cr);
			zi      = _mm_add_pd(_mm_mul_pd(s8, _mm_andnot_pd(zi_mask,
				_mm_add_pd(
					_mm_sub_pd(
						_mm_mul_pd(
							_mm_mul_pd(s4 , _mm_mul_pd(zr4, zr)),
							_mm_mul_pd(zi3, _mm_mul_pd(zr , zr))
						),
						_mm_mul_pd(
							_mm_mul_pd(s5 , _mm_mul_pd(zr5, zr)),
							_mm_mul_pd(zi4, _mm_mul_pd(zi , zi))
						)
					),
					_mm_mul_pd(
						_mm_mul_pd(s6 , zi5),
						_mm_mul_pd(_mm_mul_pd(zi, zi), _mm_mul_pd(zi , zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));
		
		Block_EndLoop_SSE2_FP64();
	}

	void sexticRender_SSE2_FP64(FractalParameters(fp64, fp64)) {

		Block_Init_SSE2_FP64();

			__m128d zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6;
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

			const __m128d s1 = (fS[0]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s2 = (fS[1]) ? _mm_set_pd1(-15.0) : _mm_set_pd1(15.0);
			const __m128d s3 = (fS[2]) ? _mm_set_pd1(-15.0) : _mm_set_pd1(15.0);
			const __m128d s4 = (fS[3]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s5 = (fS[4]) ? _mm_set_pd1(-6.0 ) : _mm_set_pd1(6.0 );
			const __m128d s6 = (fS[5]) ? _mm_set_pd1(-20.0) : _mm_set_pd1(20.0);
			const __m128d s7 = (fS[6]) ? _mm_set_pd1(-6.0 ) : _mm_set_pd1(6.0 );
			const __m128d s8 = (fO[0]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
			const __m128d s9 = (fO[1]) ? _mm_set_pd1(-1.0 ) : _mm_set_pd1(1.0 );
		
			const __m128d zr1_mask = (fA[ 0]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi1_mask = (fA[ 1]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr2_mask = (fA[ 2]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi2_mask = (fA[ 3]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr3_mask = (fA[ 4]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi3_mask = (fA[ 5]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr4_mask = (fA[ 6]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi4_mask = (fA[ 7]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr5_mask = (fA[ 8]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi5_mask = (fA[ 9]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr6_mask = (fA[10]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi6_mask = (fA[11]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zr_mask  = (fO[ 2]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
			const __m128d zi_mask  = (fO[ 3]) ? _mm_set_pd1(-0.0) : _mm_set_pd1(0.0);
		
		Block_BeginLoop_SSE2_FP64();

			zr1 = _mm_andnot_pd(zr1_mask, zr);
			zi1 = _mm_andnot_pd(zi1_mask, zi);
			zr2 = _mm_andnot_pd(zr2_mask, zr);
			zi2 = _mm_andnot_pd(zi2_mask, zi);
			zr3 = _mm_andnot_pd(zr3_mask, zr);
			zi3 = _mm_andnot_pd(zi3_mask, zi);
			zr4 = _mm_andnot_pd(zr4_mask, zr);
			zi4 = _mm_andnot_pd(zi4_mask, zi);
			zr5 = _mm_andnot_pd(zr5_mask, zr);
			zi5 = _mm_andnot_pd(zi5_mask, zi);
			zr6 = _mm_andnot_pd(zr6_mask, zr);
			zi6 = _mm_andnot_pd(zi6_mask, zi);

			temp_zr = _mm_add_pd(_mm_mul_pd(s8, _mm_andnot_pd(zr_mask ,
				_mm_sub_pd(
					_mm_add_pd(
						_mm_sub_pd(
							_mm_mul_pd(
								_mm_mul_pd(s1, _mm_mul_pd(zr1, zr)),
								_mm_mul_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zr, zr))
							),
							_mm_mul_pd(
								_mm_mul_pd(s2, _mm_mul_pd(zr2, zi1)),
								_mm_mul_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zr, zi))
							)
						),
						_mm_mul_pd(
							_mm_mul_pd(s3, _mm_mul_pd(zr3, zi2)),
							_mm_mul_pd(_mm_mul_pd(zr, zi), _mm_mul_pd(zi, zi))
						)
					),
					_mm_mul_pd(
						_mm_mul_pd(s4, _mm_mul_pd(zi3, zi)),
						_mm_mul_pd(_mm_mul_pd(zi, zi), _mm_mul_pd(zi, zi))
					)
				)
			)), cr);
			zi      = _mm_add_pd(_mm_mul_pd(s9, _mm_andnot_pd(zi_mask,
				_mm_add_pd(
					_mm_sub_pd(
						_mm_mul_pd(
							_mm_mul_pd(s5, _mm_mul_pd(zr4, zi4)),
							_mm_mul_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zr, zr))
						),
						_mm_mul_pd(
							_mm_mul_pd(s6, _mm_mul_pd(zr5, zi5)),
							_mm_mul_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi))
						)
					),
					_mm_mul_pd(
						_mm_mul_pd(s7, _mm_mul_pd(zr6, zi6)),
						_mm_mul_pd(_mm_mul_pd(zi, zi), _mm_mul_pd(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));

		Block_EndLoop_SSE2_FP64();
	}

/* Fractal Rendering SSE2 FP32 */

	void quadraticRender_SSE2_FP32(FractalParameters(fp32, fp32)) {

		__m128 zr1, zr2, zi1, zi2;

		bool f[8]; // 8 bit formula
		for (uint8_t q = 0; q < 8; q++) {
			f[q] = ((param.formula >> q) & 1) ? true : false;
		}
		
		// Bits 0-2 will flip signage
		const __m128 s1 = (f[0]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
		const __m128 s2 = (f[1]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
		const __m128 s3 = (f[2]) ? _mm_set_ps1(-2.0f) : _mm_set_ps1(2.0f);
		
		// Bits 3-7 will apply fabs() via a mask
		const __m128 zr1_mask = (f[3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
		const __m128 zi1_mask = (f[4]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
		const __m128 zr2_mask = (f[5]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
		const __m128 zi2_mask = (f[6]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
		const __m128 zr_mask = (f[7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);

		

		Block_BeginLoop_SSE2_FP32();

			// Applies fabs() to zr and zi
			zr1 = _mm_andnot_ps(zr1_mask, zr);
			zr2 = _mm_andnot_ps(zr2_mask, zr);
			zi1 = _mm_andnot_ps(zi1_mask, zi);
			zi2 = _mm_andnot_ps(zi2_mask, zi);
			
			// Calculates the new zr and zi
			zr = _mm_add_ps(_mm_andnot_ps(zr_mask, _mm_mul_ps(s1,
				_mm_sub_ps(
					_mm_mul_ps(zr1, zr),
					_mm_mul_ps(s2,
						_mm_mul_ps(zi1, zi)
					)
				)
			)), cr);
			zi = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(zr2, zi2), s3), ci);

			zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi));
		
		Block_EndLoop_SSE2_FP32();
	}

	void cubicRender_SSE2_FP32(FractalParameters(fp32, fp32)) {
		
		Block_Init_SSE2_FP32();

			__m128 zr1, zr2, zr3, zi1, zi2, zi3;
			bool f[14];
			for (uint8_t q = 0; q < 14; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m128 s1 = (f[0]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
			const __m128 s2 = (f[1]) ? _mm_set_ps1(-3.0f) : _mm_set_ps1(3.0f);
			const __m128 s3 = (f[2]) ? _mm_set_ps1(-3.0f) : _mm_set_ps1(3.0f);
			const __m128 s4 = (f[3]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
			const __m128 s5 = (f[4]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
			const __m128 s6 = (f[5]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
			
			const __m128 zr1_mask = (f[ 6]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi1_mask = (f[ 7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr2_mask = (f[ 8]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi2_mask = (f[ 9]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr3_mask = (f[10]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi3_mask = (f[11]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr_mask  = (f[12]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi_mask  = (f[13]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);

		Block_BeginLoop_SSE2_FP32();

			zr1 = _mm_andnot_ps(zr1_mask, zr);
			zi1 = _mm_andnot_ps(zi1_mask, zi);
			zr2 = _mm_andnot_ps(zr2_mask, zr);
			zi2 = _mm_andnot_ps(zi2_mask, zi);
			zr3 = _mm_andnot_ps(zr3_mask, zr);
			zi3 = _mm_andnot_ps(zi3_mask, zi);

			temp_zr = _mm_add_ps(_mm_mul_ps(s5, _mm_andnot_ps(zr_mask,
				_mm_sub_ps(
					_mm_mul_ps(_mm_mul_ps(s1, zr1), _mm_mul_ps(zr , zr)),
					_mm_mul_ps(_mm_mul_ps(s2, zr2), _mm_mul_ps(zi1, zi))
				)
			)), cr);
			zi      = _mm_add_ps(_mm_mul_ps(s6, _mm_andnot_ps(zi_mask,
				_mm_sub_ps(
					_mm_mul_ps(_mm_mul_ps(s3, zr3), _mm_mul_ps(zr, zi2)),
					_mm_mul_ps(_mm_mul_ps(s4, zi3), _mm_mul_ps(zi, zi ))
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi));
		
		Block_EndLoop_SSE2_FP32();
	}

	void quarticRender_SSE2_FP32(FractalParameters(fp32, fp32)) {

		Block_Init_SSE2_FP32();

			__m128 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4;
			bool f[17];
			for (uint8_t q = 0; q < 17; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			const __m128 s1 = (f[0]) ? _mm_set1_ps(-1.0f) : _mm_set1_ps(1.0f);
			const __m128 s2 = (f[1]) ? _mm_set1_ps(-6.0f) : _mm_set1_ps(6.0f);
			const __m128 s3 = (f[2]) ? _mm_set1_ps(-1.0f) : _mm_set1_ps(1.0f);
			const __m128 s4 = (f[3]) ? _mm_set1_ps(-4.0f) : _mm_set1_ps(4.0f);
			const __m128 s5 = (f[4]) ? _mm_set1_ps(-4.0f) : _mm_set1_ps(4.0f);
			const __m128 s6 = (f[5]) ? _mm_set1_ps(-1.0f) : _mm_set1_ps(1.0f);
			const __m128 s7 = (f[6]) ? _mm_set1_ps(-1.0f) : _mm_set1_ps(1.0f);
			
			const __m128 zr1_mask = (f[ 7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi1_mask = (f[ 8]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr2_mask = (f[ 9]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi2_mask = (f[10]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr3_mask = (f[11]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi3_mask = (f[12]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr4_mask = (f[13]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi4_mask = (f[14]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr_mask  = (f[15]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi_mask  = (f[16]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);

		Block_BeginLoop_SSE2_FP32();

			zr1 = _mm_andnot_ps(zr1_mask, zr);
			zi1 = _mm_andnot_ps(zi1_mask, zi);
			zr2 = _mm_andnot_ps(zr2_mask, zr);
			zi2 = _mm_andnot_ps(zi2_mask, zi);
			zr3 = _mm_andnot_ps(zr3_mask, zr);
			zi3 = _mm_andnot_ps(zi3_mask, zi);
			zr4 = _mm_andnot_ps(zr4_mask, zr);
			zi4 = _mm_andnot_ps(zi4_mask, zi);

			temp_zr = _mm_add_ps(_mm_mul_ps(s6, _mm_andnot_ps(zr_mask ,
				_mm_add_ps(
					_mm_sub_ps(
						_mm_mul_ps(s1,
							_mm_mul_ps(_mm_mul_ps(zr1, zr), _mm_mul_ps(zr, zr))
						),
						_mm_mul_ps(s2,
							_mm_mul_ps(_mm_mul_ps(zr2, zi1), _mm_mul_ps(zr, zi))
						)
					),
					_mm_mul_ps(s3,
						_mm_mul_ps(_mm_mul_ps(zi2, zi), _mm_mul_ps(zi, zi))
					)
				)
			)), cr);
			zi      = _mm_add_ps(_mm_mul_ps(s7, _mm_andnot_ps(zi_mask,
				_mm_sub_ps(
					_mm_mul_ps(s4,
						_mm_mul_ps(_mm_mul_ps(zr3, zi3), _mm_mul_ps(zr, zr))
					),
					_mm_mul_ps(s5,
						_mm_mul_ps(_mm_mul_ps(zr4, zi4), _mm_mul_ps(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi));

		Block_EndLoop_SSE2_FP32();
	}

	void quinticRender_SSE2_FP32(FractalParameters(fp32, fp32)) {

		Block_Init_SSE2_FP32();

			__m128 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5;
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
			const __m128 s1 = (fS[0]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s2 = (fS[1]) ? _mm_set_ps1(-10.0f) : _mm_set_ps1(10.0f);
			const __m128 s3 = (fS[2]) ? _mm_set_ps1(-5.0f ) : _mm_set_ps1(5.0f );
			const __m128 s4 = (fS[3]) ? _mm_set_ps1(-5.0f ) : _mm_set_ps1(5.0f );
			const __m128 s5 = (fS[4]) ? _mm_set_ps1(-10.0f) : _mm_set_ps1(10.0f);
			const __m128 s6 = (fS[5]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s7 = (fO[0]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s8 = (fO[1]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );

			const __m128 zr1_mask = (fA[0]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi1_mask = (fA[1]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr2_mask = (fA[2]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi2_mask = (fA[3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr3_mask = (fA[4]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi3_mask = (fA[5]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr4_mask = (fA[6]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi4_mask = (fA[7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr5_mask = (fA[8]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi5_mask = (fA[9]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr_mask  = (fO[2]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi_mask  = (fO[3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);

		Block_BeginLoop_SSE2_FP32();

			zr1 = _mm_andnot_ps(zr1_mask, zr);
			zi1 = _mm_andnot_ps(zi1_mask, zi);
			zr2 = _mm_andnot_ps(zr2_mask, zr);
			zi2 = _mm_andnot_ps(zi2_mask, zi);
			zr3 = _mm_andnot_ps(zr3_mask, zr);
			zi3 = _mm_andnot_ps(zi3_mask, zi);
			zr4 = _mm_andnot_ps(zr4_mask, zr);
			zi4 = _mm_andnot_ps(zi4_mask, zi);
			zr5 = _mm_andnot_ps(zr5_mask, zr);
			zi5 = _mm_andnot_ps(zi5_mask, zi);

			temp_zr = _mm_add_ps(_mm_mul_ps(s7, _mm_andnot_ps(zr_mask ,
				_mm_add_ps(
					_mm_sub_ps(
						_mm_mul_ps(
							_mm_mul_ps(s1, zr1),
							_mm_mul_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zr , zr))
						),
						_mm_mul_ps(
							_mm_mul_ps(s2, _mm_mul_ps(zr2, zi1)),
							_mm_mul_ps(zi, _mm_mul_ps(zr , zr ))
						)
					),
					_mm_mul_ps(
						_mm_mul_ps(s3, _mm_mul_ps(zr3, zi2)),
						_mm_mul_ps(zi, _mm_mul_ps(zi , zi ))
					)
				)
			)), cr);
			zi      = _mm_add_ps(_mm_mul_ps(s8, _mm_andnot_ps(zi_mask,
				_mm_add_ps(
					_mm_sub_ps(
						_mm_mul_ps(
							_mm_mul_ps(s4 , _mm_mul_ps(zr4, zr)),
							_mm_mul_ps(zi3, _mm_mul_ps(zr , zr))
						),
						_mm_mul_ps(
							_mm_mul_ps(s5 , _mm_mul_ps(zr5, zr)),
							_mm_mul_ps(zi4, _mm_mul_ps(zi , zi))
						)
					),
					_mm_mul_ps(
						_mm_mul_ps(s6 , zi5),
						_mm_mul_ps(_mm_mul_ps(zi, zi), _mm_mul_ps(zi , zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi));
		
		Block_EndLoop_SSE2_FP32();
	}

	void sexticRender_SSE2_FP32(FractalParameters(fp32, fp32)) {

		Block_Init_SSE2_FP32();

			__m128 zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6;
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

			const __m128 s1 = (fS[0]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s2 = (fS[1]) ? _mm_set_ps1(-15.0f) : _mm_set_ps1(15.0f);
			const __m128 s3 = (fS[2]) ? _mm_set_ps1(-15.0f) : _mm_set_ps1(15.0f);
			const __m128 s4 = (fS[3]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s5 = (fS[4]) ? _mm_set_ps1(-6.0f ) : _mm_set_ps1(6.0f );
			const __m128 s6 = (fS[5]) ? _mm_set_ps1(-20.0f) : _mm_set_ps1(20.0f);
			const __m128 s7 = (fS[6]) ? _mm_set_ps1(-6.0f ) : _mm_set_ps1(6.0f );
			const __m128 s8 = (fO[0]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
			const __m128 s9 = (fO[1]) ? _mm_set_ps1(-1.0f ) : _mm_set_ps1(1.0f );
		
			const __m128 zr1_mask = (fA[ 0]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi1_mask = (fA[ 1]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr2_mask = (fA[ 2]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi2_mask = (fA[ 3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr3_mask = (fA[ 4]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi3_mask = (fA[ 5]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr4_mask = (fA[ 6]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi4_mask = (fA[ 7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr5_mask = (fA[ 8]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi5_mask = (fA[ 9]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr6_mask = (fA[10]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi6_mask = (fA[11]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zr_mask  = (fO[ 2]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
			const __m128 zi_mask  = (fO[ 3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
		
		Block_BeginLoop_SSE2_FP32();

			zr1 = _mm_andnot_ps(zr1_mask, zr);
			zi1 = _mm_andnot_ps(zi1_mask, zi);
			zr2 = _mm_andnot_ps(zr2_mask, zr);
			zi2 = _mm_andnot_ps(zi2_mask, zi);
			zr3 = _mm_andnot_ps(zr3_mask, zr);
			zi3 = _mm_andnot_ps(zi3_mask, zi);
			zr4 = _mm_andnot_ps(zr4_mask, zr);
			zi4 = _mm_andnot_ps(zi4_mask, zi);
			zr5 = _mm_andnot_ps(zr5_mask, zr);
			zi5 = _mm_andnot_ps(zi5_mask, zi);
			zr6 = _mm_andnot_ps(zr6_mask, zr);
			zi6 = _mm_andnot_ps(zi6_mask, zi);

			temp_zr = _mm_add_ps(_mm_mul_ps(s8, _mm_andnot_ps(zr_mask ,
				_mm_sub_ps(
					_mm_add_ps(
						_mm_sub_ps(
							_mm_mul_ps(
								_mm_mul_ps(s1, _mm_mul_ps(zr1, zr)),
								_mm_mul_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zr, zr))
							),
							_mm_mul_ps(
								_mm_mul_ps(s2, _mm_mul_ps(zr2, zi1)),
								_mm_mul_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zr, zi))
							)
						),
						_mm_mul_ps(
							_mm_mul_ps(s3, _mm_mul_ps(zr3, zi2)),
							_mm_mul_ps(_mm_mul_ps(zr, zi), _mm_mul_ps(zi, zi))
						)
					),
					_mm_mul_ps(
						_mm_mul_ps(s4, _mm_mul_ps(zi3, zi)),
						_mm_mul_ps(_mm_mul_ps(zi, zi), _mm_mul_ps(zi, zi))
					)
				)
			)), cr);
			zi      = _mm_add_ps(_mm_mul_ps(s9, _mm_andnot_ps(zi_mask,
				_mm_add_ps(
					_mm_sub_ps(
						_mm_mul_ps(
							_mm_mul_ps(s5, _mm_mul_ps(zr4, zi4)),
							_mm_mul_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zr, zr))
						),
						_mm_mul_ps(
							_mm_mul_ps(s6, _mm_mul_ps(zr5, zi5)),
							_mm_mul_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi))
						)
					),
					_mm_mul_ps(
						_mm_mul_ps(s7, _mm_mul_ps(zr6, zi6)),
						_mm_mul_ps(_mm_mul_ps(zi, zi), _mm_mul_ps(zi, zi))
					)
				)
			)), ci);
			zr = temp_zr;

			zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi));

		Block_EndLoop_SSE2_FP32();
	}

#endif /* ENABLE_SSE2_RENDERING */