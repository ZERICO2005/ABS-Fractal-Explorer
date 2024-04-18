/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_MULTI_INTERNAL_H
#define FRAC_MULTI_INTERNAL_H

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"
#include "render.h"

const uint64_t factorialLUT[] = {
	/*        0! */ 1,
	/*  1! -  4! */ 1, 2, 6, 24,
	/*  5! -  8! */ 120, 720,5040,40320,
	/*  9! - 12! */ 362880, 3628800,39916800,479001600,
	/* 13! - 16! */ 6227020800, 87178291200,1307674368000, 20922789888000,
	/* 17! - 20! */ 355687428096000, 6402373705728000, 121645100408832000, 2432902008176640000
};

#define nPr(n,r) (factorialLUT[(n)] / (factorialLUT[(n) - (r)]))
#define nCr(n,r) (factorialLUT[(n)] / (factorialLUT[(r)] * factorialLUT[(n) - (r)]))

#define FractalParameters(fpX, fpColor) uint8_t* data, const PreCalc_Param<fpX, fpColor> param, size_t p0, const size_t p1, std::atomic<bool>& ABORT_RENDERING

constexpr inline fp64 inverse_log2(fp64 p) { return 1.0 / log2(p); }

template<typename fpX, typename fpColor>
struct PreCalc_Param {
	/* Header */
		uint64_t formula;
		uint32_t power;
		dim32_t resX;
		dim32_t resY;
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
		fpColor sampleDiv;

		fpColor exterior_Alpha;
			fpColor exterior_R_Amp;
			fpColor exterior_R_Freq;
			fpColor exterior_R_Phase;
			fpColor exterior_G_Amp;
			fpColor exterior_G_Freq;
			fpColor exterior_G_Phase;
			fpColor exterior_B_Amp;
			fpColor exterior_B_Freq;
			fpColor exterior_B_Phase;
		fpColor interior_Alpha;
			fpColor interior_R_Amp;
			fpColor interior_R_Freq;
			fpColor interior_R_Phase;
			fpColor interior_G_Amp;
			fpColor interior_G_Freq;
			fpColor interior_G_Phase;
			fpColor interior_B_Amp;
			fpColor interior_B_Freq;
			fpColor interior_B_Phase;
};

#endif /* FRAC_MULTI_INTERNAL_H */