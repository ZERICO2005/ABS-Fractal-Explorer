/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "frac_Multi_Internal.h"
#include "frac_Multi_SSE2.h"

#include "fractal.h"
#include "render.h"

#include <xmmintrin.h>

void CPU_Interior_Coloring_SSE2_FP32(
	fp32& outR, fp32& outG, fp32& outB, fp32& outA,
	const PreCalc_Param<fp32,fp32>& param,
	fp32 low
) {
	outR += param.interior_R_Amp * param.interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.interior_R_Freq + param.interior_R_Phase));
	outG += param.interior_G_Amp * param.interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.interior_G_Freq + param.interior_G_Phase));
	outB += param.interior_B_Amp * param.interior_Alpha * (0.5f - 0.5f * cosf(logf(low) * param.interior_B_Freq + param.interior_B_Phase));
	outA += param.interior_Alpha;
}
			
void CPU_Exterior_Coloring_SSE2_FP32(
	fp32& outR, fp32& outG, fp32& outB, fp32& outA,
	const PreCalc_Param<fp32,fp32>& param,
	uint32_t itr, fp32 zs, fp32 inverse_log2_power
) {
	fp32 smooth = log1pf(fmaxf(0.0f, (fp32)itr - log2f(log2f(zs) / 2.0f) * inverse_log2_power));
	outR += param.exterior_R_Amp * param.exterior_Alpha * (0.5f - 0.5f * cosf((fp32)TAU * (param.exterior_R_Freq * smooth + param.exterior_R_Phase)));
	outG += param.exterior_G_Amp * param.exterior_Alpha * (0.5f - 0.5f * cosf((fp32)TAU * (param.exterior_G_Freq * smooth + param.exterior_G_Phase)));
	outB += param.exterior_B_Amp * param.exterior_Alpha * (0.5f - 0.5f * cosf((fp32)TAU * (param.exterior_B_Freq * smooth + param.exterior_B_Phase)));
	outA += param.exterior_Alpha;
}

void quadraticRender_SSE2_FP64(FractalParameters(fp64, fp64));

#define printf_m128_fp32(x) \
	printFlush("\n" STR_M(x) ": %f %f %f %f", ((fp32*)((void*)(&x)))[0], ((fp32*)((void*)(&x)))[1], ((fp32*)((void*)(&x)))[2], ((fp32*)((void*)(&x)))[3]);

#define printfInterval_m128_fp32(freq, x) \
	printfInterval(freq, "\n" STR_M(x) ": %f %f %f %f", ((fp32*)((void*)(&x)))[0], ((fp32*)((void*)(&x)))[1], ((fp32*)((void*)(&x)))[2], ((fp32*)((void*)(&x)))[3]);

#define printf_m128_u32(x) \
	printFlush("\n" STR_M(x) ": %08X %08X %08X %08X", ((uint32_t*)((void*)(&x)))[0], ((uint32_t*)((void*)(&x)))[1], ((uint32_t*)((void*)(&x)))[2], ((uint32_t*)((void*)(&x)))[3]);

#define printfInterval_m128_u32(freq, x) \
	printfInterval(freq, "\n" STR_M(x) ": %08X %08X %08X %08X", ((uint32_t*)((void*)(&x)))[0], ((uint32_t*)((void*)(&x)))[1], ((uint32_t*)((void*)(&x)))[2], ((uint32_t*)((void*)(&x)))[3]);


