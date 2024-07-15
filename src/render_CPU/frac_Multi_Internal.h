/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_MULTI_INTERNAL_H
#define FRAC_MULTI_INTERNAL_H

#include "../Common_Def.h"
#include "../Program_Def.h"
#include "../copyBuffer.h"
#include "../fractal.h"
#include "../render.h"

/* Rendering Modes */

	#define ENABLE_SSE2_RENDERING
	
	#define ENABLE_AVX_RENDERING

	// Development for AVX512 will be held off for a bit as I don't have any CPU's to test it on. Although I have heard Intel has a CPU emulator that I could use to test AVX512
	// #define ENABLE_AVX512_F_RENDERING
	// #define ENABLE_AVX512_FP16_RENDERING
	// #define ENABLE_AVX512_BF16_RENDERING

/* Constants */
	constexpr size_t bitCount_Float16 = 16;
	constexpr size_t bitCount_Float32 = 32;
	constexpr size_t bitCount_Float64 = 64;

	constexpr size_t bitCount_SSE_Family = 128;
	constexpr size_t bitCount_AVX_Family = 256;
	constexpr size_t bitCount_AVX512_Family = 512;

/* Spacing */

	constexpr size_t Spacing_Generic = 1;

	constexpr size_t Spacing_SSE2_Float32 = (bitCount_SSE_Family / bitCount_Float32);
	constexpr size_t Spacing_SSE2_Float64 = (bitCount_SSE_Family / bitCount_Float64);

	constexpr size_t Spacing_AVX_Float32 = (bitCount_AVX_Family / bitCount_Float32);
	constexpr size_t Spacing_AVX_Float64 = (bitCount_AVX_Family / bitCount_Float64);

	constexpr size_t Spacing_AVX512_F_Float32 = (bitCount_AVX512_Family / bitCount_Float32);
	constexpr size_t Spacing_AVX512_F_Float64 = (bitCount_AVX512_Family / bitCount_Float64);
	constexpr size_t Spacing_AVX512_FP16_Float16 = (bitCount_AVX512_Family / bitCount_Float16);
	
/* Factorials */

	constexpr uint64_t factorialLUT[] = {
		/*        0! */ 1,
		/*  1! -  4! */ 1, 2, 6, 24,
		/*  5! -  8! */ 120, 720,5040,40320,
		/*  9! - 12! */ 362880, 3628800,39916800,479001600,
		/* 13! - 16! */ 6227020800, 87178291200,1307674368000, 20922789888000,
		/* 17! - 20! */ 355687428096000, 6402373705728000, 121645100408832000, 2432902008176640000
	};

	#define nPr(n,r) (factorialLUT[(n)] / (factorialLUT[(n) - (r)]))
	#define nCr(n,r) (factorialLUT[(n)] / (factorialLUT[(r)] * factorialLUT[(n) - (r)]))

#define FractalParameters(fpX, fpColor) \
	uint8_t* data, const PreCalc_Param<fpX, fpColor> param, size_t p0, const size_t p1, std::atomic<bool>& ABORT_RENDERING

template<typename fpX>
constexpr inline fpX inverse_log2(fpX p) { return (fpX)1.0 / log2(p); }

template<typename fpX, typename fpColor>
struct PreCalc_Param {
	/* Header */
		uint64_t formula;
		uint32_t power;
		dim32_t Image_ResX;
		dim32_t Image_ResY;
		dim32_t Cord_ResX;
		dim32_t Cord_ResY;
		int32_t sample;
		uint32_t maxItr;
		bool juliaSet;
	/* Coordinates */
		fpX realCord;
		fpX imagCord;
		fpX realJulia;
		fpX imagJulia;
		fpX zoom_PC;
		fpX rotSin_PC;
		fpX rotCos_PC;
		fpX breakoutValue;
	/* Constants */
		fpX numY;
		fpX numX;
		fpX recip_numZ;
		fpX neg_recip_numW;
	/* Polar */
		fpX polarPower;
		fpX polarPowerHalf;
	/* Coloring */
		fpColor inverse_log2_power;
		fpColor alphaDiv;

