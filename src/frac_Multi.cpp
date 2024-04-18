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

	#ifdef MONOCHROME_MODE
		#define CPU_Interior_Coloring(fpX, fpColor); \
			outR += param.interior_B_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_B_Freq + param.interior_B_Phase));\
			outG += param.interior_B_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_B_Freq + param.interior_B_Phase));\
			outB += param.interior_B_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_B_Freq + param.interior_B_Phase));\
			outA += param.interior_Alpha;

		#define CPU_Exterior_Coloring(fpX, fpColor, inverse_log2_power); \
			fpColor smooth = (fpColor)log1p((fpColor)fmax((fpColor)0.0, (fpColor)itr - (fpColor)log2(log2(zs) / (fpX)2.0) * inverse_log2_power));\
			outR += param.exterior_R_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_R_Freq * smooth + param.exterior_R_Phase)));\
			outG += param.exterior_R_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_R_Freq * smooth + param.exterior_R_Phase)));\
			outB += param.exterior_R_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_R_Freq * smooth + param.exterior_R_Phase)));\
			outA += param.exterior_Alpha;
	#else
		#define CPU_Interior_Coloring(fpX, fpColor); \
			outR += param.interior_R_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_R_Freq + param.interior_R_Phase));\
			outG += param.interior_G_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_G_Freq + param.interior_G_Phase));\
			outB += param.interior_B_Amp * param.interior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos((fpColor)log((fpColor)low) * param.interior_B_Freq + param.interior_B_Phase));\
			outA += param.interior_Alpha;

		#define CPU_Exterior_Coloring(fpX, fpColor, inverse_log2_power); \
			fpColor smooth = (fpColor)log1p((fpColor)fmax((fpColor)0.0, (fpColor)itr - (fpColor)log2(log2(zs) / (fpX)2.0) * inverse_log2_power));\
			outR += param.exterior_R_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_R_Freq * smooth + param.exterior_R_Phase)));\
			outG += param.exterior_G_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_G_Freq * smooth + param.exterior_G_Phase)));\
			outB += param.exterior_B_Amp * param.exterior_Alpha * ((fpColor)0.5 - (fpColor)0.5 * (fpColor)cos(TAU * (param.exterior_B_Freq * smooth + param.exterior_B_Phase)));\
			outA += param.exterior_Alpha;
	#endif

#define Block_Init(fpX, fpColor);

