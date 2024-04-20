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
#include "frac_Multi.h"
#include "frac_Multi_SSE2.h"

#include "fractal.h"
#include "render.h"

/* General Formulas */

/* BOILERPLATE */

	#define CPU_Interior_Coloring(fpX, fpColor); \
		outR += param.Interior_R_Amp_mult_Interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.Interior_R_Freq + param.Interior_R_Phase));\
		outG += param.Interior_G_Amp_mult_Interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.Interior_G_Freq + param.Interior_G_Phase));\
		outB += param.Interior_B_Amp_mult_Interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.Interior_B_Freq + param.Interior_B_Phase));\
		outA += param.Interior_Alpha;

	#define CPU_Exterior_Coloring(fpX, fpColor, inverse_log2_power); \
		fpColor smooth = (fpColor)log1p((fpColor)fmax((fpColor)0.0, (fpColor)itr - (fpColor)log2(log2(zs) / (fpX)2.0) * inverse_log2_power));\
		outR += param.Exterior_R_Amp_mult_Exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(param.Exterior_R_Freq_mult_TAU * smooth + param.Exterior_R_Phase_mult_TAU));\
		outG += param.Exterior_G_Amp_mult_Exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(param.Exterior_G_Freq_mult_TAU * smooth + param.Exterior_G_Phase_mult_TAU));\
		outB += param.Exterior_B_Amp_mult_Exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(param.Exterior_B_Freq_mult_TAU * smooth + param.Exterior_B_Phase_mult_TAU));\
		outA += param.Exterior_Alpha;

#define Block_Init_Generic(fpX, fpColor);

