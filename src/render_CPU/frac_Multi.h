/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_H
#define FRAC_MULTI_H

#include <cstddef>
#include <cstdint>
#include <atomic>

struct BufferBox;

struct Render_Data;
struct ABS_Mandelbrot;

void renderCPU_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount = 1);

#endif /* FRAC_MULTI_H */