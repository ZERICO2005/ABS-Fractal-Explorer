/*
**	Author: zerico2005 (2023-2024)
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef BUILD_CL_H
#define BUILD_CL_H

#include "fracCL_Internal.h"

#define PROGRAM_FILE "src/render_GPU/fracCL.cl"
#define KERNEL_FUNC "renderFracCLPoint"

//#define BUILD_FRAC_FROM_FILE
#define BUILD_FRAC_FROM_CONST

const char* getOpenCLErrorString(cl_int errorCode);
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

#endif /* BUILD_CL_H */