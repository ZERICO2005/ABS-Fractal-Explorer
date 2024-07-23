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
class Render_Configurator;

int32_t render_OpenCL_ABS_Mandelbrot(
	const BufferBox* buf, Render_Data ren, ABS_Mandelbrot param,
	const Render_Configurator& GPU_Render_Config,
	std::atomic<bool>& ABORT_RENDERING
);

int32_t init_OpenCL();
int32_t terminate_OpenCL();

void query_OpenCL_GPU();

class FNV1A_Hash;
void calculate_GPU_Hardware_Hash(FNV1A_Hash& hash);

struct OpenCL_Engine_Information;

const OpenCL_Engine_Information get_GPU_Information();

#endif /* FRAC_CL_H */