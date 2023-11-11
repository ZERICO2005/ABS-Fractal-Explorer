/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "fracSingle.h"

/* BOILERPLATE */

#define BREAKOUT 4096.0;

#define Block0 u32 dataPtr = 0;\
u32 maxItr = param->itr;\
fp64 typeC = (fp64)param->type;\
fp64 r = param->r;\
fp64 i = param->i;\
fp64 zoom = param->zoom;\
fp64 cr = 0.0;\
fp64 ci = 0.0;\
fp64 zr = 0.0;\
fp64 zi = 0.0;

#define Block1 for (u32 y = 0; y < resY; y++) {\
ci = (param->julia & 0x1) ? param->zi : (-((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + i);\
	for (u32 x = 0; x < resX; x++) {\
		if (param->julia & 0x1) {\
			zi = -((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + i;\
			zr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + r;\
			cr = param->zr;\
		} else {\
			cr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + r;\
			zr = (param->julia & 0x2) ? 0.0 : param->zr;\
			zi = (param->julia & 0x2) ? 0.0 : param->zi;\
		}\
		fp64 low = 4.0;\
		fp64 temp;\
		fp64 zs;\
		for (u32 itr = 0; itr < maxItr; itr++) {

#define Block2(l) zs = (zr * zr + zi * zi);\
			if (zs < low) {\
				low = zs;\
			} else if (zs > BREAKOUT) {\
				fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) / log2(l)));\
				data[dataPtr] = (uint8_t)floor(0.9 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++;\
				data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.9)))); dataPtr++;\
				data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.1)))); dataPtr++;\
				break;\
			}\
		}\
		if (zs <= BREAKOUT) {\
			data[dataPtr] = 0; dataPtr++;\
			data[dataPtr] = 0; dataPtr++;\
			data[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;\
		}\
	}\
}

/* BOILERPLATE */

void quadraticRender(frac* param, uint8_t* data, u32 resX, u32 resY) {
	Block0
	fp64 zr1, zr2, zi1, zi2, s1, s2, s3;
	uint8_t f[8];
	for (uint8_t q = 0; q < 8; q++) {
		f[q] = ((param->formula >> q) & 1) ? 1 : 0;
	}

	s1 = (f[0]) ? -1.0 : 1.0;
	s2 = (f[1]) ? -1.0 : 1.0;
	s3 = (f[2]) ? -2.0 : 2.0;

	Block1
	zr1 = (f[3]) ? abs(zr) : zr;
	zi1 = (f[4]) ? abs(zi) : zi;
	zr2 = (f[5]) ? abs(zr) : zr;
	zi2 = (f[6]) ? abs(zi) : zi;

	if (f[7] == 0) {
		temp = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;
		zi = (zr2 * zi2 * s3) + ci;
		zr = temp;
	} else {
		temp = s1 * abs((zr1 * zr) - s2 * (zi1 * zi)) + cr;
		zi = (zr2 * zi2 * s3) + ci;
		zr = temp;
	}

	Block2(2.0)
}

