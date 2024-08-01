/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"

#if defined(ENABLE_MPFR_RENDERING) && Enable_FloatMPFR

#include <mpfr.h>

#define MPFR_RoundMode MPFR_RNDZ

constexpr mpfr_prec_t MPFR_Precision = 256;

typedef long Int_MPFR;
typedef unsigned long Uint_MPFR;

typedef mpfr_sign_t Fabs_Mask_MPFR;

constexpr mpfr_sign_t Positive_MPFR = 1;
constexpr mpfr_sign_t Negative_MPFR = -1;

/* Basic Arithmetic */
	
	#define MPFR_Add(ret, x, y) \
		mpfr_add(ret, x, y, MPFR_RoundMode)

	#define MPFR_Sub(ret, x, y) \
		mpfr_sub(ret, x, y, MPFR_RoundMode)

	#define MPFR_Mul(ret, x, y) \
		mpfr_mul(ret, x, y, MPFR_RoundMode)

	#define MPFR_Div(ret, x, y) \
		mpfr_div(ret, x, y, MPFR_RoundMode)

	#define MPFR_Dim(ret, x, y) \
		mpfr_sub(ret, x, y, MPFR_RoundMode)
	
/* Unary */

	#define MPFR_Neg(x) \
		mpfr_neg(x, MPFR_RoundMode)

	#define MPFR_Fabs(x) \
		mpfr_abs(x, MPFR_RoundMode)

/* Logs and Exponents */

	#define MPFR_Pow(ret, x, y) \
		mpfr_pow(ret, x, y, MPFR_RoundMode)

	#define MPFR_Log(ret, x) \
		mpfr_log(ret, x, MPFR_RoundMode)

	#define MPFR_Log2(ret, x) \
		mpfr_log2(ret, x, MPFR_RoundMode)

/* Comparison */

	// x == y
	#define MPFR_EQ(x, y) \
		(mpfr_equal_p(x, y) != 0)

	// x != y
	#define MPFR_NEQ(x, y) \
		(mpfr_equal_p(x, y) == 0)

	// x > y
	#define MPFR_GT(x, y) \
		(mpfr_greater_p(x, y) != 0)
	
	// x >= y
	#define MPFR_GTEQ(x, y) \
		(mpfr_greaterequal_p(x, y) != 0)

	// x < y
	#define MPFR_LT(x, y) \
		(mpfr_less_p(x, y) != 0)
	
	// x <= y
	#define MPFR_LTEQ(x, y) \
		(mpfr_lessequal_p(x, y) != 0)

	// x == 0
	#define MPFR_EQ0(x) \
		(mpfr_zero_p(x) != 0)

	// x != 0
	#define MPFR_NEQ0(x) \
		(mpfr_zero_p(x) == 0)

	#define MPFR_MIN(ret, x, y) \
		mpfr_min(ret, x, y, MPFR_RoundMode)

	#define MPFR_MAX(ret, x, y) \
		mpfr_max(ret, x, y, MPFR_RoundMode)

/* Trigonometry */

	#define MPFR_Sin(ret, x) \
		mpfr_sin(ret, x, y, MPFR_RoundMode)

	#define MPFR_Cos(ret, x) \
		mpfr_cos(ret, x, y, MPFR_RoundMode)

	#define MPFR_SinCos(ret_sin, ret_cos, x) \
		mpfr_cos(ret_sin, ret_cos, x, MPFR_RoundMode)

	#define MPFR_ArcTan2(ret, y, x) \
		mpfr_cos(ret, y, x, MPFR_RoundMode)


/* Fused Multiply Add */
	// Fused Mult Add: ret = a * b + c
	#define MPFR_FMA(ret, a, b, c) \
		mpfr_fma(ret, a, b, c, MPFR_RoundMode)

	// Fused Mult Sub: ret = a * b - c
	#define MPFR_FMS(ret, a, b, c) \
		mpfr_fms(ret, a, b, c, MPFR_RoundMode)

	// Fused Mult Mult Add: ret = a * b + x * y
	#define MPFR_FMMA(ret, a, b, x, y) \
		mpfr_fmma(ret, a, b, x, y, MPFR_RoundMode)

	// Fused Mult Mult Sub: ret = a * b + x * y
	#define MPFR_FMMS(ret, a, b, x, y) \
		mpfr_fmms(ret, a, b, x, y, MPFR_RoundMode)

