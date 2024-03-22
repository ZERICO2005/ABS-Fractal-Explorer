/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "frac_preprocessor.h"
#include "fractal.h"


// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int preCalc_Frac_Param(
	Frac_Param_PC<fpX,fpC>& param_PC,
	const ABS_Mandelbrot& frac
) {
	/* Type */
		param_PC.polarMandelbrot = frac.polarMandelbrot;
		param_PC.polarMandelbrot = frac.juliaSet;
	/* Coordinates */
		param_PC.r = frac.r;
		param_PC.i = frac.i;
		param_PC.zr = frac.zr;
		param_PC.zi = frac.zi;
		param_PC.zoom_PC = (fpX)pow((fp128)10.0,(fp128)frac.zoom);
	/* Transformation */
		param_PC.rotSin_PC = (fpX)sin((fp128)frac.rot);
		param_PC.rotCos_PC = (fpX)cos((fp128)frac.rot);
		param_PC.sX = frac.sX;
		param_PC.sY = frac.sY;
	/* Parameters */
		param_PC.maxItr = frac.maxItr;
		param_PC.formula = frac.formula;
		param_PC.breakoutValue = frac.breakoutValue;
			param_PC.power = frac.power;
			param_PC.polarPower = frac.polarPower;
			param_PC.recip_log2_Power = (fpX)((fp128)1.0 / log2((fp128)frac.polarPower));
			param_PC.mult_count = 0;
			param_PC.bool_count = 0;
	/* Coloring */
		param_PC.smoothColoring = frac.smoothColoring;
		param_PC.r_Amp = frac.rA;
		param_PC.r_Freq = frac.rF * TAU;
		param_PC.r_Phase = frac.rP * TAU;
		param_PC.g_Amp = frac.gA;
		param_PC.g_Freq = frac.gF * TAU;
		param_PC.g_Phase = frac.gP * TAU;
		param_PC.b_Amp = frac.bA;
		param_PC.b_Freq = frac.bF * TAU;
		param_PC.b_Phase = frac.bP * TAU;
		param_PC.interior_Amp = frac.bA;
		param_PC.interior_Freq = frac.bF * TAU;
		param_PC.interior_Phase = frac.bP * TAU;
		param_PC.Color_Mult = (fpX)12.5;
		param_PC.Div_Mult = 1;
		param_PC.interior_Alpha = 0xFF;
		param_PC.exterior_Alpha = 0xFF;
		param_PC.smoothColoring = true;
	return 0;
}

// Returns 0 on success, negative otherwise
int preCalc_Frac_Render(
	Frac_Render_PC& render_PC,
	const Render_Data& render_data
) {
	render_PC.total_ResX = render_data.resX;
	render_PC.total_ResY = render_data.resY;
	render_PC.sub_ResX = render_data.subResX;
	render_PC.sub_ResY = render_data.subResY;
	render_PC.start_PosX = render_data.offsetX;
	render_PC.start_PosY = render_data.offsetY;
	render_PC.end_PosX = render_data.offsetX + render_data.subResX;
	render_PC.end_PosY = render_data.offsetY + render_data.subResY;

	render_PC.total_Size = render_data.resX * render_data.resY * render_data.channels;
	render_PC.pitch = render_data.resY * render_data.channels;
	render_PC.channels = render_data.channels;

	render_PC.super_sampleX = render_data.sample;
	render_PC.super_sampleY = render_data.sample;

	render_PC.export_Image = render_data.export_Image;
	render_PC.rendering_method = render_data.rendering_method;
	render_PC.CPU_Precision = render_data.CPU_Precision;
	render_PC.CPU_Threads = render_data.CPU_Threads;
	render_PC.GPU_Precision = render_data.GPU_Precision;
	render_PC.GPU_Partitions = render_data.GPU_Partitions;
	return 0;
}

// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int preCalc_Frac_Task(
	Frac_Task_PC<fpX, fpC>& task_PC,
	const ABS_Mandelbrot& frac, const ABS_Mandelbrot& render_data,
	nano64_t timeStamp
) {
	return 0;
}

// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int mergeTo_Frac_Task(
	Frac_Task_PC<fpX, fpC>& task_PC,
	Frac_Param_PC<fpX, fpC>& param_PC, Frac_Render_PC& render_PC,
	nano64_t timeStamp
) {

	return 0;
}