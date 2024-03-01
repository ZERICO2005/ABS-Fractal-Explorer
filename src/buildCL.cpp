/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#include "Common_Def.h"
#include "Program_Def.h"

#include "buildCL.h"

// Code below is version 1.0.2
const char* FractalOpenCL_SRC = "\
\n\
/* Type Definitions */\n\
	typedef uchar	uint8_t;\n\
	typedef ushort	uint16_t;\n\
	typedef uint	uint32_t;\n\
	typedef ulong	uint64_t;\n\
	typedef char	int8_t;\n\
	typedef short	int16_t;\n\
	typedef int		int32_t;\n\
	typedef long	int64_t;\n\
\n\
	typedef uchar	u8;\n\
	typedef ushort	u16;\n\
	typedef uint	u32;\n\
	typedef ulong	u64;\n\
	typedef char	i8;\n\
	typedef short	i16;\n\
	typedef int		i32;\n\
	typedef long	i64;\n\
\n\
	typedef half	fp16;\n\
	typedef float	fp32;\n\
	typedef double	fp64;\n\
/* Type Definitions */\n\
\n\
/* Constants */\n\
	#define PI		3.141592653589793f\n\
	#define TAU		6.283185307179586f\n\
	#define EULER	2.718281828459045f\n\
	#define BREAKOUT 4096.0f\n\
	#define IMAGE_BUFFER_CHANNELS 4\n\
/* Constants */\n\
\n\
 __kernel void renderFracCLPoint(\n\
			fp32 r, fp32 i,\n\
			u32 maxItr,\n\
			u32 resX, u32 resY,\n\
			fp32 zr0, fp32 zi0,\n\
			uint32_t formula, fp32 power, uint32_t sample,\n\
			fp32 rot,\n\
			fp32 numZ,fp32 numW,\n\
			__global uint8_t* resultBuf\n\
) { // Some values like zoom are embeded into precalculated constants\n\
    u32 id = get_global_id(0);\n\
	uint32_t outR = 0;\n\
	uint32_t outG = 0;\n\
	uint32_t outB = 0;\n\
	uint32_t outA = 0;\n\
	fp32 smooth = 0.0;\n\
	u8 type = (formula & 0x40000000) ? 1 : (uint8_t)power;\n\
	fp32 y = (fp32)(id / resX);\n\
	fp32 x = (fp32)(id % resX);\n\
	\n\
	//fp32 cr0,ci0,zrc,zic;\n\
\n\
	resX = (resX * sample) - 1;\n\
	resY = (resY * sample) - 1;\n\
	x *= sample;\n\
	y *= sample;\n\
	fp32 numY = ((fp32)resY / 2.0f);\n\
	fp32 numX = ((fp32)resX / 2.0f);\n\
	fp32 rSin = sin(rot);\n\
	fp32 rCos = cos(rot);\n\
	x -= numX;\n\
	y -= numY;\n\
\n\
	for (u32 v = 0; v < sample; v++) {\n\
		for (u32 u = 0; u < sample; u++) {\n\
			if (id >= resX * resY) {\n\
				return;\n\
			}\n\
			fp32 cr,ci,zr,zi;\n\
			fp32 low = 4.0f; // Squared\n\
			fp32 temp = 0.0f;\n\
			fp32 zs = 0.0f;\n\
\n\
			fp32 xC = (x / numZ);\n\
    		fp32 yC = -(y / numW);\n\
			if (formula & 0x20000000) { // Julia Set // Optimized Coordinate Formula\n\
				zr = (xC * rCos - yC * rSin) + r;\n\
				zi = (yC * rCos + xC * rSin) + i;\n\
				cr = zr0;\n\
				ci = zi0;\n\
			} else {\n\
				cr = (xC * rCos - yC * rSin) + r;\n\
				ci = (yC * rCos + xC * rSin) + i;\n\
				zr = zr0; // Default 0.0f\n\
				zi = zi0; // Default 0.0f\n\
			}\n\
			//formula &= 0x1FFFFFFF;\n\
			if (type == 1) {\n\
				zs = (zr * zr + zi * zi); // Otherwise Julia Sets don't work\n\
				fp32 za = 0.0f;\n\
				fp32 powerHalf = power / 2.0f;\n\
				for (uint32_t itr = 0; itr < maxItr; itr++) {\n\
					za = atan2(zi, zr) * power;\n\
					zr = pow(zs, powerHalf) * cos(za) + cr;\n\
					zi = pow(zs, powerHalf) * sin(za) + ci;\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(power)));\n\
						break;\n\
					}\n\
				}\n\
			} else if (type == 2) {\n\
				fp32 zr1, zr2, zi1, zi2, s1, s2, s3;\n\
				uint8_t f[8];\n\
				for (uint8_t q = 0; q < 8; q++) {\n\
					f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				s1 = (f[0]) ? -1.0f : 1.0f;\n\
				s2 = (f[1]) ? -1.0f : 1.0f;\n\
				s3 = (f[2]) ? -2.0f : 2.0f;\n\
				for (u32 itr = 0; itr < maxItr; itr++) {\n\
					zr1 = (f[3]) ? fabs(zr) : zr;\n\
					zi1 = (f[4]) ? fabs(zi) : zi;\n\
					zr2 = (f[5]) ? fabs(zr) : zr;\n\
					zi2 = (f[6]) ? fabs(zi) : zi;\n\
					if (f[7] == 0) {\n\
						temp = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;\n\
						zi = (zr2 * zi2 * s3) + ci;\n\
						zr = temp;\n\
					} else {\n\
						temp = s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr;\n\
						zi = (zr2 * zi2 * s3) + ci;\n\
						zr = temp;\n\
					}\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(2.0f)));\n\
						break;\n\
					}\n\
				}\n\
			} else if (type == 3) {\n\
				fp32 zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;\n\
				uint8_t f[14];\n\
				for (uint8_t q = 0; q < 14; q++) {\n\
					f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				s1 = (f[0]) ? -1.0f: 1.0f;\n\
				s2 = (f[1]) ? -3.0f: 3.0f;\n\
				s3 = (f[2]) ? -3.0f: 3.0f;\n\
				s4 = (f[3]) ? -1.0f: 1.0f;\n\
				s5 = (f[4]) ? -1.0f: 1.0f;\n\
				s6 = (f[5]) ? -1.0f: 1.0f;\n\
				for (u32 itr = 0; itr < maxItr; itr++) {\n\
					zr1 = (f[6]) ? fabs(zr) : zr;\n\
					zi1 = (f[7]) ? fabs(zi) : zi;\n\
					zr2 = (f[8]) ? fabs(zr) : zr;\n\
					zi2 = (f[9]) ? fabs(zi) : zi;\n\
					zr3 = (f[10]) ? fabs(zr) : zr;\n\
					zi3 = (f[11]) ? fabs(zi) : zi;\n\
					if (f[12] == 0) {\n\
						if (f[13] == 0) {\n\
						temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
						zr = temp;\n\
						} else {\n\
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
						zr = temp;\n\
						}\n\
					} else {\n\
						if (f[13] == 0) {\n\
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
						zr = temp;\n\
						} else {\n\
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
						zi = s6 * fabs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
						zr = temp;\n\
						}\n\
					}\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(3.0f)));\n\
						break;\n\
					}\n\
				}\n\
			} else if (type == 4) {\n\
				fp32 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;\n\
				uint8_t f[17];\n\
				for (uint8_t q = 0; q < 17; q++) {\n\
					f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				s1 = (f[0]) ? -1.0f: 1.0f;\n\
				s2 = (f[1]) ? -6.0f: 6.0f;\n\
				s3 = (f[2]) ? -1.0f: 1.0f;\n\
				s4 = (f[3]) ? -4.0f: 4.0f;\n\
				s5 = (f[4]) ? -4.0f: 4.0f;\n\
				s6 = (f[5]) ? -1.0f: 1.0f;\n\
				s7 = (f[6]) ? -1.0f: 1.0f;\n\
				for (u32 itr = 0; itr < maxItr; itr++) {\n\
					zr1 = (f[7]) ? fabs(zr) : zr;\n\
					zi1 = (f[8]) ? fabs(zi) : zi;\n\
					zr2 = (f[9]) ? fabs(zr) : zr;\n\
					zi2 = (f[10]) ? fabs(zi) : zi;\n\
					zr3 = (f[11]) ? fabs(zr) : zr;\n\
					zi3 = (f[12]) ? fabs(zi) : zi;\n\
					zr4 = (f[13]) ? fabs(zr) : zr;\n\
					zi4 = (f[14]) ? fabs(zi) : zi;\n\
\n\
					if (f[15] == 0) {\n\
						if (f[16] == 0) {\n\
							temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
							zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
							zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					} else {\n\
						if (f[16] == 0) {\n\
							temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
							zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
							zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					}\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(4.0f)));\n\
						break;\n\
					}\n\
				}\n\
			} else if (type == 5) {\n\
				fp32 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;\n\
				uint8_t fS[6];\n\
				uint8_t fA[10];\n\
				uint8_t fO[4];\n\
				for (uint8_t q = 0; q < 6; q++) { //0-5\n\
					fS[q] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 6; q < 8; q++) { //6-7\n\
					fO[q - 6] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 8; q < 18; q++) { //8-17\n\
					fA[q - 8] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 18; q < 20; q++) { //18-19\n\
					fO[q - 16] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				s1 = (fS[0]) ? -1.0f: 1.0f;\n\
				s2 = (fS[1]) ? -10.0f: 10.0f;\n\
				s3 = (fS[2]) ? -5.0f: 5.0f;\n\
				s4 = (fS[3]) ? -5.0f: 5.0f;\n\
				s5 = (fS[4]) ? -10.0f: 10.0f;\n\
				s6 = (fS[5]) ? -1.0f: 1.0f;\n\
				s7 = (fO[0]) ? -1.0f: 1.0f;\n\
				s8 = (fO[1]) ? -1.0f: 1.0f;\n\
				for (u32 itr = 0; itr < maxItr; itr++) {\n\
					zr1 = (fA[0]) ? fabs(zr) : zr;\n\
					zi1 = (fA[1]) ? fabs(zi) : zi;\n\
					zr2 = (fA[2]) ? fabs(zr) : zr;\n\
					zi2 = (fA[3]) ? fabs(zi) : zi;\n\
					zr3 = (fA[4]) ? fabs(zr) : zr;\n\
					zi3 = (fA[5]) ? fabs(zi) : zi;\n\
					zr4 = (fA[6]) ? fabs(zr) : zr;\n\
					zi4 = (fA[7]) ? fabs(zi) : zi;\n\
					zr5 = (fA[8]) ? fabs(zr) : zr;\n\
					zi5 = (fA[9]) ? fabs(zi) : zi;\n\
\n\
					if (fO[2] == 0) {\n\
						if (fO[3] == 0) {\n\
							temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
							zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
							zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					} else {\n\
						if (fO[3] == 0) {\n\
							temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
							zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
							zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					}\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(5.0f)));\n\
						break;\n\
					}\n\
				}\n\
			} else if (type == 6) {\n\
				fp32 zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6, s1, s2, s3, s4, s5, s6, s7, s8, s9;\n\
				uint8_t fS[7];\n\
				uint8_t fA[12];\n\
				uint8_t fO[4];\n\
				for (uint8_t q = 0; q <= 6; q++) { /* 0-6 */ \n\
					fS[q] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 7; q <= 8; q++) { /* 7-8 */ \n\
					fO[q - 7] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 9; q <= 20; q++) { /* 9-20 */ \n\
					fA[q - 9] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				for (uint8_t q = 21; q <= 22; q++) { /* 21-22 */ \n\
					fO[q - 19] = ((formula >> q) & 1) ? 1 : 0;\n\
				}\n\
				s1 = (fS[0]) ? -1.0f : 1.0f;\n\
				s2 = (fS[1]) ? -15.0f : 15.0f;\n\
				s3 = (fS[2]) ? -15.0f : 15.0f;\n\
				s4 = (fS[3]) ? -1.0f : 1.0f;\n\
				s5 = (fS[4]) ? -6.0f : 6.0f;\n\
				s6 = (fS[5]) ? -20.0f : 20.0f;\n\
				s7 = (fS[6]) ? -6.0f : 6.0f;\n\
				s8 = (fO[0]) ? -1.0f : 1.0f;\n\
				s9 = (fO[1]) ? -1.0f : 1.0f;\n\
				for (u32 itr = 0; itr < maxItr; itr++) {\n\
					zr1 = (fA[0]) ? fabs(zr) : zr;\n\
					zi1 = (fA[1]) ? fabs(zi) : zi;\n\
					zr2 = (fA[2]) ? fabs(zr) : zr;\n\
					zi2 = (fA[3]) ? fabs(zi) : zi;\n\
					zr3 = (fA[4]) ? fabs(zr) : zr;\n\
					zi3 = (fA[5]) ? fabs(zi) : zi;\n\
					zr4 = (fA[6]) ? fabs(zr) : zr;\n\
					zi4 = (fA[7]) ? fabs(zi) : zi;\n\
					zr5 = (fA[8]) ? fabs(zr) : zr;\n\
					zi5 = (fA[9]) ? fabs(zi) : zi;\n\
					zr6 = (fA[10]) ? fabs(zr) : zr;\n\
					zi6 = (fA[11]) ? fabs(zi) : zi;\n\
\n\
					if (fO[2] == 0) {\n\
						if (fO[3] == 0) {\n\
							temp = s8 * (s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;\n\
							zi = s9 * (s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s8 * (s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;\n\
							zi = s9 * fabs(s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					} else {\n\
						if (fO[3] == 0) {\n\
							temp = s8 * fabs(s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;\n\
							zi = s9 * (s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						} else {\n\
							temp = s8 * fabs(s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;\n\
							zi = s9 * fabs(s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;\n\
							zr = temp;\n\
						}\n\
					}\n\
					zs = zr * zr + zi * zi;\n\
					if (zs < low) {\n\
						low = zs;\n\
					} else if (zs > BREAKOUT) {\n\
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(6.0f)));\n\
						break;\n\
					}\n\
				}\n\
			}\n\
			\n\
			if (zs > BREAKOUT) {\n\
				outR += (uint32_t)(0.9f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.5f))));\n\
				outG += (uint32_t)(1.0f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.9f))));\n\
				outB += (uint32_t)(1.0f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));\n\
				outA += 0xFF;\n\
			} else {\n\
				outR += 0;\n\
				outG += 0;\n\
				//outR += (uint16_t)(511.5f - 511.5f * cos(log(low) / 2.0f));\n\
				//outG += (uint16_t)(511.5f - 511.5f * cos(log(low) / 2.0f));\n\
				outB += (uint16_t)(511.5f - 511.5f * cos(log(low) / 2.0f));\n\
				outA += 0xFF;\n\
			}\n\
			x++;\n\
		}\n\
		x -= sample;\n\
		y++;\n\
	}\n\
	uint32_t div = sample * sample * 4;\n\
	outR /= div;\n\
	outG /= div;\n\
	outB /= div;\n\
	outA /= sample * sample;\n\
	//uint32_t outAvr = (outR + outG + outB) / 3; outR = outAvr; outG = outAvr; outB = outAvr; /* Grey-scale */\n\
	//uint32_t outAvr = (outR + outG + outB) / 3; outR = (outAvr + outR) / 2; outG = (outAvr + outG) / 2; outB = (outAvr + outB) / 2; /* Low-saturation */\n\
	id *= IMAGE_BUFFER_CHANNELS;\n\
	resultBuf[id] = (uint8_t)outR; id++;\n\
	resultBuf[id] = (uint8_t)outG; id++;\n\
	resultBuf[id] = (uint8_t)outB; id++;\n\
	resultBuf[id] = (uint8_t)outA;\n\
}\n\
";