/* Optimized Functions */

	#define MPFR_Fabs_Mask(x, mask) \
		x[0]._mpfr_sign &= mask;

	#define MPFR_Square(ret, x) \
		mpfr_sqr(ret, x, MPFR_RoundMode)

	// Uint Pow
	#define MPFR_PowN(ret, x, y) \
		mpfr_pow_ui(ret, x, y, MPFR_RoundMode)

	// x * Int
	#define MPFR_Mul_Int(ret, x, y) \
		mpfr_mul_si(ret, x, y, MPFR_RoundMode)
	
	// x / Int
	#define MPFR_Div_Int(ret, x, y) \
		mpfr_div_si(ret, x, y, MPFR_RoundMode)

	// x * 2^Uint
	#define MPFR_Mul_Pow2(ret, x, y) \
		mpfr_mul_2ui(ret, x, y, MPFR_RoundMode)
	
	// x / 2^Uint
	#define MPFR_Div_Pow2(ret, x, y) \
		mpfr_div_2ui(ret, x, y, MPFR_RoundMode)

/* Conversions */

	#define MPFR_from_MPFR(ret, x) \
		mpfr_set(ret, x, MPFR_RoundMode)

	#define MPFR_from_Float32(ret, x) \
		mpfr_set_flt(ret, x, MPFR_RoundMode)
	
	#define MPFR_from_Float64(ret, x) \
		mpfr_set_d(ret, x, MPFR_RoundMode)
	
	#define MPFR_from_FloatCord(ret, x) \
		mpfr_set_d(ret, x, MPFR_RoundMode)

	#define MPFR_from_Int(ret, x) \
		mpfr_set_si(ret, x, MPFR_RoundMode)
	
	#define MPFR_from_Uint(ret, x) \
		mpfr_set_ui(ret, x, MPFR_RoundMode)

	#define MPFR_to_Float32(x) \
		mpfr_get_flt(x, MPFR_RoundMode)

	#define MPFR_to_Float64(x) \
		mpfr_get_d(x, MPFR_RoundMode)
		
	#define MPFR_to_FloatCord(x) \
		mpfr_get_d(x, MPFR_RoundMode)

/* Function Definition */

	#define color_pow_2(x) (x) * (x)

	// log(low)
	static void CPU_Interior_Coloring_MPFR(
		fp64* outputColor,
		const PreCalc_Param<fp64, fp64>& param,
		fp64 log_low
	) {
		outputColor[0] += color_pow_2(param.Interior_R_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log_low * param.Interior_R_Freq + param.Interior_R_Phase_mult_TAU)));
		outputColor[1] += color_pow_2(param.Interior_G_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log_low * param.Interior_G_Freq + param.Interior_G_Phase_mult_TAU)));
		outputColor[2] += color_pow_2(param.Interior_B_Amp_mult_Interior_Alpha * (0.5 - 0.5 * cos(log_low * param.Interior_B_Freq + param.Interior_B_Phase_mult_TAU)));
		outputColor[3] += color_pow_2(param.Interior_Alpha);
	}

	// log2(zs) / 2.0
	static void CPU_Exterior_Coloring_MPFR(
		fp64* outputColor,
		const PreCalc_Param<fp64, fp64>& param,
		uint32_t itr, fp64 log2_zs, fp64 inverse_log2_power
	) {
		// TAU = 2 * PI
		fp64 smooth = log1p(fmax(0.0, (fp64)itr - log2(log2_zs / 2.0) * inverse_log2_power));
		outputColor[0] += color_pow_2(param.Exterior_R_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_R_Freq_mult_TAU * smooth + param.Exterior_R_Phase_mult_TAU)));
		outputColor[1] += color_pow_2(param.Exterior_G_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_G_Freq_mult_TAU * smooth + param.Exterior_G_Phase_mult_TAU)));
		outputColor[2] += color_pow_2(param.Exterior_B_Amp_mult_Exterior_Alpha * (0.5 - 0.5 * cos(param.Exterior_B_Freq_mult_TAU * smooth + param.Exterior_B_Phase_mult_TAU)));
		outputColor[3] += color_pow_2(param.Exterior_Alpha);
	}
	
	#define Block_Init_MPFR();

	#define Block_BeginLoop_MPFR();
		

	#define Block_EndLoop_MPFR();
							

