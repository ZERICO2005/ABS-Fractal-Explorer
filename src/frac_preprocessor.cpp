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
int preCalc_Frac_Param(
	Frac_Param_PC& param_PC,
	const ABS_Mandelbrot& frac
) {
	/* Type */
		param_PC.polarMandelbrot = frac.polarMandelbrot;
	/* Coordinates */
		param_PC.r = frac.r;
		param_PC.i = frac.i;
		param_PC.zr = frac.zr;
		param_PC.zi = frac.zi;
		param_PC.zoom_PC = pow(10.0,frac.zoom);
	/* Transformation */
		param_PC.rotSin_PC = sin(frac.rot);
		param_PC.rotCos_PC = cos(frac.rot);
		param_PC.sX = frac.sX;
		param_PC.sY = frac.sY;
	/* Parameters */
		param_PC.maxItr = frac.maxItr;
		param_PC.formula = frac.formula;
		param_PC.breakoutValue = frac.breakoutValue;
			param_PC.power = frac.power;
			param_PC.polarPower = frac.polarPower;
	/* Coloring */
		param_PC.smoothColoring = frac.smoothColoring;
		param_PC.r_Amp = frac.rA;
		param_PC.r_Freq = frac.rF;
		param_PC.r_Phase = frac.rP;
		param_PC.g_Amp = frac.gA;
		param_PC.g_Freq = frac.gF;
		param_PC.g_Phase = frac.gP;
		param_PC.b_Amp = frac.bA;
		param_PC.b_Freq = frac.bF;
		param_PC.b_Phase = frac.bP;
		param_PC.interior_Amp = frac.bA;
		param_PC.interior_Freq = frac.bF;
		param_PC.interior_Phase = frac.bP;
		param_PC.interior_Alpha = 0xFF;
		param_PC.exterior_Alpha = 0xFF;
	return 0;
}

// Returns 0 on success, negative otherwise
int preCalc_Frac_Render(
	Frac_Render_PC& render_PC,
	const ABS_Mandelbrot& render_data
) {
	return 0;
}

// Returns 0 on success, negative otherwise
int preCalc_Frac_Task(
	Frac_Task_PC& task_PC,
	const ABS_Mandelbrot& frac, const ABS_Mandelbrot& render_data,
	nano64_t timeStamp
) {
	return 0;
}

// Returns 0 on success, negative otherwise
int mergeTo_Frac_Task(
	Frac_Task_PC& task_PC,
	Frac_Param_PC& param_PC, Frac_Render_PC& render_PC,
	nano64_t timeStamp
) {
	return 0;
}