void quadraticRender_SSE2_FP32(FractalParameters(fp32, fp32)) {

	const fp32 inverse_log2_power = inverse_log2(2.0);
	
	const __m128 realCord         = _mm_set_ps1(param.realCord        );
	const __m128 imagCord         = _mm_set_ps1(param.imagCord        );
	const __m128 realJulia        = _mm_set_ps1(param.realJulia       );
	const __m128 imagJulia        = _mm_set_ps1(param.imagJulia       );
	const __m128 zoom_PC          = _mm_set_ps1(param.zoom_PC         );
	const __m128 rotSin_PC        = _mm_set_ps1(param.rotSin_PC       );
	const __m128 rotCos_PC        = _mm_set_ps1(param.rotCos_PC       );
	const __m128 breakoutValue    = _mm_set_ps1(param.breakoutValue   );
	const __m128 numY             = _mm_set_ps1(param.numY            );
	const __m128 numX             = _mm_set_ps1(param.numX            );
	const __m128 recip_numZ       = _mm_set_ps1(param.recip_numZ      );
	const __m128 neg_recip_numW   = _mm_set_ps1(param.neg_recip_numW  );
	const __m128 polarPower       = _mm_set_ps1(param.polarPower      );
	const __m128 polarPowerHalf   = _mm_set_ps1(param.polarPowerHalf  );
	const __m128 sampleDiv        = _mm_set_ps1(param.sampleDiv       );
	const __m128 exterior_Alpha   = _mm_set_ps1(param.exterior_Alpha  );
	const __m128 exterior_R_Amp   = _mm_set_ps1(param.exterior_R_Amp  );
	const __m128 exterior_R_Freq  = _mm_set_ps1(param.exterior_R_Freq );
	const __m128 exterior_R_Phase = _mm_set_ps1(param.exterior_R_Phase);
	const __m128 exterior_G_Amp   = _mm_set_ps1(param.exterior_G_Amp  );
	const __m128 exterior_G_Freq  = _mm_set_ps1(param.exterior_G_Freq );
	const __m128 exterior_G_Phase = _mm_set_ps1(param.exterior_G_Phase);
	const __m128 exterior_B_Amp   = _mm_set_ps1(param.exterior_B_Amp  );
	const __m128 exterior_B_Freq  = _mm_set_ps1(param.exterior_B_Freq );
	const __m128 exterior_B_Phase = _mm_set_ps1(param.exterior_B_Phase);
	const __m128 interior_Alpha   = _mm_set_ps1(param.interior_Alpha  );
	const __m128 interior_R_Amp   = _mm_set_ps1(param.interior_R_Amp  );
	const __m128 interior_R_Freq  = _mm_set_ps1(param.interior_R_Freq );
	const __m128 interior_R_Phase = _mm_set_ps1(param.interior_R_Phase);
	const __m128 interior_G_Amp   = _mm_set_ps1(param.interior_G_Amp  );
	const __m128 interior_G_Freq  = _mm_set_ps1(param.interior_G_Freq );
	const __m128 interior_G_Phase = _mm_set_ps1(param.interior_G_Phase);
	const __m128 interior_B_Amp   = _mm_set_ps1(param.interior_B_Amp  );
	const __m128 interior_B_Freq  = _mm_set_ps1(param.interior_B_Freq );
	const __m128 interior_B_Phase = _mm_set_ps1(param.interior_B_Phase);

	__m128 zr1, zr2, zi1, zi2;

	bool f[8];
	for (uint8_t q = 0; q < 8; q++) {
		f[q] = ((param.formula >> q) & 1) ? true : false;
	}
	
	const __m128 s1 = (f[0]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
	const __m128 s2 = (f[1]) ? _mm_set_ps1(-1.0f) : _mm_set_ps1(1.0f);
	const __m128 s3 = (f[2]) ? _mm_set_ps1(-2.0f) : _mm_set_ps1(2.0f);
	
	const __m128 zr1_mask = (f[3]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
	const __m128 zi1_mask = (f[4]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
	const __m128 zr2_mask = (f[5]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
	const __m128 zi2_mask = (f[6]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
	const __m128 zr_mask = (f[7]) ? _mm_set_ps1(-0.0f) : _mm_set_ps1(0.0f);
	
	// printf_m128_fp32(s1);
	// printf_m128_fp32(s2);
	// printf_m128_fp32(s3);
	// printf_m128_u32(zr1_mask);
	// printf_m128_u32(zr1_mask);
	// printf_m128_u32(zi1_mask);
	// printf_m128_u32(zr2_mask);
	// printf_m128_u32(zi2_mask);
	// printf_m128_u32(zr_mask);

	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;
	int32_t y = (int32_t)(p0 / (size_t)param.resX);
	int32_t x = (int32_t)(p0 % (size_t)param.resX);
	x *= param.sample;
	y *= param.sample;
	for (; y < param.resY; y++) {
		fp32 y_val = (fp32)y;
		for (; x < param.resX; x += 4) {
			fp32 x_val = (fp32)x;
			if (p0 >= p1 || ABORT_RENDERING == true) {
				return;
			}
			__m128 outR = _mm_setzero_ps();
			__m128 outG = _mm_setzero_ps();
			__m128 outB = _mm_setzero_ps();
			__m128 outA = _mm_setzero_ps();
			// for (int32_t v = 0; v < param.sample; v++) {
			// 	for (int32_t u = 0; u < param.sample; u++) {
					__m128 xCord = _mm_set_ps(x_val, x_val + 1, x_val + 2, x_val + 3);
					xCord = _mm_sub_ps(xCord, numX);
					xCord = _mm_mul_ps(xCord, recip_numZ);
					__m128 yCord = _mm_set_ps1(y_val);
					yCord = _mm_sub_ps(yCord, numY);
					yCord = _mm_mul_ps(yCord, neg_recip_numW);

					__m128 cr = (!param.juliaSet) ? _mm_add_ps(_mm_sub_ps(_mm_mul_ps(xCord, rotCos_PC), _mm_mul_ps(yCord, rotSin_PC)), realCord) : realJulia;
					__m128 ci = (!param.juliaSet) ? _mm_add_ps(_mm_add_ps(_mm_mul_ps(yCord, rotCos_PC), _mm_mul_ps(xCord, rotSin_PC)), imagCord) : imagJulia;
					__m128 zr = (param.juliaSet) ? _mm_add_ps(_mm_sub_ps(_mm_mul_ps(xCord, rotCos_PC), _mm_mul_ps(yCord, rotSin_PC)), realCord) : realJulia;
					__m128 zi = (param.juliaSet) ? _mm_add_ps(_mm_add_ps(_mm_mul_ps(yCord, rotCos_PC), _mm_mul_ps(xCord, rotSin_PC)), imagCord) : imagJulia;
				
					__m128 low = _mm_set_ps1(4.0f);
					__m128 zs = _mm_setzero_ps();
					__m128 temp_zr = _mm_setzero_ps();
					__m128 current_value_mask = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps());
					for (uint32_t itr = 0; itr < param.maxItr; itr++) {
						// zr1 = zr; //_mm_andnot_ps(zr1_mask, zr);
						// zr2 = zr; //_mm_andnot_ps(zr2_mask, zr);
						// zi1 = zi; //_mm_andnot_ps(zi1_mask, zi);
						// zi2 = zi; //_mm_andnot_ps(zi2_mask, zi);
						//printfInterval(0.3,"\ncr: %f ci: %f", ((float*)((void*)(&cr)))[0], ((float*)((void*)(&ci)))[0]);
						//zr = _mm_add_ps(_mm_andnot_ps(zr_mask, _mm_mul_ps(s1, _mm_add_ps(_mm_mul_ps(zr1, zr),  _mm_mul_ps(s2, _mm_mul_ps(zi1, zi))))), cr);
						//zr = _mm_add_ps(_mm_mul_ps(s1, _mm_add_ps(_mm_mul_ps(zr1, zr),  _mm_mul_ps(s2, _mm_mul_ps(zi1, zi)))), cr);
						//zi = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(zr2, zi2), s3), ci);
						
						temp_zr = _mm_add_ps(_mm_sub_ps(_mm_mul_ps(zr, zr),  _mm_mul_ps(zi, zi)), cr);
						zi = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(zr, zi), s3), ci);
						zr = temp_zr;

						zs = _mm_add_ps(_mm_mul_ps(zr, zr), _mm_mul_ps(zi, zi)); 
						low = _mm_min_ps(zs, low);
						zs = _mm_and_ps(zs, current_value_mask);
						int break_mask = _mm_movemask_ps(_mm_cmpgt_ps(zs, breakoutValue));
						if (break_mask != 0) {
							for (int i = 0; i < 4; i++) {
								if (break_mask & (1 << i)) {
									CPU_Exterior_Coloring_SSE2_FP32(
										((float*)((void*)(&outR)))[i], ((float*)((void*)(&outG)))[i], ((float*)((void*)(&outB)))[i], ((float*)((void*)(&outA)))[i],
										param,
										itr, ((float*)((void*)(&zs)))[i], inverse_log2_power
									);
									((float*)((void*)(&current_value_mask)))[i] = 0.0f;
								}
							}
							if (_mm_movemask_ps(_mm_cmpeq_ps(current_value_mask, _mm_setzero_ps())) == 0) {
								break;
							}
						}
					}
					int inside_value_mask = _mm_movemask_ps(_mm_cmpneq_ps(current_value_mask, _mm_setzero_ps()));
					if (inside_value_mask != 0) {
						for (int i = 0; i < 4; i++) {
							if (inside_value_mask & (1 << i)) {
								CPU_Interior_Coloring_SSE2_FP32(
									((float*)((void*)(&outR)))[i], ((float*)((void*)(&outG)))[i], ((float*)((void*)(&outB)))[i], ((float*)((void*)(&outA)))[i],
									param,
									((float*)((void*)(&low)))[i]
								);
							}
						}
					}
			// 		x++;
			// 	}
			// 	x -= param.sample;
			// 	y++;
			// }
			// y -= param.sample;
			const __m128 div_zero_mask = _mm_cmpeq_ps(outA, _mm_setzero_ps());
			outR = _mm_div_ps(outR, outA);
			outG = _mm_div_ps(outG, outA);
			outB = _mm_div_ps(outB, outA);
			outR = _mm_andnot_ps(div_zero_mask, outR);
			outG = _mm_andnot_ps(div_zero_mask, outG);
			outB = _mm_andnot_ps(div_zero_mask, outB);
			outA = _mm_div_ps(outA, sampleDiv);
			const __m128 color_mult = _mm_set_ps1(255.0f);
			outR = _mm_mul_ps(outR, color_mult);
			outG = _mm_mul_ps(outG, color_mult);
			outB = _mm_mul_ps(outB, color_mult);
			outA = _mm_mul_ps(outA, color_mult);
			for (int i = 4 - 1; i >= 0; i--) {
				data[dataPtr] = (uint8_t)((float*)((void*)(&outR)))[i]; dataPtr++;
				data[dataPtr] = (uint8_t)((float*)((void*)(&outG)))[i]; dataPtr++;
				data[dataPtr] = (uint8_t)((float*)((void*)(&outB)))[i]; dataPtr++;
				data[dataPtr] = (uint8_t)((float*)((void*)(&outA)))[i]; dataPtr++;
			}
			//dataPtr += 4;
			p0 += 4;
		}
		x = 0;\
	}
}