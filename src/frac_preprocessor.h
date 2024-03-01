/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_PREPROCESSOR_H
#define FRAC_PREPROCESSOR_H

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"

// Stores precalculated (PC) fractal parameters. Struct will be read only in rendering engines
struct Frac_Param_PC {
	/* Type */
	bool polarMandelbrot;
	/* Coordinates */
	fp128 r;
	fp128 i;
	fp128 zr;
	fp128 zi;
	fp64 zoom_PC;
	/* Transformation */
	fp128 rotSin_PC;
	fp128 rotCos_PC;
	fp64 sX;
	fp64 sY;
	/* Parameters */
	uint32_t maxItr;
	uint64_t formula;
	fp64 breakoutValue;
		uint32_t power;
		fp64 polarPower;
	/* Coloring */
	bool smoothColoring;
	fp64 r_Amp;
	fp64 r_Freq;
	fp64 r_Phase;
	fp64 g_Amp;
	fp64 g_Freq;
	fp64 g_Phase;
	fp64 b_Amp;
	fp64 b_Freq;
	fp64 b_Phase;
	fp64 interior_Amp;
	fp64 interior_Freq;
	fp64 interior_Phase;
	uint8_t interior_Alpha;
	uint8_t exterior_Alpha;
}; typedef struct Frac_Param_PC;

// Read only precalculated values
struct Frac_Render_PC {
	uint32_t total_ResX;
	uint32_t total_ResY;
	int32_t offset_PosX;
	int32_t offset_PosY;
	uint32_t sub_ResX;
	uint32_t sub_ResY;

	size_t total_Size;
	size_t pitch;
	uint8_t channels;

	bool export_Image;
	uint32_t rendering_method;
	uint32_t CPU_Precision;
	uint32_t CPU_Threads;
	uint32_t GPU_Precision;
	uint32_t GPU_Partitions;
}; typedef struct Frac_Render_PC;

struct Frac_Task_PC {
	nano64_t timeStamp;
	Frac_Param_PC param_PC;
	Frac_Render_PC render_PC;
}; typedef struct Frac_Task_PC;

// Returns 0 on success, negative otherwise
int preCalc_Frac_Param(
	Frac_Param_PC& param_PC,
	const ABS_Mandelbrot& frac
);

// Returns 0 on success, negative otherwise
int preCalc_Frac_Render(
	Frac_Render_PC& render_PC,
	const ABS_Mandelbrot& render_data
);

// Returns 0 on success, negative otherwise
int preCalc_Frac_Task(
	Frac_Task_PC& task_PC,
	const ABS_Mandelbrot& frac, const ABS_Mandelbrot& render_data,
	nano64_t timeStamp = 0
);

// Returns 0 on success, negative otherwise
int mergeTo_Frac_Task(
	Frac_Task_PC& task_PC,
	Frac_Param_PC& param_PC, Frac_Render_PC& render_PC,
	nano64_t timeStamp = 0
);

#endif /* FRAC_PREPROCESSOR_H */