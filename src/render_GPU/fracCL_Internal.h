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

struct OpenCL_Float_Config {
	uint64_t Flags;
	bool Denormalized;
	bool Infinity_NAN;
	bool Round_to_Nearest;
	bool Round_to_Zero;
	bool Round_to_Infinity;
	bool Fused_Multiply_Add;
	bool Correctly_Rounded_Divide_SquareRoot;
	bool Soft_Float;
};

struct OpenCL_Device_Properties {
	std::string Name;
	std::string Vendor;
	std::string Driver_Version;
	std::string Profile;
	std::string OpenCL_Version;
	size_t   Maximum_WorkGroup_Size;
	uint32_t Maximum_Compute_Units;
	uint64_t Global_Memory_Size;
	uint64_t Maximum_Memory_Alloc_Size;
	uint32_t Maximum_Clock_Frequency;
	size_t   Maximum_Parameter_Size;
	uint64_t Maximum_Constant_Buffer_Size;
	uint32_t Maximum_Constant_Args;
	uint64_t Local_Memory_Size;
	size_t   Profiling_Timer_Resolution;
	OpenCL_Float_Config Float32_Config;
	OpenCL_Float_Config Float64_Config;
};

struct OpenCL_Kernel_Properties {
	uint64_t WorkGroup_Size;
	uint64_t Local_Memory_Size;
	size_t   Preferred_WorkGroup_Size_Multiple;
	uint64_t Private_Memory_Size;
};

struct OpenCL_Engine {
	cl_device_id device_id;
	OpenCL_Device_Properties device_properties;
	cl_context context;
	cl_program program;
	cl_command_queue command_queue;
};

struct OpenCL_Engine_Information {
	cl_device_id device_id;
	OpenCL_Device_Properties device_properties;
	cl_context context;
	cl_program program;
	OpenCL_Kernel_Properties kernel_properties;
	cl_command_queue command_queue;
};

/* Legacy code */
	#define writefToLog(...) //printFlush(__VA_ARGS__)

#endif /* FRAC_CL_INTERNAL_H */