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
template<typename fpX, typename fpC>
struct Frac_Param_PC {
	/* Type */
		bool polarMandelbrot;
		bool juliaSet;
	/* Coordinates */
		fpX r;
		fpX i;
		fpX zr;
		fpX zi;
		fpX zoom_PC;
		fpX zr0;
		fpX zi0;
	/* Transformation */
		fpX rotSin_PC;
		fpX rotCos_PC;
		fpX sX;
		fpX sY;
	/* Values */
		fpX numX;
		fpX numY;
		fpX recip_numZ;
		fpX neg_recip_numW;
	/* Parameters */
		uint32_t maxItr;
		uint64_t formula;
		fpX breakoutValue;
			uint32_t power;
			fpX polarPower;
			fpX recip_log2_Power;
			size_t mult_Count;
			size_t bool_Count;
	/* Coloring */
		// Phase and Freq are pre-multiplied by TAU
		fpC r_Amp;
		fpC r_Freq;
		fpC r_Phase;
		fpC g_Amp;
		fpC g_Freq;
		fpC g_Phase;
		fpC b_Amp;
		fpC b_Freq;
		fpC b_Phase;
		fpC interior_Amp;
		fpC interior_Freq;
		fpC interior_Phase;
		// Other
		fpC Color_Mult;
		uint32_t Div_Mult;
		uint8_t interior_Alpha;
		uint8_t exterior_Alpha;
		bool smoothColoring;
};

// Read only precalculated values
struct Frac_Render_PC {
	int32_t total_ResX;
	int32_t total_ResY;
	int32_t sub_ResX;
	int32_t sub_ResY;
	int32_t start_PosX;
	int32_t start_PosY;
	int32_t end_PosX;
	int32_t end_PosY;

	size_t total_Size;
	size_t pitch;
	uint8_t channels;

	int32_t super_sampleX;
	int32_t super_sampleY;

	bool export_Image;
	uint32_t rendering_method;
	uint32_t CPU_Precision;
	uint32_t CPU_Threads;
	uint32_t GPU_Precision;
	uint32_t GPU_Partitions;
}; typedef struct Frac_Render_PC Frac_Render_PC;

template <typename fpX, typename fpC>
struct Frac_Task_PC {
	nano64_t timeStamp;
	Frac_Param_PC<fpX,fpC> param_PC;
	Frac_Render_PC render_PC;
};

// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int preCalc_Frac_Param(
	Frac_Param_PC<fpX,fpC>& param_PC,
	const ABS_Mandelbrot& frac
);

// Returns 0 on success, negative otherwise
int preCalc_Frac_Render(
	Frac_Render_PC& render_PC,
	const Render_Data& render_data
);

// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int preCalc_Frac_Task(
	Frac_Task_PC<fpX,fpC>& task_PC,
	const ABS_Mandelbrot& frac, const ABS_Mandelbrot& render_data,
	nano64_t timeStamp = 0
);


// Returns 0 on success, negative otherwise
template <typename fpX, typename fpC>
int mergeTo_Frac_Task(
	Frac_Task_PC<fpX,fpC>& task_PC,
	Frac_Param_PC<fpX,fpC>& param_PC, Frac_Render_PC& render_PC,
	nano64_t timeStamp = 0
);

#endif /* FRAC_PREPROCESSOR_H */