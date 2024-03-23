/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_CL_H
#define FRAC_CL_H

#include "Common_Def.h"
#include "Program_Def.h"
#include "fractal.h"
#include "copyBuffer.h"

int32_t renderOpenCL_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING);

int32_t init_OpenCL();
int32_t terminate_OpenCL();
void queryOpenCL_GPU();

#define writefToLog(...) //printFlush(__VA_ARGS__)

void calculate_GPU_Hardware_Hash(uint64_t& hash);

#endif /* FRAC_CL_H */