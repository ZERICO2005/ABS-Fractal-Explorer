/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
/* Type Definitions */
	typedef uchar	uint8_t;
	typedef ushort	uint16_t;
	typedef uint	uint32_t;
	typedef ulong	uint64_t;
	typedef char	int8_t;
	typedef short	int16_t;
	typedef int		int32_t;
	typedef long	int64_t;

	typedef uchar	u8;
	typedef ushort	u16;
	typedef uint	u32;
	typedef ulong	u64;
	typedef char	i8;
	typedef short	i16;
	typedef int		i32;
	typedef long	i64;

	typedef half	fp16;
	typedef float	fp32;
	typedef double	fp64;
/* Type Definitions */

/* Constants */
	#define PI		3.141592653589793f
	#define TAU		6.283185307179586f
	#define EULER	2.718281828459045f
	#define BREAKOUT 4096.0f
/* Constants */

 __kernel void renderFracCLPoint(
			fp32 r, fp32 i,
			u32 maxItr,
			u32 resX, u32 resY,
			fp32 zr0, fp32 zi0,
			uint32_t formula, fp32 power, uint32_t sample,
			fp32 rot,
			fp32 numZ,fp32 numW,
			__global uint8_t* resultBuf
) { // Some values like zoom are embeded into precalculated constants
    u32 id = get_global_id(0);
	uint32_t outR = 0;
	uint32_t outG = 0;
	uint32_t outB = 0;
	fp32 smooth = 0.0;
	u8 type = (formula & 0x40000000) ? 1 : (uint8_t)power;
	fp32 y = (fp32)(id / resX);
	fp32 x = (fp32)(id % resX);
	
	//fp32 cr0,ci0,zrc,zic;

	resX = (resX * sample) - 1;
	resY = (resY * sample) - 1;
	x *= sample;
	y *= sample;
	fp32 numY = ((fp32)resY / 2.0f);
	fp32 numX = ((fp32)resX / 2.0f);
	const fp32 rSin = sin(rot);
	const fp32 rCos = cos(rot);
	x -= numX;
	y -= numY;

	for (u32 v = 0; v < sample; v++) {
		for (u32 u = 0; u < sample; u++) {
			fp32 cr,ci,zr,zi;
			fp32 low = 4.0f; // Squared
			fp32 temp = 0.0f;
			fp32 zs = 0.0f;

			fp32 xC = (x / numZ);
    		fp32 yC = -(y / numW);
			if (formula & 0x20000000) { // Julia Set // Optimized Coordinate Formula
				zr = (xC * rCos - yC * rSin) + r;
				zi = (yC * rCos + xC * rSin) + i;
				cr = zr0;
				ci = zi0;
			} else {
				cr = (xC * rCos - yC * rSin) + r;
				ci = (yC * rCos + xC * rSin) + i;
				zr = zr0; // Default 0.0f
				zi = zi0; // Default 0.0f
			}
			//formula &= 0x1FFFFFFF;
			if (type == 1) {
				zs = (zr * zr + zi * zi); // Otherwise Julia Sets don't work
				fp32 za = 0.0f;
				const fp32 powerHalf = power / 2.0f;
				for (uint32_t itr = 0; itr < maxItr; itr++) {
					za = atan2(zi, zr) * power;
					zr = pow(zs, powerHalf) * cos(za) + cr;
					zi = pow(zs, powerHalf) * sin(za) + ci;
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(power)));
						break;
					}
				}
			} else if (type == 2) {
				fp32 zr1, zr2, zi1, zi2, s1, s2, s3;
				uint8_t f[8];
				for (uint8_t q = 0; q < 8; q++) {
					f[q] = ((formula >> q) & 1) ? 1 : 0;
				}
				s1 = (f[0]) ? -1.0f : 1.0f;
				s2 = (f[1]) ? -1.0f : 1.0f;
				s3 = (f[2]) ? -2.0f : 2.0f;
				for (u32 itr = 0; itr < maxItr; itr++) {
					zr1 = (f[3]) ? fabs(zr) : zr;
					zi1 = (f[4]) ? fabs(zi) : zi;
					zr2 = (f[5]) ? fabs(zr) : zr;
					zi2 = (f[6]) ? fabs(zi) : zi;
					if (f[7] == 0) {
						temp = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;
						zi = (zr2 * zi2 * s3) + ci;
						zr = temp;
					} else {
						temp = s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr;
						zi = (zr2 * zi2 * s3) + ci;
						zr = temp;
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(2.0f)));
						break;
					}
				}
			} else if (type == 3) {
				fp32 zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;
				uint8_t f[14];
				for (uint8_t q = 0; q < 14; q++) {
					f[q] = ((formula >> q) & 1) ? 1 : 0;
				}
				s1 = (f[0]) ? -1.0f: 1.0f;
				s2 = (f[1]) ? -3.0f: 3.0f;
				s3 = (f[2]) ? -3.0f: 3.0f;
				s4 = (f[3]) ? -1.0f: 1.0f;
				s5 = (f[4]) ? -1.0f: 1.0f;
				s6 = (f[5]) ? -1.0f: 1.0f;
				for (u32 itr = 0; itr < maxItr; itr++) {
					zr1 = (f[6]) ? fabs(zr) : zr;
					zi1 = (f[7]) ? fabs(zi) : zi;
					zr2 = (f[8]) ? fabs(zr) : zr;
					zi2 = (f[9]) ? fabs(zi) : zi;
					zr3 = (f[10]) ? fabs(zr) : zr;
					zi3 = (f[11]) ? fabs(zi) : zi;
					if (f[12] == 0) {
						if (f[13] == 0) {
						temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
						zr = temp;
						} else {
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
						zr = temp;
						}
					} else {
						if (f[13] == 0) {
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
						zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
						zr = temp;
						} else {
						temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
						zi = s6 * fabs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
						zr = temp;
						}
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(3.0f)));
						break;
					}
				}
			} else if (type == 4) {
				fp32 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;
				uint8_t f[17];
				for (uint8_t q = 0; q < 17; q++) {
					f[q] = ((formula >> q) & 1) ? 1 : 0;
				}
				s1 = (f[0]) ? -1.0f: 1.0f;
				s2 = (f[1]) ? -6.0f: 6.0f;
				s3 = (f[2]) ? -1.0f: 1.0f;
				s4 = (f[3]) ? -4.0f: 4.0f;
				s5 = (f[4]) ? -4.0f: 4.0f;
				s6 = (f[5]) ? -1.0f: 1.0f;
				s7 = (f[6]) ? -1.0f: 1.0f;
				for (u32 itr = 0; itr < maxItr; itr++) {
					zr1 = (f[7]) ? fabs(zr) : zr;
					zi1 = (f[8]) ? fabs(zi) : zi;
					zr2 = (f[9]) ? fabs(zr) : zr;
					zi2 = (f[10]) ? fabs(zi) : zi;
					zr3 = (f[11]) ? fabs(zr) : zr;
					zi3 = (f[12]) ? fabs(zi) : zi;
					zr4 = (f[13]) ? fabs(zr) : zr;
					zi4 = (f[14]) ? fabs(zi) : zi;

					if (f[15] == 0) {
						if (f[16] == 0) {
							temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
							zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
							zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
							zr = temp;
						}
					} else {
						if (f[16] == 0) {
							temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
							zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;
							zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;
							zr = temp;
						}
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(4.0f)));
						break;
					}
				}
			} else if (type == 5) {
				fp32 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;
				uint8_t fS[6];
				uint8_t fA[10];
				uint8_t fO[4];
				for (uint8_t q = 0; q < 6; q++) { //0-5
					fS[q] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 6; q < 8; q++) { //6-7
					fO[q - 6] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 8; q < 18; q++) { //8-17
					fA[q - 8] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 18; q < 20; q++) { //18-19
					fO[q - 16] = ((formula >> q) & 1) ? 1 : 0;
				}
				s1 = (fS[0]) ? -1.0f: 1.0f;
				s2 = (fS[1]) ? -10.0f: 10.0f;
				s3 = (fS[2]) ? -5.0f: 5.0f;
				s4 = (fS[3]) ? -5.0f: 5.0f;
				s5 = (fS[4]) ? -10.0f: 10.0f;
				s6 = (fS[5]) ? -1.0f: 1.0f;
				s7 = (fO[0]) ? -1.0f: 1.0f;
				s8 = (fO[1]) ? -1.0f: 1.0f;
				for (u32 itr = 0; itr < maxItr; itr++) {
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

					if (fO[2] == 0) {
						if (fO[3] == 0) {
							temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
							zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
							zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
							zr = temp;
						}
					} else {
						if (fO[3] == 0) {
							temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
							zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;
							zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;
							zr = temp;
						}
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(5.0f)));
						break;
					}
				}
			} else if (type == 6) {
				fp32 zr1, zr2, zr3, zr4, zr5, zr6, zi1, zi2, zi3, zi4, zi5, zi6, s1, s2, s3, s4, s5, s6, s7, s8, s9;
				uint8_t fS[7];
				uint8_t fA[12];
				uint8_t fO[4];
				for (uint8_t q = 0; q <= 6; q++) { /* 0-6 */ 
					fS[q] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 7; q <= 8; q++) { /* 7-8 */ 
					fO[q - 7] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 9; q <= 20; q++) { /* 9-20 */ 
					fA[q - 9] = ((formula >> q) & 1) ? 1 : 0;
				}
				for (uint8_t q = 21; q <= 22; q++) { /* 21-22 */ 
					fO[q - 19] = ((formula >> q) & 1) ? 1 : 0;
				}
				s1 = (fS[0]) ? -1.0f : 1.0f;
				s2 = (fS[1]) ? -15.0f : 15.0f;
				s3 = (fS[2]) ? -15.0f : 15.0f;
				s4 = (fS[3]) ? -1.0f : 1.0f;
				s5 = (fS[4]) ? -6.0f : 6.0f;
				s6 = (fS[5]) ? -20.0f : 20.0f;
				s7 = (fS[6]) ? -6.0f : 6.0f;
				s8 = (fO[0]) ? -1.0f : 1.0f;
				s9 = (fO[1]) ? -1.0f : 1.0f;
				for (u32 itr = 0; itr < maxItr; itr++) {
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

					if (fO[2] == 0) {
						if (fO[3] == 0) {
							temp = s8 * (s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;
							zi = s9 * (s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s8 * (s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;
							zi = s9 * fabs(s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;
							zr = temp;
						}
					} else {
						if (fO[3] == 0) {
							temp = s8 * fabs(s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;
							zi = s9 * (s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;
							zr = temp;
						} else {
							temp = s8 * fabs(s1 * (zr1 * zr * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zr * zi1 * zi) + s3 * (zr3 * zr * zi2 * zi * zi * zi) - s4 * (zi3 * zi * zi * zi * zi * zi)) + cr;
							zi = s9 * fabs(s5 * (zr4 * zr * zr * zr * zr * zi4) - s6 * (zr5 * zr * zr * zi5 * zi * zi) + s7 * (zr6 * zi6 * zi * zi * zi * zi)) + ci;
							zr = temp;
						}
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > BREAKOUT) {
						smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(6.0f)));
						break;
					}
				}
			}
			
			if (zs > BREAKOUT) {
				outR += (uint32_t)(0.9f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.5f))));
				outG += (uint32_t)(1.0f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.9f))));
				outB += (uint32_t)(1.0f * (511.5f - 511.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));
			} else {
				//outR += 0;
				//outG += 0;
				outB += (uint16_t)(511.5f - 511.5f * cos(log(low) / 2.0f));
			}
			x++;
		}
		x -= sample;
		y++;
	}
	uint32_t div = sample * sample * 4;
	outR /= div;
	outG /= div;
	outB /= div;
	//uint32_t outA = (outR + outG + outB) / 3; outR = outA; outG = outA; outB = outA; /* Grey-scale */
	//uint32_t outA = (outR + outG + outB) / 3; outR = (outA + outR) / 2; outG = (outA + outG) / 2; outB = (outA + outB) / 2; /* Low-saturation */
	id *= 3;
	resultBuf[id] = (uint8_t)outR; id++;
	resultBuf[id] = (uint8_t)outG; id++;
	resultBuf[id] = (uint8_t)outB;
}

