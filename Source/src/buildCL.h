/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef BUILD_CL_H
#define BUILD_CL_H

#include "gamma.h"

#define PROGRAM_FILE "src/fracCL.cl"
#define KERNEL_FUNC "renderFracCLPoint"
#define CL_TARGET_OPENCL_VERSION 120

#include <CL/cl.h>

//#define BUILD_FRAC_FROM_FILE
#define BUILD_FRAC_FROM_CONST

struct _OpenCL_Engine {
	cl_device_id device;
	cl_context context;
	cl_program program;
	cl_kernel kernel;
	cl_command_queue queue;
}; typedef struct _OpenCL_Engine OpenCL_Engine;

cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

#endif /* BUILD_CL_H */