const char* getOpenCLErrorString(cl_int errorCode) {
switch(errorCode){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

cl_device_id create_device() {
	cl_platform_id platform;
	cl_device_id dev;
	int32_t err;

	err = clGetPlatformIDs(1, &platform, NULL);
	if(err < 0) {
		//perror("Unable to identify platform");
		throw "Unable to identify platform";
	} 

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL); // GPU
	if(err == CL_DEVICE_NOT_FOUND) { 
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL); // CPU
	}
	if(err < 0) {
		//perror("Unable to access devices");
		throw "Unable to access devices";
	}
	return dev;
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {
	
	#ifdef BUILD_FRAC_FROM_FILE
		cl_program program;
		FILE *program_handle;
		char *program_buffer, *program_log;
		size_t program_size, log_size;
		int32_t err;

		program_handle = fopen(filename, "r");
		if(program_handle == NULL) {
			//perror("Unable to find the program file");
			throw "Unable to find the program file";
		}
		fseek(program_handle, 0, SEEK_END);
		program_size = ftell(program_handle);
		rewind(program_handle);
		program_buffer = (char*)malloc(program_size + 1);
		program_buffer[program_size] = '\0';
		fread(program_buffer, sizeof(char), program_size, program_handle);
		fclose(program_handle);


		program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, &program_size, &err);
		if(err < 0) {
			//perror("Unable to create program");
			throw "Unable to create program";
		}
		free(program_buffer);

		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if(err < 0) { //Prints error log
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			program_log = (char*) malloc(log_size + 1);
			program_log[log_size] = '\0';
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
			printf("%s\n", program_log);
			free(program_log);
			throw "Error occured when building";
		}
	#endif
	#ifdef BUILD_FRAC_FROM_CONST
		cl_program program;
		FILE *program_handle;
		char *program_log;
		size_t program_size, log_size;
		int err;

		program_size = strlen(FractalOpenCL_SRC);
		program = clCreateProgramWithSource(ctx, 1, (const char**)&FractalOpenCL_SRC, &program_size, &err);
		if(err < 0) {
			//perror("Unable to create program");
			throw "Unable to create program";
		}
		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if(err < 0) { //Prints error log
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			program_log = (char*) malloc(log_size + 1);
			program_log[log_size] = '\0';
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
			printf("%s\n", program_log);
			free(program_log);
			throw "Error occured when building";
		}
	#endif
	return program;
}