/*
** DEPRECATED CODE BELOW
*/

/*
 __kernel void renderFracCLPoint(fp32 r, fp32 i, fp32 zoom, u32 maxItr, u32 resX, u32 resY, fp32 zr0, fp32 zi0, uint32_t formula, __global uint8_t* resultBuf) {
    u32 id = get_global_id(0);
	fp32 y = (fp32)(id / resX);
	fp32 x = (fp32)(id % resX);
	id *= 3;
	
	fp32 ci = -(((y - (((fp32)resY - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + i;
	fp32 cr = (((x - (((fp32)resX - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + r;
	fp32 zr = zr0; // Default 0.0f
	fp32 zi = zi0; // Default 0.0f
	fp32 low = 4.0f; // Squared
	fp32 temp;
	fp32 zs;
	
	for (u32 itr = 0; itr < maxItr; itr++) {

		temp = fabs(-1.0f * (zr * zr * zr * zr) - 6.0f * (zr * zr * fabs(zi) * zi) + (fabs(zi) * zi * zi * zi)) + cr;
		zi = -1.0f * fabs(4.0f * (zr * zr * zr * fabs(zi)) - 4.0f * (fabs(zr) * zi * zi * fabs(zi))) + ci;
		zr = temp;
		zs = (fp64)zr * zr + zi * zi;
		
		if (zs < low) {
			low = zs;
		} else if (zs > BREAKOUT) {
			fp32 smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(4.0f))); //log2(sqrt(zs)) / log2(base)
			resultBuf[id] = (uint8_t)floor(0.9f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.5f)))); id++;
			resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.9f)))); id++;
			resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));
			break;
		}

	}
	if (zs <= BREAKOUT) {
		resultBuf[id] = 0; id++;
		resultBuf[id] = 0; id++;
		resultBuf[id] = (uint8_t)floor(127.5f - 127.5f * cos(log(low) / 2.0f));
	}
}
*/