		fpColor Exterior_Alpha;
			fpColor Exterior_R_Amp_mult_Exterior_Alpha;
			fpColor Exterior_R_Freq_mult_TAU;
			fpColor Exterior_R_Phase_mult_TAU;
			fpColor Exterior_G_Amp_mult_Exterior_Alpha;
			fpColor Exterior_G_Freq_mult_TAU;
			fpColor Exterior_G_Phase_mult_TAU;
			fpColor Exterior_B_Amp_mult_Exterior_Alpha;
			fpColor Exterior_B_Freq_mult_TAU;
			fpColor Exterior_B_Phase_mult_TAU;
		fpColor Interior_Alpha;
			fpColor Interior_R_Amp_mult_Interior_Alpha;
			fpColor Interior_R_Freq;
			fpColor Interior_R_Phase_mult_TAU;
			fpColor Interior_G_Amp_mult_Interior_Alpha;
			fpColor Interior_G_Freq;
			fpColor Interior_G_Phase_mult_TAU;
			fpColor Interior_B_Amp_mult_Interior_Alpha;
			fpColor Interior_B_Freq;
			fpColor Interior_B_Phase_mult_TAU;
};

template <typename fpX, typename fpColor>
void Generate_PreCalc_Param(
	PreCalc_Param<fpX,fpColor>& preCalc_Param,
	const BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param
) {
	// nano64_t startTimer = getNanoTime();
	if (buf == nullptr) { return; }
	/* Header */
		preCalc_Param.formula = param.formula;
		preCalc_Param.power = param.power;
		preCalc_Param.Image_ResX = buf->resX;
		preCalc_Param.Image_ResY = buf->resY;
		preCalc_Param.Cord_ResX = buf->resX * ren.sample;
		preCalc_Param.Cord_ResY = buf->resY * ren.sample;
		preCalc_Param.sample = ren.sample;
		preCalc_Param.maxItr = param.maxItr;
		preCalc_Param.juliaSet = param.juliaSet;
	/* Coordinates */
		preCalc_Param.realCord = (fpX)param.r;
		preCalc_Param.imagCord = (fpX)param.i;
		bool use_startingZ = (param.startingZ || param.juliaSet) ? true : false;
		preCalc_Param.realJulia = use_startingZ ? (fpX)param.zr : (fpX)0.0;
		preCalc_Param.imagJulia = use_startingZ ? (fpX)param.zi : (fpX)0.0;
		const fpCord zoom_PC = pow((fpCord)10.0, (fpCord)param.zoom);
		preCalc_Param.zoom_PC = (fpX)zoom_PC;
		preCalc_Param.rotSin_PC = (fpX)sin((fpCord)param.rot);
		preCalc_Param.rotCos_PC = (fpX)cos((fpCord)param.rot);
		preCalc_Param.breakoutValue = (fpX)param.breakoutValue;
		if (std::is_same<fpX, fp32>::value && preCalc_Param.breakoutValue > 65536.0f) {
			preCalc_Param.breakoutValue = 65536.0f;
		}
		
		
		const dim32_t sResY = preCalc_Param.Cord_ResY - 1;
		const dim32_t sResX = preCalc_Param.Cord_ResX - 1;
		const fpCord numY = ((fpCord)sResY / (fpCord)2.0);
		const fpCord numX = ((fpCord)sResX / (fpCord)2.0);
		const fpCord numT = (sResX >= sResY) ?
			(numY * zoom_PC) :
			(numX * zoom_PC);
		preCalc_Param.numY = (fpX)numY;
		preCalc_Param.numX = (fpX)numX;
		preCalc_Param.recip_numZ = (fpX)((fpCord)param.sX / numT);
		preCalc_Param.neg_recip_numW = (fpX)(-((fpCord)param.sY / numT));
		
	/* Polar */
		preCalc_Param.polarPower = (fpX)param.polarPower;
		preCalc_Param.polarPowerHalf = (fpX)param.polarPower / (fpX)2.0;
	/* Coloring */
		if (param.polarMandelbrot == true) {
			preCalc_Param.inverse_log2_power = (fpColor)1.0 / log2((fpColor)param.polarPower);
		} else {
			preCalc_Param.inverse_log2_power = (fpColor)1.0 / log2((fpColor)param.power);
		}
		preCalc_Param.alphaDiv = (fpColor)(ren.sample * ren.sample);

		preCalc_Param.Exterior_Alpha = (fpColor)param.exterior_Alpha;
			preCalc_Param.Exterior_R_Amp_mult_Exterior_Alpha = (fpColor)(param.exterior_R_Amp * param.exterior_Alpha);
			preCalc_Param.Exterior_R_Freq_mult_TAU           = (fpColor)(param.exterior_R_Freq  * TAU               );
			preCalc_Param.Exterior_R_Phase_mult_TAU          = (fpColor)(param.exterior_R_Phase * TAU               );
			preCalc_Param.Exterior_G_Amp_mult_Exterior_Alpha = (fpColor)(param.exterior_G_Amp * param.exterior_Alpha);
			preCalc_Param.Exterior_G_Freq_mult_TAU           = (fpColor)(param.exterior_G_Freq  * TAU               );
			preCalc_Param.Exterior_G_Phase_mult_TAU          = (fpColor)(param.exterior_G_Phase * TAU               );
			preCalc_Param.Exterior_B_Amp_mult_Exterior_Alpha = (fpColor)(param.exterior_B_Amp * param.exterior_Alpha);
			preCalc_Param.Exterior_B_Freq_mult_TAU           = (fpColor)(param.exterior_B_Freq  * TAU               );
			preCalc_Param.Exterior_B_Phase_mult_TAU          = (fpColor)(param.exterior_B_Phase * TAU               );
		preCalc_Param.Interior_Alpha = (fpColor)param.interior_Alpha;
			preCalc_Param.Interior_R_Amp_mult_Interior_Alpha = (fpColor)(param.interior_R_Amp * param.interior_Alpha);
			preCalc_Param.Interior_R_Freq                    = (fpColor) param.interior_R_Freq                       ;
			preCalc_Param.Interior_R_Phase_mult_TAU          = (fpColor)(param.interior_R_Phase * TAU)               ;
			preCalc_Param.Interior_G_Amp_mult_Interior_Alpha = (fpColor)(param.interior_G_Amp * param.interior_Alpha);
			preCalc_Param.Interior_G_Freq                    = (fpColor) param.interior_G_Freq                       ;
			preCalc_Param.Interior_G_Phase_mult_TAU          = (fpColor)(param.interior_G_Phase * TAU)               ;
			preCalc_Param.Interior_B_Amp_mult_Interior_Alpha = (fpColor)(param.interior_B_Amp * param.interior_Alpha);
			preCalc_Param.Interior_B_Freq                    = (fpColor) param.interior_B_Freq                       ;
			preCalc_Param.Interior_B_Phase_mult_TAU          = (fpColor)(param.interior_B_Phase * TAU)               ;
		
		#ifdef MONOCHROME_MODE
			preCalc_Param.Exterior_G_Amp_mult_Exterior_Alpha = preCalc_Param.Exterior_R_Amp_mult_Exterior_Alpha;
			preCalc_Param.Exterior_G_Freq_mult_TAU           = preCalc_Param.Exterior_R_Freq_mult_TAU          ;
			preCalc_Param.Exterior_G_Phase_mult_TAU          = preCalc_Param.Exterior_R_Phase_mult_TAU         ;
			preCalc_Param.Exterior_B_Amp_mult_Exterior_Alpha = preCalc_Param.Exterior_R_Amp_mult_Exterior_Alpha;
			preCalc_Param.Exterior_B_Freq_mult_TAU           = preCalc_Param.Exterior_R_Freq_mult_TAU          ;
			preCalc_Param.Exterior_B_Phase_mult_TAU          = preCalc_Param.Exterior_R_Phase_mult_TAU         ;

			preCalc_Param.Interior_R_Amp_mult_Interior_Alpha = preCalc_Param.Interior_B_Amp_mult_Interior_Alpha;
			preCalc_Param.Interior_R_Freq                    = preCalc_Param.Interior_B_Freq                   ;
			preCalc_Param.Interior_R_Phase_mult_TAU          = preCalc_Param.Exterior_B_Phase_mult_TAU         ;
			preCalc_Param.Interior_G_Amp_mult_Interior_Alpha = preCalc_Param.Interior_B_Amp_mult_Interior_Alpha;
			preCalc_Param.Interior_G_Freq                    = preCalc_Param.Interior_B_Freq                   ;
			preCalc_Param.Interior_G_Phase_mult_TAU          = preCalc_Param.Exterior_B_Phase_mult_TAU         ;
		#endif
	// nano64_t finishTimer = getNanoTime();
	// printfInterval(0.3,"\nPreCalc: %.3lfus %.3lf",
	// 	NANO_TO_SECONDS(finishTimer - startTimer) * 1.0e6, NANO_TO_FRAMERATE(finishTimer - startTimer)
	// );
}

#endif /* FRAC_MULTI_INTERNAL_H */