/* Fractal Rendering MPFR */

	void quadraticRender_MPFR(FractalParameters(fp64, fp64)) {

		Block_Init_MPFR();

			mpfr_t zr1, zr2, zi1, zi2;
			mpfr_inits2(MPFR_Precision,
				zr1, zr2, zi1, zi2,
			nullptr);

			bool f[8]; // 8 bit formula
			for (uint8_t q = 0; q < 8; q++) {
				f[q] = ((param.formula >> q) & 1) ? true : false;
			}
			
			// Bits 0-2 will flip signage
			const Int_MPFR s1_1 = f[0] ? -1 : 1;
			const Int_MPFR s2_1 = f[1] ? -1 : 1;
			const Int_MPFR s3_2 = f[2] ? -2 : 2;
			
			// Bits 3-7 will apply fabs() via a mask
			const Fabs_Mask_MPFR zr1_mask = f[3] ? Positive_MPFR : Negative_MPFR;
			const Fabs_Mask_MPFR zi1_mask = f[4] ? Positive_MPFR : Negative_MPFR;
			const Fabs_Mask_MPFR zr2_mask = f[5] ? Positive_MPFR : Negative_MPFR;
			const Fabs_Mask_MPFR zi2_mask = f[6] ? Positive_MPFR : Negative_MPFR;
			const Fabs_Mask_MPFR zr_mask  = f[7] ? Positive_MPFR : Negative_MPFR;

		Block_BeginLoop_MPFR();
			\
			/* Load Precalculated constants */\
			mpfr_t /* Declaration */
				realCord, imagCord, realJulia, imagJulia,
				rotSin_PC, rotCos_PC, breakoutValue,
				numY, numX, recip_numZ, neg_recip_numW;

			mpfr_inits2(MPFR_Precision,
				realCord, imagCord, realJulia, imagJulia,
				rotSin_PC, rotCos_PC, breakoutValue,
				numY, numX, recip_numZ, neg_recip_numW,
			nullptr);

			MPFR_from_FloatCord(realCord     , param.realCord     );
			MPFR_from_FloatCord(imagCord     , param.imagCord     );
			MPFR_from_FloatCord(realJulia    , param.realJulia    );
			MPFR_from_FloatCord(imagJulia    , param.imagJulia    );
			MPFR_from_FloatCord(rotSin_PC    , param.rotSin_PC    );
			MPFR_from_FloatCord(rotCos_PC    , param.rotCos_PC    );
			MPFR_from_FloatCord(breakoutValue, param.breakoutValue);
			/* numX, numY, numZ, and numW are constants used to caluculate the cordinates */\
			MPFR_from_FloatCord(numY          , param.numY          );\
			MPFR_from_FloatCord(numX          , param.numX          );\
			MPFR_from_FloatCord(recip_numZ    , param.recip_numZ    );\
			MPFR_from_FloatCord(neg_recip_numW, param.neg_recip_numW);\
			\
			__attribute__((unused)) const fp64 Alpha_Mult = 255.0 / param.alphaDiv;\
			\
			/* Init Loop */\
			size_t dataPtr = p0 * IMAGE_BUFFER_CHANNELS; /* Determines the starting image offset/index */\
			int32_t y = (int32_t)(p0 / (size_t)param.Image_ResX); /* Determines the starting y offset/index */\
			int32_t x = (int32_t)(p0 % (size_t)param.Image_ResX); /* Determines the starting x offset/index */\
			x *= param.sample; /* Scales x by the samples per pixel */\
			y *= param.sample; /* Scales x by the samples per pixel */\
			/* Local Variables */
				mpfr_t xCord, yCord, cr, ci, zr, zi, low, zs;
				mpfr_inits2(MPFR_Precision,
					xCord, yCord, cr, ci, zr, zi, low, zs,
				nullptr);
				/* __attribute__((unused)) */ mpfr_t temp_zr;
				mpfr_init2(temp_zr, MPFR_Precision);
				
			for (; y < param.Cord_ResY; y += param.sample) {\
				for (; x < param.Cord_ResX; x += param.sample) {\
					/* returns once the image offset/index reaches the end offset/index */\
					if (p0 >= p1 || ABORT_RENDERING == true) {\
						return;\
					}\
					fp64 outputColor[IMAGE_BUFFER_CHANNELS] = {0.0, 0.0, 0.0, 0.0};

					for (int32_t v = 0; v < param.sample; v++) {\
						/* Calculates y cordinate-value */\
						MPFR_from_Int(yCord, (Int_MPFR)y);\
						MPFR_Sub(yCord, yCord, numY);\
						MPFR_Mul(yCord, yCord, neg_recip_numW);
						\
						for (int32_t u = 0; u < param.sample; u++) {\
							/* Calculates the 4 x cordinate-values */\
							MPFR_from_Int(xCord, (Int_MPFR)x);\
							MPFR_Sub(xCord, xCord, numX);\
							MPFR_Mul(xCord, xCord, recip_numZ);
							\
							/* Transforms the x and y cordinate-values into the cordinates */\
							/* `cr` and `ci` are swapped with `zr` and `zi` if `param.juliaSet == true` */\
							
							MPFR_FMMS(cr, xCord, rotCos_PC, yCord, rotSin_PC);
							MPFR_Add (cr, cr, realCord);

							MPFR_FMMA(ci, yCord, rotCos_PC, xCord, rotSin_PC);
							MPFR_Add (ci, ci, imagCord);
							MPFR_from_MPFR(zr, realJulia);
							MPFR_from_MPFR(zi, imagJulia);
							if (param.juliaSet == true) {
								mpfr_swap(cr, zr);
								mpfr_swap(ci, zi);
							}
							\
							MPFR_from_Uint(low, (Uint_MPFR)4);\
							mpfr_set_zero(zs, Positive_MPFR);\
							mpfr_set_zero(temp_zr, Positive_MPFR);
							uint32_t itr = 0;
							for (; itr < param.maxItr; itr++) {
		
			// Applies fabs() to zr and zi
			MPFR_from_MPFR(zr1, zr); MPFR_Fabs_Mask(zr1, zr1_mask);
			MPFR_from_MPFR(zr2, zr); MPFR_Fabs_Mask(zr2, zr2_mask);
			MPFR_from_MPFR(zi1, zi); MPFR_Fabs_Mask(zi1, zi1_mask);
			MPFR_from_MPFR(zi2, zi); MPFR_Fabs_Mask(zi2, zi2_mask);
			
			// Calculates the new zr and zi
			MPFR_Mul_Int(zi1, zi1, s2_1);
			MPFR_FMMS(zr, zr1, zr, zi1, zi);
			MPFR_Mul_Int(zr, zr, s1_1);
			MPFR_Fabs_Mask(zr, zr_mask);
			MPFR_Add(zr, zr, cr);

			MPFR_Mul_Int(zi, zi2, s3_2);
			MPFR_FMA(zi, zi, zr2, ci);
			MPFR_FMMA(zs, zr, zr, zi, zi);
			
		Block_EndLoop_MPFR();
								/* Tracks the lowest value */\
								MPFR_MIN(low, low, zs);\

								if (MPFR_GT(zs, breakoutValue)) {
									break;
								}
							}\
							if (MPFR_LTEQ(zs, breakoutValue)) {\
								/* Z^2 values that did Not exceed the breakout value will use `Interior_Coloring` */\
								MPFR_Log(low, low);
								CPU_Interior_Coloring_MPFR(\
									outputColor, param, MPFR_to_Float64(low)\
								);\
							} else {
								/* Z^2 values that exceeded the breakout value will use `Exterior_Coloring` */\
								MPFR_Log2(zs, zs);
								CPU_Exterior_Coloring_MPFR(\
									outputColor, param,\
									itr, MPFR_to_Float64(zs), param.inverse_log2_power\
								);\
							}
							x++;\
						}\
						x -= param.sample;\
						y++;\
					}\
					y -= param.sample;\
					\
					/* Code to write the colors to the image buffer */\
					\
					/* Divide by alpha and normalize colors */\

						if (outputColor[3] != 0.0) {\
							outputColor[0] = sqrt(outputColor[0] / outputColor[3]);\
							outputColor[1] = sqrt(outputColor[1] / outputColor[3]);\
							outputColor[2] = sqrt(outputColor[2] / outputColor[3]);\
							outputColor[3] = sqrt(outputColor[3] / param.alphaDiv);\
						}\
						outputColor[0] *= 255.0;\
						outputColor[1] *= 255.0;\
						outputColor[2] *= 255.0;\
						outputColor[3] *= 255.0;\
						/* Writes in the equivilant of SDL_PIXELFORMAT_ABGR8888 */\
						data[dataPtr] = (uint8_t)outputColor[0]; dataPtr++;\
						data[dataPtr] = (uint8_t)outputColor[1]; dataPtr++;\
						data[dataPtr] = (uint8_t)outputColor[2]; dataPtr++;\
						data[dataPtr] = (uint8_t)outputColor[3]; dataPtr++;\

					/* Increases the image offset/index */\
					p0++;\
				}\
				x = 0;\
			}
			/* Clear Locals */
				mpfr_clears(
					xCord, yCord, cr, ci, zr, zi, low, zs,
				nullptr);
				mpfr_clear(temp_zr);
			/* Clear Constants */
				mpfr_clears(
					realCord, imagCord, realJulia, imagJulia,
					rotSin_PC, rotCos_PC, breakoutValue,
					numY, numX, recip_numZ, neg_recip_numW,
				nullptr);
		mpfr_clears(
			zr1, zr2, zi1, zi2,
		nullptr);
	}

#endif /* ENABLE_MPFR_RENDERING */