/*
**	Author: zerico2005 (2023-2024)
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

	typedef half	fp16;
	typedef float	fp32;
	typedef double	fp64;
/* Type Definitions */

/* Constants */
	#define PI		3.141592653589793f
	#define TAU		6.283185307179586f
	#define EULER	2.718281828459045f
	#define BREAKOUT 4096.0f
	#define IMAGE_BUFFER_CHANNELS 4
/* Constants */

 __kernel void renderFracCLPoint(
			fp32 r, fp32 i,
			uint32_t maxItr,
			uint32_t resX, uint32_t resY,
			fp32 zr0, fp32 zi0,
			uint32_t formula, fp32 power, uint32_t sample,
			fp32 rSin,
			fp32 rCos,
			fp32 breakoutValue,
			fp32 recip_numZ, fp32 neg_recip_numW,
			__global uint8_t* resultBuf,
			fp32 Exterior_R_Freq_mult_TAU, fp32 Exterior_R_Phase_mult_TAU, fp32 Exterior_R_Amp_mult_Exterior_Alpha,
			fp32 Exterior_G_Freq_mult_TAU, fp32 Exterior_G_Phase_mult_TAU, fp32 Exterior_G_Amp_mult_Exterior_Alpha,
			fp32 Exterior_B_Freq_mult_TAU, fp32 Exterior_B_Phase_mult_TAU, fp32 Exterior_B_Amp_mult_Exterior_Alpha,
			fp32 Exterior_Alpha,
			fp32 Interior_R_Freq, fp32 Interior_R_Phase, fp32 Interior_R_Amp_mult_Interior_Alpha,
			fp32 Interior_G_Freq, fp32 Interior_G_Phase, fp32 Interior_G_Amp_mult_Interior_Alpha,
			fp32 Interior_B_Freq, fp32 Interior_B_Phase, fp32 Interior_B_Amp_mult_Interior_Alpha,
			fp32 Interior_Alpha
) { // Some values like zoom are embeded into precalculated constants
	uint32_t id = get_global_id(0);
	fp32 outR = 0.0f;
	fp32 outG = 0.0f;
	fp32 outB = 0.0f;
	fp32 outA = 0.0f;
	fp32 smooth = 0.0f;
	uint8_t type = (formula & 0x40000000) ? 1 : (uint8_t)power;
	fp32 y = (fp32)(id / resX);
	fp32 x = (fp32)(id % resX);
	
	//fp32 cr0,ci0,zrc,zic;

	resX = (resX * sample) - 1;
	resY = (resY * sample) - 1;
	x *= sample;
	y *= sample;
	const fp32 numY = ((fp32)resY / 2.0f);
	const fp32 numX = ((fp32)resX / 2.0f);
	x -= numX;
	y -= numY;

	for (uint32_t v = 0; v < sample; v++) {
		fp32 yC = y * neg_recip_numW;
		for (uint32_t u = 0; u < sample; u++) {
			if (id >= resX * resY) {
				return;
			}
			fp32 cr,ci,zr,zi;
			fp32 low = 4.0f; // Squared
			fp32 temp = 0.0f;
			fp32 zs = 0.0f;

			fp32 xC = x * recip_numZ;
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
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(power)));
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
				for (uint32_t itr = 0; itr < maxItr; itr++) {
					zr1 = (f[3]) ? fabs(zr) : zr;
					zi1 = (f[4]) ? fabs(zi) : zi;
					zr2 = (f[5]) ? fabs(zr) : zr;
					zi2 = (f[6]) ? fabs(zi) : zi;
					
					if (f[7] == 0) {
						zr = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;
						zi = (zr2 * zi2 * s3) + ci;
					} else {
						zr = s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr;
						zi = (zr2 * zi2 * s3) + ci;
					}
					zs = zr * zr + zi * zi;
					if (zs < low) {
						low = zs;
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(2.0f)));
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
				for (uint32_t itr = 0; itr < maxItr; itr++) {
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
							temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;
							zi = s6 * fabs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;
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
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(3.0f)));
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
				for (uint32_t itr = 0; itr < maxItr; itr++) {
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
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(4.0f)));
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
				for (uint32_t itr = 0; itr < maxItr; itr++) {
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
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(5.0f)));
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
				for (uint32_t itr = 0; itr < maxItr; itr++) {
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
					} else if (zs > breakoutValue) {
						smooth = log1p(fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(6.0f)));
						break;
					}
				}
			}
			
			if (zs > breakoutValue) {
				outR += Exterior_R_Amp_mult_Exterior_Alpha * (0.5f - 0.5f * cos(Exterior_R_Freq_mult_TAU * smooth + Exterior_R_Phase_mult_TAU));
				outG += Exterior_G_Amp_mult_Exterior_Alpha * (0.5f - 0.5f * cos(Exterior_G_Freq_mult_TAU * smooth + Exterior_G_Phase_mult_TAU));
				outB += Exterior_B_Amp_mult_Exterior_Alpha * (0.5f - 0.5f * cos(Exterior_B_Freq_mult_TAU * smooth + Exterior_B_Phase_mult_TAU));
				outA += Exterior_Alpha;
			} else {
				outR += Interior_R_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cos(log(low) * Interior_R_Freq + Interior_R_Phase));
				outG += Interior_G_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cos(log(low) * Interior_G_Freq + Interior_G_Phase));
				outB += Interior_B_Amp_mult_Interior_Alpha * (0.5f - 0.5f * cos(log(low) * Interior_B_Freq + Interior_B_Phase));
				outA += Interior_Alpha;
			}
			x++;
		}
		x -= sample;
		y++;
	}
	if (outA != 0.0f) {
		outR = outR / outA;
		outG = outG / outA;
		outB = outB / outA;
		outA = outA / (fp32)(sample * sample);
	}
	outR *= 255.0f;
	outG *= 255.0f;
	outB *= 255.0f;
	outA *= 255.0f;
	//uint32_t outAvr = (outR + outG + outB) / 3; outR = outAvr; outG = outAvr; outB = outAvr; /* Grey-scale */
	//uint32_t outAvr = (outR + outG + outB) / 3; outR = (outAvr + outR) / 2; outG = (outAvr + outG) / 2; outB = (outAvr + outB) / 2; /* Low-saturation */
	id *= IMAGE_BUFFER_CHANNELS;
	resultBuf[id] = (uint8_t)outR; id++;
	resultBuf[id] = (uint8_t)outG; id++;
	resultBuf[id] = (uint8_t)outB; id++;
	resultBuf[id] = (uint8_t)outA;
}