/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"
#include "copyBuffer.h"
#include "frac_preprocessor.h"

template<typename fpX, typename fpC>
void colorExterior(
	const Frac_Param_PC<fpX,fpC>& param, uint32_t itr, fpX zs,
	uint32_t& outR, uint32_t& outG, uint32_t& outB, uint32_t& outA
) {
	// log2(log2(zs)) - 1 == log2(log2(sqrt(zs)))
	fpC smooth = log1p(fmax((fpC)0.0, (fpC)itr - (fpC)(log2(log2(zs)) - (fpC)1.0) * param.recip_log2_Power));
	outR += (uint32_t)(param.r_Amp * ((param.Color_Mult) - (param.Color_Mult) * cos(param.r_Freq * smooth + param.r_Phase)));
	outG += (uint32_t)(param.gA * ((param.Color_Mult) - (param.Color_Mult) * cos(param.g_Freq * smooth + param.g_Phase)));
	outB += (uint32_t)(param.bA * ((param.Color_Mult) - (param.Color_Mult) * cos(param.b_Freq * smooth + param.b_Phase)));
	outA += param.exterior_Alpha;
}

template<typename fpX, typename fpC>
void colorInterior(
	const Frac_Param_PC<fpX,fpC>& param, fpC low,
	uint32_t& outR, uint32_t& outG, uint32_t& outB, uint32_t& outA
) {
	outR += 0;
	outG += 0;
	outB += (uint32_t)(param.interior_Alpha * ((param.Color_Mult) - (param.Color_Mult) * cos(log(low) * param.interior_Freq + param.interior_Phase)));
	outA += param.interior_Alpha;
}




template<typename fpX, typename fpC>
void parallel_Quadratic(
	fpX cr, fpX ci, fpX zr, fpX zi,
	const Frac_Param_PC<fpX,fpC>& param,
	const fpX* mult, const bool* formulaBit,
	uint32_t& outR, uint32_t& outG, uint32_t& outB, uint32_t& outA
) {
	fpX low = 4.0;
	fpX temp = 0.0;
	fpX zs = 0.0;
	fpX zr1,zi1,zr2,zi2;
	for (uint32_t itr = 0; itr < param.maxItr; itr++) {
		zr1 = (formulaBit[0]) ? abs(zr) : zr;
		zi1 = (formulaBit[1]) ? abs(zi) : zi;
		zr2 = (formulaBit[2]) ? abs(zr) : zr;
		zi2 = (formulaBit[3]) ? abs(zi) : zi;
		temp = ((zr1 * zr) - mult[0] * (zi1 * zi));
		zi = (zr2 * zi2 * mult[1]) + ci;
		zr = (formulaBit[4]) ? mult[2] * fabs(temp) + cr : mult[2] * temp + cr;
		
		zs = zr * zr + zi * zi;
		if (zs < low) {
			low = zs;
		} else if (zs > param.breakoutValue) {
			colorExterior(
				param,itr,zs,
				outR,outG,outB,outA
			);
			return;
		}
	}
	colorInterior(
		param,(fpC)low,
		outR,outG,outB,outA
	);
}

template<typename fpX, typename fpC>
void point_ABS_Mandelbrot(
	int32_t x, int32_t y,
	const Frac_Render_PC& ren, const Frac_Param_PC<fpX,fpC>& param,
	const fpX* mult, const bool* formulaBit,
	/*__restrict*/ uint8_t* buf,
	std::atomic<bool>& ABORT_RENDERING
) {
	uint32_t outR = 0;
	uint32_t outG = 0;
	uint32_t outB = 0;
	uint32_t outA = 0;
	for (int32_t v = 0; v < ren.super_sampleY; v++) {
		for (int32_t u = 0; u < ren.super_sampleX; u++) {
			if (ABORT_RENDERING) {
				return;
			}
			fpX xCord = ((fpX)x * param.recip_numZ);
			fpX yCord = ((fpX)y * param.neg_recip_numW);
			fpX cr = (!param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.r) : param.zr0;
			fpX ci = (!param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.i) : param.zi0;
			fpX zr = (param.juliaSet) ? ((xCord * param.rotCos_PC - yCord * param.rotSin_PC) + param.r) : param.zr0;
			fpX zi = (param.juliaSet) ? ((yCord * param.rotCos_PC + xCord * param.rotSin_PC) + param.i) : param.zi0;
			switch (param.power) {
				case 2:
				parallel_Quadratic(
					cr,ci,zr,zi,
					ren, param,
					mult, formulaBit,
					outR,outG,outB,outA,
					ABORT_RENDERING
				); break;
				case 3:
				parallel_Cubic(
					cr,ci,zr,zi,
					ren, param,
					mult, formulaBit,
					outR,outG,outB,outA,
					ABORT_RENDERING
				); break;
				case 4:
				parrallel_Quartic(
					cr,ci,zr,zi,
					ren, param,
					mult, formulaBit,
					outR,outG,outB,outA,
					ABORT_RENDERING
				); break;
				case 5:
				parrallel_Quintic(
					cr,ci,zr,zi,
					ren, param,
					mult, formulaBit,
					outR,outG,outB,outA,
					ABORT_RENDERING
				); break;
				case 6:
				parrallel_Sextic(
					cr,ci,zr,zi,
					ren, param,
					mult, formulaBit,
					outR,outG,outB,outA,
					ABORT_RENDERING
				); break;
			};
			x++;
		}
		x -= ren.super_sampleX;
		y++;
	}
	uint32_t div_Color = (param.Div_Mult * ren.super_sampleX * ren.super_sampleY);
	*buf = (uint8_t)(outR / div_Color); buf++;
	*buf = (uint8_t)(outG / div_Color); buf++;
	*buf = (uint8_t)(outB / div_Color);
	if (ren.channels == 4) {
		buf++;
		*buf = (uint8_t)(outA / div_Color);
	} 
}
 
template<typename fpX, typename fpC>
void parallel_ABS_Mandelbrot(
	const Frac_Render_PC& ren, const Frac_Param_PC<fpX,fpC>& param,
	/*__restrict*/ uint8_t* buf, size_t len,
	std::atomic<bool>& ABORT_RENDERING
) {
	fpX mult[param.mult_Count]; 
	bool formulaBit[param.bool_Count];
	for (int32_t y = ren.start_PosY; y < ren.end_PosY; y += ren.super_sampleY) {
		for (int32_t x = ren.start_PosY; x < ren.end_PosY; x += ren.super_sampleX) {
			if (len == 0 || ABORT_RENDERING) {
				return;
			}
			point_ABS_Mandelbrot(
				x,y,
				ren,param,
				mult,formulaBit,
				buf,
				ABORT_RENDERING
			);
			len--;
		}
	}
}

void parallel_CPU_rendering(
	BufferBox* buf, Render_Data ren, ABS_Mandelbrot param,
	std::atomic<bool>& ABORT_RENDERING, uint32_t tc = 1
) {
	
	
}