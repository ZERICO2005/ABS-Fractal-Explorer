/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_PARALLEL_H
#define FRAC_PARALLEL_H

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"
#include "copyBuffer.h"

void parallel_CPU_rendering(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t tc = 1);

#endif /* FRAC_PARALLEL_H */