#define Block_BeginLoop(fpX, fpColor); \
	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;\
	int32_t y = (int32_t)(p0 / (size_t)param.resX);\
	int32_t x = (int32_t)(p0 % (size_t)param.resX);\
	x *= param.sample;\
	y *= param.sample;\
	for (; y < param.resY; y += param.sample) {\
		for (; x < param.resX; x += param.sample) {\
			if (p0 == p1 || ABORT_RENDERING == true) {\
				return;\
			}\
			fpColor outR = (fpColor)0.0;\
			fpColor outG = (fpColor)0.0;\
			fpColor outB = (fpColor)0.0;\
			fpColor outA = (fpColor)0.0;\
			for (int32_t v = 0; v < param.sample; v++) {\
				for (int32_t u = 0; u < param.sample; u++) {\
					fpX xCord = (((fpX)x - param.numX) * param.recip_numZ);\
					fpX yCord = (((fpX)y - param.numY) * param.neg_recip_numW);\
					fpX cr = (!param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;\
					fpX ci = (!param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;\
					fpX zr = (param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.realCord) : param.realJulia;\
					fpX zi = (param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.imagCord) : param.imagJulia;\
					\
					fpX low = (fpX)4.0;\
					fpX temp_zr = (fpX)0.0;\
					fpX zs = (fpX)0.0;\
					for (uint32_t itr = 0; itr < param.maxItr; itr++) {

#define Block_EndLoop(fpX, fpColor, l); \
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
				outA = outA / param.sampleDiv;\
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

	Block_Init(fpX, fpColor);

		fpX zr1, zr2, zi1, zi2, s1, s2, s3;
		bool f[8];
		for (uint8_t q = 0; q < 8; q++) {
			f[q] = ((param.formula >> q) & 1) ? true : false;
		}
		
		s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;
		s2 = (f[1]) ? (fpX)-1.0 : (fpX)1.0;
		s3 = (f[2]) ? (fpX)-2.0 : (fpX)2.0;
		
	Block_BeginLoop(fpX, fpColor);

		zr1 = (f[3]) ? fabs(zr) : zr;
		zi1 = (f[4]) ? fabs(zi) : zi;
		zr2 = (f[5]) ? fabs(zr) : zr;
		zi2 = (f[6]) ? fabs(zi) : zi;
		temp_zr = (f[7]) ?
			(s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr) :
			(s1 *     ((zr1 * zr) - s2 * (zi1 * zi)) + cr);
		zi = (zr2 * zi2 * s3) + ci;
		zr = temp_zr;
	
	Block_EndLoop(fpX, fpColor, inverse_log2(2.0));
}
	
template<typename fpX, typename fpColor>
void cubicRender(FractalParameters(fpX, fpColor)) {
	
	Block_Init(fpX, fpColor);

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
	
	Block_BeginLoop(fpX, fpColor);

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
	
	Block_EndLoop(fpX, fpColor, inverse_log2(3.0));
}
	
template<typename fpX, typename fpColor>
void quarticRender(FractalParameters(fpX, fpColor)) {

	Block_Init(fpX, fpColor);

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
	
	Block_BeginLoop(fpX, fpColor);

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
	
	Block_EndLoop(fpX, fpColor, inverse_log2(4.0));
}
	
template<typename fpX, typename fpColor>
void quinticRender(FractalParameters(fpX, fpColor)) {

	Block_Init(fpX, fpColor);

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
	
	Block_BeginLoop(fpX, fpColor);

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
	
	Block_EndLoop(fpX, fpColor, inverse_log2(5.0));
}

template<typename fpX, typename fpColor>
void sexticRender(FractalParameters(fpX, fpColor)) {

	Block_Init(fpX, fpColor);

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
	
	Block_BeginLoop(fpX, fpColor);

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

	Block_EndLoop(fpX, fpColor, inverse_log2(6.0));
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
// void polynomialRender(FractalParameters) {
// 	uint8_t* data = buf->vram;
// 	uint32_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;
// 	uint32_t maxItr = param.maxItr;
// 	fpX r = (fpX)param.r;
// 	fpX i = (fpX)param.i;
// 	fpX zoom = (fpX)param.zoom;
// 	uint32_t y = p0 / resX;
// 	uint32_t x = p0 % resX;
// 	uint32_t sample = ren.sample;
// 	uint32_t power = param.power;
// 	fpX cr = (fpX)0.0;
// 	fpX ci = (fpX)0.0;
// 	fpX zr = (fpX)0.0;
// 	fpX zi = (fpX)0.0;
// 	resX *= sample;
// 	resY *= sample;
// 	x *= sample;
// 	y *= sample;

// 	const fp64 powerLog2 = 1.0 / log2((fp64)power);
// 	size_t sizeOuter = 4;
// 	size_t sizeSign = power + 1;
// 	size_t sizeAbs = power * 2;
// 	bool fOuter[sizeOuter];
// 	fpX* fSign = (fpX*)calloc(sizeSign, sizeof(fpX));
// 	bool* fAbs = (bool*)calloc(sizeAbs, sizeof(bool));
// 	{
// 		size_t sIndex = 0;
// 		for (size_t s = 0; s < sizeSign; s += 2) { /* Real Coefficients*/
// 			fSign[sIndex] = (fpX)nCr(power,s);
// 			fSign[sIndex] *= (param.formula & (0x1 << s)) ? (fpX)(-1.0) : (fpX)(1.0);
// 			sIndex++;
// 		}
// 		for (size_t s = 1; s < sizeSign; s += 2) { /* Imag Coefficients */
// 			fSign[sIndex] = (fpX)nCr(power,s);
// 			fSign[sIndex] *= (param.formula & (0x1 << s)) ? (fpX)(-1.0) : (fpX)(1.0);
// 			sIndex++;
// 		}
// 		for (size_t a = 0; a < sizeAbs; a++) {
// 			fAbs[a] = (param.formula & (0x1 << (sizeSign + 2 + a)));
// 		}
// 		fOuter[0] = (param.formula & (0x1 << (sizeSign))); /* Real Sign */
// 		fOuter[1] = (param.formula & (0x1 << (sizeSign + 1))); /* Imag Sign */
// 		fOuter[2] = (param.formula & (0x1 << (sizeSign + 2 + sizeAbs))); /* Real Abs */
// 		fOuter[3] = (param.formula & (0x1 << (sizeSign + 2 + sizeAbs + 1))); /* Imag Abs */
// 	}
// 	uint32_t realTerms = (power + 2) / 2;
// 	uint32_t imagTerms = (power + 1) / 2;
	
// 	for (; y < param.resY; y += sample) {
// 		for (; x < param.resX; x += sample) {
// 			if (p0 == p1 || ABORT_RENDERING == true) {
// 				FREE(fSign);
// 				FREE(fAbs);
// 				return;
// 			}
// 			fpColor outR = 0;
// 			fpColor outG = 0;
// 			fpColor outB = 0;
// 			fpColor outA = 0;
// 			for (uint32_t v = 0; v < sample; v++) {
// 				for (uint32_t u = 0; u < sample; u++) {
// 					if (param.juliaSet == true) {
// 						cpu_pixel_to_coordinate(x, y, &zr, &zi, &param, resX, resY, subSample);
// 						cr = (fpX)param.zr;
// 						ci = (fpX)param.zi;
// 					} else {
// 						cpu_pixel_to_coordinate(x, y, &cr, &ci, &param, resX, resY, subSample);
// 						zr = (param.startingZ == false) ? (fpX)0.0 : (fpX)param.zr;
// 						zi = (param.startingZ == false) ? (fpX)0.0 : (fpX)param.zi;
// 					}
// 					fpX low = (fpX)4.0;
// 					fpX temp_zr;
// 					fpX zs = (fpX)0.0;
// 					for (uint32_t itr = 0; itr < maxItr; itr++) {
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
// 						} /* sample * sample * 4 */
						
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
// 							CPU_Exterior_Coloring(fpX, fpColor, powerLog2);
// 							break;
// 						}
// 					}
// 					if (zs <= param.breakoutValue) {
// 						CPU_Interior_Coloring(fpX, fpColor);
// 					}
// 					x++;
// 				}
// 				x -= sample;
// 				y++;
// 			}
// 			y-= sample;
// 			if (outA != 0.0) {
// 				outR = outR / outA;
// 				outG = outG / outA;
// 				outB = outB / outA;
// 				outA = outA / param.sampleDiv;
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

template <typename fpX>
constexpr inline fpX polarAngle(fpX zr, fpX zi) {
	fpX angle;
	if (zi == (fpX)0.0) {
		return (fpX)0.0;
	}
	if (zr > (fpX)0.0) {
		angle = (fpX)atan(zi / zr);
	} else {
		if (zi > (fpX)0.0) {
			angle = (fpX)PI + (fpX)atan(zi / zr);
		} else {
			angle = (fpX)atan(zi / zr) - (fpX)PI;
		}
	}
	return angle;
}

template<typename fpX, typename fpColor>
void polarRender(FractalParameters(fpX, fpColor)) {
	size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS;
	int32_t y = (int32_t)(p0 / (size_t)param.resX);
	int32_t x = (int32_t)(p0 % (size_t)param.resX);
	x *= param.sample;
	y *= param.sample;
	const fpX power = param.polarPower;
	const fpX powerHalf = param.polarPowerHalf;
	for (; y < param.resY; y += param.sample) {
		for (; x < param.resX; x += param.sample) {
			if (p0 == p1 || ABORT_RENDERING == true) {
				return;
			}
			fp64 outR = 0.0;
			fp64 outG = 0.0;
			fp64 outB = 0.0;
			fp64 outA = 0.0;
			for (int32_t v = 0; v < param.sample; v++) {
				for (int32_t u = 0; u < param.sample; u++) {
					fpX xCord = (((fpX)x - param.numX) * param.recip_numZ);
					fpX yCord = (((fpX)y - param.numY) * param.neg_recip_numW);

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
				outA = outA / param.sampleDiv;
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

template <typename fpX, typename fpColor>
void Generate_PreCalc_Param(
	PreCalc_Param<fpX,fpColor>& preCalc_Param,
	const BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param
) {
	if (buf == nullptr) { return; }
	/* Header */
		preCalc_Param.formula = param.formula;
		preCalc_Param.power = param.power;
		preCalc_Param.resX = buf->resX * ren.sample;
		preCalc_Param.resY = buf->resY * ren.sample;
		preCalc_Param.sample = ren.sample;
		preCalc_Param.maxItr = param.maxItr;
		preCalc_Param.juliaSet = param.juliaSet;
		preCalc_Param.power = param.power;
	/* Coordinates */
		preCalc_Param.realCord = (fpX)param.r;
		preCalc_Param.imagCord = (fpX)param.i;
		preCalc_Param.realJulia = (fpX)param.zr;
		preCalc_Param.imagJulia = (fpX)param.zi;
		preCalc_Param.zoom_PC = (fpX)pow((fpCord)10.0, (fpCord)param.zoom);
		preCalc_Param.rotSin_PC = (fpX)sin((fpCord)param.rot);
		preCalc_Param.rotCos_PC = (fpX)cos((fpCord)param.rot);
		const dim32_t sResX = preCalc_Param.resX - 1;
		const dim32_t sResY = preCalc_Param.resY - 1;
		preCalc_Param.numY = ((fpX)sResY / (fpX)2.0);
		preCalc_Param.numX = ((fpX)sResX / (fpX)2.0);
		const fpX numT = (sResX >= sResY) ?
			(preCalc_Param.numY * preCalc_Param.zoom_PC) :
			(preCalc_Param.numX * preCalc_Param.zoom_PC);
		preCalc_Param.recip_numZ = ((fpX)param.sX / numT);
		preCalc_Param.neg_recip_numW = -((fpX)param.sY / numT);
		preCalc_Param.breakoutValue = (fpX)param.breakoutValue;
	/* Polar */
		preCalc_Param.polarPower = (fpX)param.polarPower;
		preCalc_Param.polarPowerHalf = (fpX)param.polarPower / (fpX)2.0;
	/* Coloring */
		preCalc_Param.inverse_log2_power = (fpColor)1.0 / log2((fpColor)param.polarPower);
		preCalc_Param.sampleDiv = (fpColor)(ren.sample * ren.sample);

		preCalc_Param.exterior_Alpha = (fpColor)param.exterior_Alpha;
			preCalc_Param.exterior_R_Amp   = (fpColor)param.exterior_R_Amp  ;
			preCalc_Param.exterior_R_Freq  = (fpColor)param.exterior_R_Freq ;
			preCalc_Param.exterior_R_Phase = (fpColor)param.exterior_R_Phase;
			preCalc_Param.exterior_G_Amp   = (fpColor)param.exterior_G_Amp  ;
			preCalc_Param.exterior_G_Freq  = (fpColor)param.exterior_G_Freq ;
			preCalc_Param.exterior_G_Phase = (fpColor)param.exterior_G_Phase;
			preCalc_Param.exterior_B_Amp   = (fpColor)param.exterior_B_Amp  ;
			preCalc_Param.exterior_B_Freq  = (fpColor)param.exterior_B_Freq ;
			preCalc_Param.exterior_B_Phase = (fpColor)param.exterior_B_Phase;
		preCalc_Param.interior_Alpha = (fpColor)param.interior_Alpha;
			preCalc_Param.interior_R_Amp   = (fpColor)param.interior_R_Amp  ;
			preCalc_Param.interior_R_Freq  = (fpColor)param.interior_R_Freq ;
			preCalc_Param.interior_R_Phase = (fpColor)param.interior_R_Phase;
			preCalc_Param.interior_G_Amp   = (fpColor)param.interior_G_Amp  ;
			preCalc_Param.interior_G_Freq  = (fpColor)param.interior_G_Freq ;
			preCalc_Param.interior_G_Phase = (fpColor)param.interior_G_Phase;
			preCalc_Param.interior_B_Amp   = (fpColor)param.interior_B_Amp  ;
			preCalc_Param.interior_B_Freq  = (fpColor)param.interior_B_Freq ;
			preCalc_Param.interior_B_Phase = (fpColor)param.interior_B_Phase;
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
		switch(param.power) {
			case 2:
				switch(ren.CPU_Precision) {
					case 32:
						if (ren.sample == 1) {
							generateThreads_Alternate(quadraticRender_SSE2_FP32, fp32, fp32, 4);
						} else {
							generateThreads(quadraticRender, fp32, fp32);
						}
					break;
					#ifdef enableFP80andFP128
					case 80: generateThreads(quadraticRender, fp80, fp64); break;
					case 128: generateThreads(quadraticRender, fp128, fp64); break;
					#endif
					default:
					case 64: generateThreads(quadraticRender, fp64, fp64); break;
				};
			break;
			case 3:
				switch(ren.CPU_Precision) {
					case 32: generateThreads(cubicRender, fp32, fp32); break;
					#ifdef enableFP80andFP128
					case 80: generateThreads(cubicRender, fp80, fp64); break;
					case 128: generateThreads(cubicRender, fp128, fp64); break;
					#endif
					default:
					case 64: generateThreads(cubicRender, fp64, fp64); break;
				};
			break;
			case 4:
				switch(ren.CPU_Precision) {
					case 32: generateThreads(quarticRender, fp32, fp32); break;
					#ifdef enableFP80andFP128
					case 80: generateThreads(quarticRender, fp80, fp64); break;
					case 128: generateThreads(quarticRender, fp128, fp64); break;
					#endif
					default:
					case 64: generateThreads(quinticRender, fp64, fp64); break;
				};
			break;
			case 5:
				switch(ren.CPU_Precision) {
					case 32: generateThreads(quinticRender, fp32, fp32); break;
					#ifdef enableFP80andFP128
					case 80: generateThreads(quinticRender, fp80, fp64); break;
					case 128: generateThreads(quinticRender, fp128, fp64); break;
					#endif
					default:
					case 64: generateThreads(quinticRender, fp64, fp64); break;
				};
			break;
			case 6:
				switch(ren.CPU_Precision) {
					case 32: generateThreads(sexticRender, fp32, fp32); break;
					#ifdef enableFP80andFP128
					case 80: generateThreads(sexticRender, fp80, fp64); break;
					case 128: generateThreads(sexticRender, fp128, fp64); break;
					#endif
					default:
					case 64: generateThreads(sexticRender, fp64, fp64); break;
				};
			break;
			default:
				printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
				return;
		};
	}

	for (uint32_t t = 0; t < threadCount; t++) {
		renderThread.at(t).join();
	}
	if (ABORT_RENDERING == true) {
		printFlush("\nAborted %" PRIu32 " threads", threadCount);
	}
}