#define Block_BeginLoop_Generic(fpX, fpColor); \
	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;\
	int32_t y = (int32_t)(p0 / (size_t)param.Image_ResX);\
	int32_t x = (int32_t)(p0 % (size_t)param.Image_ResX);\
	x *= param.sample;\
	y *= param.sample;\
	for (; y < param.Cord_ResY; y += param.sample) {\
		for (; x < param.Cord_ResX; x += param.sample) {\
			if (p0 >= p1 || ABORT_RENDERING == true) {\
				return;\
			}\
			fpColor outR = (fpColor)0.0;\
			fpColor outG = (fpColor)0.0;\
			fpColor outB = (fpColor)0.0;\
			fpColor outA = (fpColor)0.0;\
			for (int32_t v = 0; v < param.sample; v++) {\
				fpX yCord = (((fpX)y - param.numY) * param.neg_recip_numW);\
				for (int32_t u = 0; u < param.sample; u++) {\
					fpX xCord = (((fpX)x - param.numX) * param.recip_numZ);\
					fpX cr = (!param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;\
					fpX ci = (!param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;\
					fpX zr = (param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;\
					fpX zi = (param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;\
					\
					fpX low = (fpX)4.0;\
					fpX zs = (fpX)0.0;\
					fpX __attribute__((unused)) temp_zr = (fpX)0.0;\
					for (uint32_t itr = 0; itr < param.maxItr; itr++) {

#define Block_EndLoop_Generic(fpX, fpColor, l); \
						zs = zr * zr + zi * zi;\
						if (zs < low) {\
							low = zs;\
						} else if (zs > param.breakoutValue) {\
							CPU_Exterior_Coloring(fpX, fpColor, l);\
							break;\
						}\
					}\
					if (zs <= param.breakoutValue) {\
						CPU_Interior_Coloring(fpX, fpColor);\
					}\
					x++;\
				}\
				x -= param.sample;\
				y++;\
			}\
			y -= param.sample;\
			if (outA != (fpColor)0.0) {\
				outR = outR / outA;\
				outG = outG / outA;\
				outB = outB / outA;\
				outA = outA / param.alphaDiv;\
			}\
			outR *= (fpColor)255.0;\
			outG *= (fpColor)255.0;\
			outB *= (fpColor)255.0;\
			outA *= (fpColor)255.0;\
			data[dataPtr] = (uint8_t)outR; dataPtr++;\
			data[dataPtr] = (uint8_t)outG; dataPtr++;\
			data[dataPtr] = (uint8_t)outB; dataPtr++;\
			data[dataPtr] = (uint8_t)outA; dataPtr++;\
			p0++;\
		}\
		x = 0;\
	}

/* BOILERPLATE */

template<typename fpX, typename fpColor>
void quadraticRender(FractalParameters(fpX, fpColor)) {

	Block_Init_Generic(fpX, fpColor);

		fpX zr1, zr2, zi1, zi2, s1, s2, s3;
		bool f[8];
		for (uint8_t q = 0; q < 8; q++) {
			f[q] = ((param.formula >> q) & 1) ? true : false;
		}
		
		s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (f[1]) ? (fpX)-1.0 : (fpX)1.0;
		s3 = (f[2]) ? (fpX)-2.0 : (fpX)2.0;
		
	Block_BeginLoop_Generic(fpX, fpColor);

		zr1 = (f[3]) ? fabs(zr) : zr;
		zi1 = (f[4]) ? fabs(zi) : zi;
		zr2 = (f[5]) ? fabs(zr) : zr;
		zi2 = (f[6]) ? fabs(zi) : zi;
		zr = (f[7]) ?
			(s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr) :
			(s1 *     ((zr1 * zr) - s2 * (zi1 * zi)) + cr);
		zi = (zr2 * zi2 * s3) + ci;
	
	Block_EndLoop_Generic(fpX, fpColor, inverse_log2(2.0));
}
	
template<typename fpX, typename fpColor>
void cubicRender(FractalParameters(fpX, fpColor)) {
	
	Block_Init_Generic(fpX, fpColor);

		fpX zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;
		bool f[14];
		for (uint8_t q = 0; q < 14; q++) {
			f[q] = ((param.formula >> q) & 1) ? true : false;
		}
		s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (f[1]) ? (fpX)-3.0 : (fpX)3.0;
		s3 = (f[2]) ? (fpX)-3.0 : (fpX)3.0;
		s4 = (f[3]) ? (fpX)-1.0 : (fpX)1.0;
		s5 = (f[4]) ? (fpX)-1.0 : (fpX)1.0;
		s6 = (f[5]) ? (fpX)-1.0 : (fpX)1.0;
	
	Block_BeginLoop_Generic(fpX, fpColor);

		zr1 = (f[6]) ? fabs(zr) : zr;
		zi1 = (f[7]) ? fabs(zi) : zi;
		zr2 = (f[8]) ? fabs(zr) : zr;
		zi2 = (f[9]) ? fabs(zi) : zi;
		zr3 = (f[10]) ? fabs(zr) : zr;
		zi3 = (f[11]) ? fabs(zi) : zi;
		temp_zr = (f[12]) ?
			(s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr) :
			(s5 *     ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr);
		zi =      (f[13]) ?
			(s6 * fabs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci) :
			(s6 *     ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci);
		zr = temp_zr;
	
	Block_EndLoop_Generic(fpX, fpColor, inverse_log2(3.0));
}
	
template<typename fpX, typename fpColor>
void quarticRender(FractalParameters(fpX, fpColor)) {

	Block_Init_Generic(fpX, fpColor);

		fpX zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;
		bool f[17];
		for (uint8_t q = 0; q < 17; q++) {
			f[q] = ((param.formula >> q) & 1) ? true : false;
		}
		s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (f[1]) ? (fpX)-6.0 : (fpX)6.0;
		s3 = (f[2]) ? (fpX)-1.0 : (fpX)1.0;
		s4 = (f[3]) ? (fpX)-4.0 : (fpX)4.0;
		s5 = (f[4]) ? (fpX)-4.0 : (fpX)4.0;
		s6 = (f[5]) ? (fpX)-1.0 : (fpX)1.0;
		s7 = (f[6]) ? (fpX)-1.0 : (fpX)1.0;
	
	Block_BeginLoop_Generic(fpX, fpColor);

		zr1 = (f[7]) ? fabs(zr) : zr;
		zi1 = (f[8]) ? fabs(zi) : zi;
		zr2 = (f[9]) ? fabs(zr) : zr;
		zi2 = (f[10]) ? fabs(zi) : zi;
		zr3 = (f[11]) ? fabs(zr) : zr;
		zi3 = (f[12]) ? fabs(zi) : zi;
		zr4 = (f[13]) ? fabs(zr) : zr;
		zi4 = (f[14]) ? fabs(zi) : zi;
		
		temp_zr = (f[15]) ?
			(s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr) :
			(s6 *     (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr);
		zi =      (f[16]) ?
			(s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci) :
			(s7 *     (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci);
		zr = temp_zr;
	
	Block_EndLoop_Generic(fpX, fpColor, inverse_log2(4.0));
}
	
template<typename fpX, typename fpColor>
void quinticRender(FractalParameters(fpX, fpColor)) {

	Block_Init_Generic(fpX, fpColor);

		fpX zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;
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
		s1 = (fS[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (fS[1]) ? (fpX)-10.0 : (fpX)10.0;
		s3 = (fS[2]) ? (fpX)-5.0 : (fpX)5.0;
		s4 = (fS[3]) ? (fpX)-5.0 : (fpX)5.0;
		s5 = (fS[4]) ? (fpX)-10.0 : (fpX)10.0;
		s6 = (fS[5]) ? (fpX)-1.0 : (fpX)1.0;
		s7 = (fO[0]) ? (fpX)-1.0 : (fpX)1.0;
		s8 = (fO[1]) ? (fpX)-1.0 : (fpX)1.0;
	
	Block_BeginLoop_Generic(fpX, fpColor);

		zr1 = (fA[0]) ? fabs(zr) : zr;
		zi1 = (fA[1]) ? fabs(zi) : zi;
		zr2 = (fA[2]) ? fabs(zr) : zr;
		zi2 = (fA[3]) ? fabs(zi) : zi;
		zr3 = (fA[4]) ? fabs(zr) : zr;
		zi3 = (fA[5]) ? fabs(zi) : zi;
		zr4 = (fA[6]) ? fabs(zr) : zr;
		zi4 = (fA[7]) ? fabs(zi) : zi;
		zr5 = (fA[8]) ? fabs(zr) : zr;
		zi5 = (fA[9]) ? fabs(zi) : zi;
		
		temp_zr = (fO[2]) ?
			(s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr) :
			(s7 *     (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr);
		zi =      (fO[3]) ?
			(s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci) :
			(s8 *     (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci);
		zr = temp_zr;
	
	Block_EndLoop_Generic(fpX, fpColor, inverse_log2(5.0));
}

template<typename fpX, typename fpColor>
void sexticRender(FractalParameters(fpX, fpColor)) {

	Block_Init_Generic(fpX, fpColor);

		fpX zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6, s1, s2, s3, s4, s5, s6, s7, s8, s9;
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
		s1 = (fS[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (fS[1]) ? (fpX)-15.0 : (fpX)15.0;
		s3 = (fS[2]) ? (fpX)-15.0 : (fpX)15.0;
		s4 = (fS[3]) ? (fpX)-1.0 : (fpX)1.0;
		s5 = (fS[4]) ? (fpX)-6.0 : (fpX)6.0;
		s6 = (fS[5]) ? (fpX)-20.0 : (fpX)20.0;
		s7 = (fS[6]) ? (fpX)-6.0 : (fpX)6.0;
		s8 = (fO[0]) ? (fpX)-1.0 : (fpX)1.0;
		s9 = (fO[1]) ? (fpX)-1.0 : (fpX)1.0;
	
	Block_BeginLoop_Generic(fpX, fpColor);

		zr1 = (fA[0]) ? fabs(zr) : zr;
		zi1 = (fA[1]) ? fabs(zi) : zi;
		zr2 = (fA[2]) ? fabs(zr) : zr;
		zi2 = (fA[3]) ? fabs(zi) : zi;
		zr3 = (fA[4]) ? fabs(zr) : zr;
		zi3 = (fA[5]) ? fabs(zi) : zi;
		zr4 = (fA[6]) ? fabs(zr) : zr;
		zi4 = (fA[7]) ? fabs(zi) : zi;
		zr5 = (fA[8]) ? fabs(zr) : zr;
		zi5 = (fA[9]) ? fabs(zi) : zi;
		zr6 = (fA[10]) ? fabs(zr) : zr;
		zi6 = (fA[11]) ? fabs(zi) : zi;
		
		temp_zr = (fO[2]) ?
			(s8 * fabs(s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr) :
			(s8 *     (s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr);
		zi =      (fO[3]) ?
			(s9 * fabs(s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci) :
			(s9 *     (s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci);
		zr = temp_zr;

	Block_EndLoop_Generic(fpX, fpColor, inverse_log2(6.0));
}

/*
**	The commented out code below is generalized code for rendering N'th power ABS-Mandelbrot-Sets;
**	Although the code currently crashes for the moment, I will return to it later.
**	Perhaps it may just be easier to write some code to generate some code of every power up to the 19th power;
**	The 19th power is the limit since it is the highest power with less than 2^64 formulas (2^62).
**	Other limits include the 26th power since nCr(26,13) is just barely less than the float32 integer limit of 2^24 or 16777216; 
**	In addition to the 56th power since nCr(56,23) is also barely less than the float64 integer limit of 2^53.
*/

// template <typename fpX, typename fpColor>
// void polynomialRender(FractalParameters(fpX, fpColor)) {
// 	const uint32_t power = param.power;
// 	constexpr size_t sizeOuter = 4;
// 	const size_t sizeSign = power + 1;
// 	const size_t sizeAbs = power * 2;
// 	bool fOuter[sizeOuter];
// 	fpX* fSign = (fpX*)calloc(sizeSign, sizeof(fpX));
// 	bool* fAbs = (bool*)calloc(sizeAbs, sizeof(bool));
// 	{
// 		size_t sIndex = 0;
// 		for (size_t s = 0; s < sizeSign; s += 2) { // Real Coefficients
// 			fSign[sIndex] = (fpX)nCr(power,s);
// 			fSign[sIndex] *= (param.formula & (0x1 << s)) ? (fpX)(-1.0) : (fpX)(1.0);
// 			sIndex++;
// 		}
// 		for (size_t s = 1; s < sizeSign; s += 2) { // Imag Coefficients
// 			fSign[sIndex] = (fpX)nCr(power,s);
// 			fSign[sIndex] *= (param.formula & (0x1 << s)) ? (fpX)(-1.0) : (fpX)(1.0);
// 			sIndex++;
// 		}
// 		for (size_t a = 0; a < sizeAbs; a++) {
// 			fAbs[a] = (param.formula & (0x1 << (sizeSign + 2 + a)));
// 		}
// 		fOuter[0] = (param.formula & (0x1 << (sizeSign))); // Real Sign
// 		fOuter[1] = (param.formula & (0x1 << (sizeSign + 1))); // Imag Sign
// 		fOuter[2] = (param.formula & (0x1 << (sizeSign + 2 + sizeAbs))); // Real Abs
// 		fOuter[3] = (param.formula & (0x1 << (sizeSign + 2 + sizeAbs + 1))); // Imag Abs
// 	}
// 	uint32_t realTerms = (power + 2) / 2;
// 	uint32_t imagTerms = (power + 1) / 2;
	
// 	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;
// 	int32_t y = (int32_t)(p0 / (size_t)param.Image_ResX);
// 	int32_t x = (int32_t)(p0 % (size_t)param.Image_ResX);
// 	x *= param.sample;
// 	y *= param.sample;
// 	for (; y < param.Cord_ResY; y += param.sample) {
// 		for (; x < param.Cord_ResX; x += param.sample) {
// 			if (p0 == p1 || ABORT_RENDERING == true) {
// 				FREE(fSign);
// 				FREE(fAbs);
// 				return;
// 			}
// 			fp64 outR = 0.0;
// 			fp64 outG = 0.0;
// 			fp64 outB = 0.0;
// 			fp64 outA = 0.0;
// 			for (int32_t v = 0; v < param.sample; v++) {
// 				fpX yCord = (((fpX)y - param.numY) * param.neg_recip_numW);
// 				for (int32_t u = 0; u < param.sample; u++) {
// 					fpX xCord = (((fpX)x - param.numX) * param.recip_numZ);

// 					fpX cr = (!param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;
// 					fpX ci = (!param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;
// 					fpX zr = (param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;
// 					fpX zi = (param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;

// 					fpX low = (fpX)4.0;
// 					fpX temp_zr;
// 					fpX zs = (fpX)0.0;
// 					for (uint32_t itr = 0; itr < param.maxItr; itr++) {
// 						size_t absR = 0;
// 						size_t absI = 1;
// 						fpX zrAcc = (fpX)0.0;
// 						fpX ziAcc = (fpX)0.0;
// 						for (uint32_t rT = 0; rT < realTerms; rT++) {
// 							fpX acc = (fpX)1.0;
// 							for (int32_t m = 0; m < ((int32_t)power - (2 * (int32_t)rT)) - 1; m++) {
// 								acc *= zr;
// 							}
// 							if (rT != realTerms - 1 || power % 2 != 0) {
// 								acc *= (fAbs[absR]) ? fabs(zr) : zr;
// 								absR += 2;
// 							}
// 							for (int32_t m = 0; m < (2 * (int32_t)rT) - 1; m++) {
// 								acc *= zi;
// 							}
// 							if (rT != 0) {
// 								acc *= (fAbs[absI]) ? fabs(zi) : zi;
// 								absI += 2;
// 							}
// 							acc *= fSign[rT];
// 							zrAcc += acc;
// 						}
// 						for (uint32_t iT = 0; iT < imagTerms; iT++) {
// 							fpX acc = (fpX)1.0;
// 							acc *= fSign[iT];
// 							ziAcc += acc;
// 						}
						
// 						ziAcc = zr * zi * 2;
// 						zrAcc = (fOuter[2]) ? fabs(zrAcc) : zrAcc;
// 						ziAcc = (fOuter[3]) ? fabs(ziAcc) : ziAcc;
// 						zrAcc *= (fOuter[0]) ? (fpX)(-1.0) : (fpX)(1.0);
// 						ziAcc *= (fOuter[1]) ? (fpX)(-1.0) : (fpX)(1.0);
// 						zr = zrAcc + cr;
// 						zi = ziAcc + ci;

// 						zs = zr * zr + zi * zi;

// 						if (zs < low) {
// 							low = zs;
// 						} else if (zs > param.breakoutValue) {
// 							CPU_Exterior_Coloring(fpX, fpColor, param.inverse_log2_power);
// 							break;
// 						}
// 					}
// 					if (zs <= param.breakoutValue) {
// 						CPU_Interior_Coloring(fpX, fpColor);
// 					}
// 					x++;
// 				}
// 				x -= param.sample;
// 				y++;
// 			}
// 			y-= param.sample;
// 			if (outA != 0.0) {
// 				outR = outR / outA;
// 				outG = outG / outA;
// 				outB = outB / outA;
// 				outA = outA / param.alphaDiv;
// 			}
// 			outR *= (fpColor)255.0;
// 			outG *= (fpColor)255.0;
// 			outB *= (fpColor)255.0;
// 			outA *= (fpColor)255.0;
// 			data[dataPtr] = (uint8_t)outR; dataPtr++;
// 			data[dataPtr] = (uint8_t)outG; dataPtr++;
// 			data[dataPtr] = (uint8_t)outB; dataPtr++;
// 			data[dataPtr] = (uint8_t)outA; dataPtr++;
// 			p0++;
// 		}
// 		x = 0;
// 	}
// 	FREE(fSign);
// 	FREE(fAbs);
// }

template<typename fpX, typename fpColor>
void polarRender(FractalParameters(fpX, fpColor)) {
	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;
	int32_t y = (int32_t)(p0 / (size_t)param.Image_ResX);
	int32_t x = (int32_t)(p0 % (size_t)param.Image_ResX);
	x *= param.sample;
	y *= param.sample;
	const fpX power = param.polarPower;
	const fpX powerHalf = param.polarPowerHalf;
	for (; y < param.Cord_ResY; y += param.sample) {
		for (; x < param.Cord_ResX; x += param.sample) {
			if (p0 >= p1 || ABORT_RENDERING == true) {
				return;
			}
			fp64 outR = 0.0;
			fp64 outG = 0.0;
			fp64 outB = 0.0;
			fp64 outA = 0.0;
			for (int32_t v = 0; v < param.sample; v++) {
				fpX yCord = (((fpX)y - param.numY) * param.neg_recip_numW);
				for (int32_t u = 0; u < param.sample; u++) {
					fpX xCord = (((fpX)x - param.numX) * param.recip_numZ);

					fpX cr = (!param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;
					fpX ci = (!param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;
					fpX zr = (param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;
					fpX zi = (param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;
					
					fpX low = (fpX)4.0;
					fpX zs = (fpX)(zr * zr + zi * zi);
					fpX za = 0.0;
					for (uint32_t itr = 0; itr < param.maxItr; itr++) {
						za = atan2(zi, zr) * power;
						zr = pow(zs, powerHalf) * cos(za) + cr;
						zi = pow(zs, powerHalf) * sin(za) + ci;
						zs = zr * zr + zi * zi;
						if (zs < low) {
							low = zs;
						} else if (zs > param.breakoutValue) {
							CPU_Exterior_Coloring(fpX, fpColor, param.inverse_log2_power);
							break;
						}
					}
					if (zs <= param.breakoutValue) {
						CPU_Interior_Coloring(fpX, fpColor);
					}
					x++;
				}
				x -= param.sample;
				y++;
			}
			y -= param.sample;
			if (outA != 0.0) {
				outR = outR / outA;
				outG = outG / outA;
				outB = outB / outA;
				outA = outA / param.alphaDiv;
			}
			outR *= (fpColor)255.0;
			outG *= (fpColor)255.0;
			outB *= (fpColor)255.0;
			outA *= (fpColor)255.0;
			data[dataPtr] = (uint8_t)outR; dataPtr++;
			data[dataPtr] = (uint8_t)outG; dataPtr++;
			data[dataPtr] = (uint8_t)outB; dataPtr++;
			data[dataPtr] = (uint8_t)outA; dataPtr++;
			p0++;
		}
		x = 0;
	}
}

void renderCPU_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount) {
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderCPU_ABS_Mandelbrot()");
		return;
	}
	std::vector<std::thread> renderThread;
	/* Thread Creation */
		#define makeThread(k, fpX, fpColor) \
			renderThread.push_back(std::thread(k<fpX, fpColor>,\
				buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)\
			))
		
		#define generateThreads(k, fpX, fpColor) \
			{\
				PreCalc_Param<fpX, fpColor> preCalc_Param;\
				Generate_PreCalc_Param(\
					preCalc_Param,\
					buf, ren, param\
				);\
				for (uint32_t t = 0; t < threadCount; t++) {\
					size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;\
					size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;\
					makeThread(k, fpX, fpColor);\
				}\
			}

		#define makeThread_Alternate(k, fpX, fpColor) \
			renderThread.push_back(std::thread(k,\
				buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)\
			))

		#define generateThreads_Alternate(k, fpX, fpColor, spacing) \
			{\
				PreCalc_Param<fpX, fpColor> preCalc_Param;\
				Generate_PreCalc_Param(\
					preCalc_Param,\
					buf, ren, param\
				);\
				for (uint32_t t = 0; t < threadCount; t++) {\
					size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;\
					size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;\
					p0 = (p0 / spacing) * spacing;\
					p1 = (p1 / spacing) * spacing;\
					makeThread_Alternate(k, fpX, fpColor);\
				}\
			}
		
	/* Thread Creation */
	
	// Default is FP64
	if (param.polarMandelbrot == true) {
		switch(ren.CPU_Precision) {
			case 32: generateThreads(polarRender, fp32, fp32); break;
			#ifdef enableFP80andFP128
			case 80: generateThreads(polarRender, fp80, fp64); break;
			case 128: generateThreads(polarRender, fp128, fp64); break;
			#endif
			default:
			case 64: generateThreads(polarRender, fp128, fp64); break;
		};
	} else {
		switch(ren.CPU_Precision) {
			case 32: {
				switch(param.power) {
					#ifdef ENABLE_SSE2_RENDERING
						case 2: generateThreads_Alternate(quadraticRender_SSE2_FP32, fp32, fp32, 4); break;
						case 3: generateThreads_Alternate(    cubicRender_SSE2_FP32, fp32, fp32, 4); break;
						case 4: generateThreads_Alternate(  quarticRender_SSE2_FP32, fp32, fp32, 4); break;
						case 5: generateThreads_Alternate(  quinticRender_SSE2_FP32, fp32, fp32, 4); break;
						case 6: generateThreads_Alternate(   sexticRender_SSE2_FP32, fp32, fp32, 4); break;
					#else
						case 2: generateThreads          (quadraticRender          , fp32, fp32   ); break;
						case 3: generateThreads          (    cubicRender          , fp32, fp32   ); break;
						case 4: generateThreads          (  quarticRender          , fp32, fp32   ); break;
						case 5: generateThreads          (  quinticRender          , fp32, fp32   ); break;
						case 6: generateThreads          (   sexticRender          , fp32, fp32   ); break;
					#endif
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			default:
			case 64: {
				switch(param.power) {
					#ifdef ENABLE_SSE2_RENDERING
						case 2: generateThreads_Alternate(quadraticRender_SSE2_FP64, fp64, fp64, 2); break;
						case 3: generateThreads_Alternate(    cubicRender_SSE2_FP64, fp64, fp64, 2); break;
						case 4: generateThreads_Alternate(  quarticRender_SSE2_FP64, fp64, fp64, 2); break;
						case 5: generateThreads_Alternate(  quinticRender_SSE2_FP64, fp64, fp64, 2); break;
						case 6: generateThreads_Alternate(   sexticRender_SSE2_FP64, fp64, fp64, 2); break;
					#else
						case 2: generateThreads          (quadraticRender          , fp64, fp64   ); break;
						case 3: generateThreads          (    cubicRender          , fp64, fp64   ); break;
						case 4: generateThreads          (  quarticRender          , fp64, fp64   ); break;
						case 5: generateThreads          (  quinticRender          , fp64, fp64   ); break;
						case 6: generateThreads          (   sexticRender          , fp64, fp64   ); break;
					#endif
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#ifdef enableFP80andFP128
			case 80: {
				switch(param.power) {
						case 2: generateThreads(quadraticRender, fp80, fp80); break;
						case 3: generateThreads(    cubicRender, fp80, fp80); break;
						case 4: generateThreads(  quarticRender, fp80, fp80); break;
						case 5: generateThreads(  quinticRender, fp80, fp80); break;
						case 6: generateThreads(   sexticRender, fp80, fp80); break;
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#endif
			#ifdef enableFP80andFP128
			case 128: {
				switch(param.power) {
						case 2: generateThreads(quadraticRender, fp128, fp128); break;
						case 3: generateThreads(    cubicRender, fp128, fp128); break;
						case 4: generateThreads(  quarticRender, fp128, fp128); break;
						case 5: generateThreads(  quinticRender, fp128, fp128); break;
						case 6: generateThreads(   sexticRender, fp128, fp128); break;
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#endif
		}
	}

	for (uint32_t t = 0; t < threadCount; t++) {
		renderThread.at(t).join();
	}
	if (ABORT_RENDERING == true) {
		printFlush("\nAborted %" PRIu32 " threads", threadCount);
	}
}