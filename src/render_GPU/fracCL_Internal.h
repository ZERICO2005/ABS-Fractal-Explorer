/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_CL_INTERNAL_H
#define FRAC_CL_INTERNAL_H

#include "../Common_Def.h"
#include "../Program_Def.h"
#include "../copyBuffer.h"

#define CL_TARGET_OPENCL_VERSION 120

#include <CL/cl.h>

struct OpenCL_Engine {
	cl_device_id device;
	cl_context context;
	cl_program program;
	cl_kernel kernel;
	cl_command_queue queue;
};

struct OpenCL_Device_Properties {
	const char* Name;
	const char* Vendor;
	const char* Driver_Version;
	const char* Profile;
	const char* OpenCL_Version;
	uint64_t Maximum_WorkGroup_Size;
	uint32_t Maximum_Compute_Units;
	uint64_t Global_Memory_Size;
	uint64_t Maximum_Memory_Alloc_Size;
	uint32_t Maximum_Clock_Frequency;
	uint64_t Maximum_Parameter_Size;
	uint64_t Maximum_Constant_Buffer_Size;
	uint32_t Maximum_Constant_Args;
	uint64_t Local_Memory_Size;
	uint64_t Profiling_Timer_Resolution;
	uint64_t Float32_Config;
	uint64_t Float64_Config;
};

struct OpenCL_Kernel_Properties {
	uint64_t WorkGroup_Size;
	uint64_t Local_Memory_Size;
	uint64_t Preferred_WorkGroup_Size_Multiple;
	uint64_t Private_Memory_Size;
};

/* Legacy code */
	#define writefToLog(...) //printFlush(__VA_ARGS__)

#endif /* FRAC_CL_INTERNAL_H */