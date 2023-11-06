/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_MULTI_H
#define FRAC_MULTI_H

#include "Common_Def.h"
#include "fractal.h"
#include "copyBuffer.h"

//void renderFormula_MultiThread(ABS_Fractal* parameter, ImageBuffer* buf, u32 threadCount);
void renderCPU_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t tc = 1);
void renderCPU_Polar_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t tc = 1);

#endif /* FRAC_MULTI_H */