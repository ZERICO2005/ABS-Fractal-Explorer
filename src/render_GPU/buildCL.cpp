/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "fracCL_Internal.h"

#include "buildCL.h"

const char FractalOpenCL_SRC[] = 
	#include "fracCL.cl"
;

static_assert(ARRAY_LENGTH(FractalOpenCL_SRC) >= 50, "Warning: FractalOpenCL_SRC is empty (Less than 50 characters)");

const char* getOpenCLErrorString(cl_int errorCode) {
switch(errorCode){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "CL_UNKNOWN_ERROR";
    }
}

cl_device_id create_device() {
	cl_platform_id platform;
	cl_device_id dev;
	int32_t err;

	err = clGetPlatformIDs(1, &platform, NULL);
	if(err < 0) {
		//perror("Unable to identify platform");
		throw "Unable to identify platform";
	} 

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL); // GPU
	if(err == CL_DEVICE_NOT_FOUND) { 
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL); // CPU
	}
	if(err < 0) {
		//perror("Unable to access devices");
		throw "Unable to access devices";
	}
	return dev;
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {
	
	#ifdef BUILD_FRAC_FROM_FILE
		cl_program program;
		FILE *program_handle;
		char *program_buffer, *program_log;
		size_t program_size, log_size;
		int32_t err;

		program_handle = fopen(filename, "r");
		if(program_handle == NULL) {
			//perror("Unable to find the program file");
			throw "Unable to find the program file";
		}
		fseek(program_handle, 0, SEEK_END);
		program_size = ftell(program_handle);
		rewind(program_handle);
		program_buffer = (char*)malloc(program_size + 1);
		program_buffer[program_size] = '\0';
		fread(program_buffer, sizeof(char), program_size, program_handle);
		fclose(program_handle);


		program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, &program_size, &err);
		if(err < 0) {
			//perror("Unable to create program");
			throw "Unable to create program";
		}
		free(program_buffer);

		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if(err < 0) { //Prints error log
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			program_log = (char*) malloc(log_size + 1);
			program_log[log_size] = '\0';
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
			printf("%s\n", program_log);
			free(program_log);
			throw "Error occured when building";
		}
	#endif
	#ifdef BUILD_FRAC_FROM_CONST
		cl_program program;
		// FILE *program_handle;
		char *program_log;
		size_t program_size, log_size;
		int err;
		static const char* Source_Code_List[] = {FractalOpenCL_SRC};
		program_size = strlen(FractalOpenCL_SRC);
		program = clCreateProgramWithSource(ctx, ARRAY_LENGTH(Source_Code_List), Source_Code_List, &program_size, &err);
		if(err < 0) {
			//perror("Unable to create program");
			throw "Unable to create program";
		}
		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if(err < 0) { //Prints error log
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			program_log = (char*) malloc(log_size + 1);
			program_log[log_size] = '\0';
			clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
			printf("%s\n", program_log);
			free(program_log);
			throw "Error occured when building";
		}
	#endif
	return program;
}