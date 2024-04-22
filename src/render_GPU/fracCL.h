/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_CL_H
#define FRAC_CL_H

#include <cstddef>
#include <cstdint>
#include <atomic>

struct BufferBox;
struct Render_Data;
struct ABS_Mandelbrot;

int32_t renderOpenCL_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING);

int32_t init_OpenCL();
int32_t terminate_OpenCL();
void queryOpenCL_GPU();

void calculate_GPU_Hardware_Hash(uint64_t& hash);

#endif /* FRAC_CL_H */