void cubicRender(frac* param, uint8_t* data, u32 resX, u32 resY) {
	Block0
	fp64 zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;
	uint8_t f[14];
	for (uint8_t q = 0; q < 14; q++) {
		f[q] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	s1 = (f[0]) ? -1.0 : 1.0;
	s2 = (f[1]) ? -3.0 : 3.0;
	s3 = (f[2]) ? -3.0 : 3.0;
	s4 = (f[3]) ? -1.0 : 1.0;
	s5 = (f[4]) ? -1.0 : 1.0;
	s6 = (f[5]) ? -1.0 : 1.0;

	Block1
	zr1 = (f[6]) ? abs(zr) : zr;
	zi1 = (f[7]) ? abs(zi) : zi;
	zr2 = (f[8]) ? abs(zr) : zr;
	zi2 = (f[9]) ? abs(zi) : zi;
	zr3 = (f[10]) ? abs(zr) : zr;
	zi3 = (f[11]) ? abs(zi) : zi;

	if (f[12] == 0) {
		if (f[13] == 0) {
		temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
		zr = temp;
		} else {
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
		zr = temp;
		}
	} else {
		if (f[13] == 0) {
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
		zr = temp;
		} else {
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
		zi = s6 * abs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
		zr = temp;
		}
	}

	Block2(3.0)
}

void quarticRender(frac* param, uint8_t* data, u32 resX, u32 resY) {
	Block0
	fp64 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;
	uint8_t f[17];
	for (uint8_t q = 0; q < 17; q++) {
		f[q] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	s1 = (f[0]) ? -1.0 : 1.0;
	s2 = (f[1]) ? -6.0 : 6.0;
	s3 = (f[2]) ? -1.0 : 1.0;
	s4 = (f[3]) ? -4.0 : 4.0;
	s5 = (f[4]) ? -4.0 : 4.0;
	s6 = (f[5]) ? -1.0 : 1.0;
	s7 = (f[6]) ? -1.0 : 1.0;

	Block1
	zr1 = (f[7]) ? abs(zr) : zr;
	zi1 = (f[8]) ? abs(zi) : zi;
	zr2 = (f[9]) ? abs(zr) : zr;
	zi2 = (f[10]) ? abs(zi) : zi;
	zr3 = (f[11]) ? abs(zr) : zr;
	zi3 = (f[12]) ? abs(zi) : zi;
	zr4 = (f[13]) ? abs(zr) : zr;
	zi4 = (f[14]) ? abs(zi) : zi;

	if (f[15] == 0) {
		if (f[16] == 0) {
		temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
		zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
		zr = temp;
		} else {
		temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
		zi = s7 * abs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
		zr = temp;
		}
	} else {
		if (f[16] == 0) {
		temp = s6 * abs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
		zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
		zr = temp;
		} else {
		temp = s6 * abs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
		zi = s7 * abs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
		zr = temp;
		}
	}

	Block2(4.0)
}

void quinticRender(frac* param, uint8_t* data, u32 resX, u32 resY) {
	Block0
	fp64 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;
	uint8_t fS[6];
	uint8_t fA[10];
	uint8_t fO[4];
	for (uint8_t q = 0; q < 6; q++) { //0-5
		fS[q] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	for (uint8_t q = 6; q < 8; q++) { //6-7
		fO[q - 6] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	for (uint8_t q = 8; q < 18; q++) { //8-17
		fA[q - 8] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	for (uint8_t q = 18; q < 20; q++) { //18-19
		fO[q - 16] = ((param->formula >> q) & 1) ? 1 : 0;
	}
	s1 = (fS[0]) ? -1.0 : 1.0;
	s2 = (fS[1]) ? -10.0 : 10.0;
	s3 = (fS[2]) ? -5.0 : 5.0;
	s4 = (fS[3]) ? -5.0 : 5.0;
	s5 = (fS[4]) ? -10.0 : 10.0;
	s6 = (fS[5]) ? -1.0 : 1.0;
	s7 = (fO[0]) ? -1.0 : 1.0;
	s8 = (fO[1]) ? -1.0 : 1.0;

	Block1
	zr1 = (fA[0]) ? abs(zr) : zr;
	zi1 = (fA[1]) ? abs(zi) : zi;
	zr2 = (fA[2]) ? abs(zr) : zr;
	zi2 = (fA[3]) ? abs(zi) : zi;
	zr3 = (fA[4]) ? abs(zr) : zr;
	zi3 = (fA[5]) ? abs(zi) : zi;
	zr4 = (fA[6]) ? abs(zr) : zr;
	zi4 = (fA[7]) ? abs(zi) : zi;
	zr5 = (fA[8]) ? abs(zr) : zr;
	zi5 = (fA[9]) ? abs(zi) : zi;

	if (fO[2] == 0) {
		if (fO[3] == 0) {
			temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
			zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
			zr = temp;
		} else {
			temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
			zi = s8 * abs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
			zr = temp;
		}
	} else {
		if (fO[3] == 0) {
			temp = s7 * abs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
			zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
			zr = temp;
		} else {
			temp = s7 * abs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
			zi = s8 * abs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
			zr = temp;
		}
	}

	Block2(5.0)
}

void renderFormula_SingleThread(frac* param, uint8_t* data, u32 resX, u32 resY) {
	if (data == NULL) {
		printf("\nError | Texture Pointer is NULL"); fflush(stdout);
		return;
	}
	if (param->type == 2) {
		quadraticRender(param,data,resX,resY);
	} else if (param->type == 3) {
		cubicRender(param,data,resX,resY);
	} else if (param->type == 4) {
		quarticRender(param,data,resX,resY);
	} else if (param->type == 5) {
		quinticRender(param,data,resX,resY);
	} else { //Custom
	/*
	u32 dataPtr = 0;
	u32 maxItr = param->itr;
	fp64 typeC = (fp64)param->type;
	
	fp64 r = param->r;
	fp64 i = param->i;
	fp64 zoom = param->zoom;
	
	fp64 cr = 0.0;
	fp64 ci = 0.0;
	//fp64 ratio = (fp64)resX/(fp64)resY;
	//fp64 jumpX = (2.0 / resY / pow(10, zoom));
	//fp64 jumpY = (2.0 / resY / pow(10, zoom));
	//fp64 startX = (-1.0 * ratio) / (pow(10, zoom)) + r;
	//fp64 startY = 1.0 / (pow(10, zoom)) + i;
		
	for (u32 y = 0; y < resY; y++) {
		ci = -((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + i;
		
		for (u32 x = 0; x < resX; x++) {
			cr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + r;
			
			fp64 zr = param->zr; // Default 0.0
			fp64 zi = param->zi; // Default 0.0
			fp64 low = 4.0; // Squared
			fp64 temp;
			fp64 zs;
			
			for (u32 itr = 0; itr < maxItr; itr++) {

				temp = zr * zr - abs(zi) * zi + cr;
				zi = zr * zi * 2 + ci;
				zr = temp;
				zs = zr * zr + zi * zi;
				
				if (zs < low) {
					low = zs;
				} else if (zs > BREAKOUT) {
					fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) / log2(typeC))); //log2(sqrt(zs)) / log2(base)
					data[dataPtr] = (uint8_t)floor(0.9 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++;
					data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.9)))); dataPtr++;
					data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.1)))); dataPtr++;
					break;
				}

			}
			if (zs <= BREAKOUT) {
				data[dataPtr] = 0; dataPtr++;
				data[dataPtr] = 0; dataPtr++;
				data[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;
			}
		}
	}
	*/
	}
}