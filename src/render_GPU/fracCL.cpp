/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "fracCL_Internal.h"

/* Temporary */ #include "../render_CPU/frac_Multi_Internal.h"


// #define Enable_OpenCL_Rendering_Partions

#include "fracCL.h"
#include "buildCL.h"
#include "../fractal.h"
#include "../user_data.h"

#include "../fnv1a_hash.hpp"

#include "../floats/double_Float32.hpp"
#include "../floats/double_Float64.hpp"

#include "../render_Configuration.hpp"

/* GPU Information */
	static OpenCL_Engine_Information Public_GPU_Engine;
	std::mutex Public_GPU_Engine_Mutex;

	const OpenCL_Engine_Information get_GPU_Information() {
		std::lock_guard<std::mutex> lock(Public_GPU_Engine_Mutex);
		return Public_GPU_Engine;
	}
	static void set_GPU_Information(
		const OpenCL_Engine& engine,
		const OpenCL_Kernel_Properties& kernel_properties
	) {
		std::lock_guard<std::mutex> lock(Public_GPU_Engine_Mutex);
		Public_GPU_Engine.device_id         = engine.device_id        ;
		Public_GPU_Engine.device_properties = engine.device_properties;
		Public_GPU_Engine.context           = engine.context          ;
		Public_GPU_Engine.command_queue     = engine.command_queue    ;
		Public_GPU_Engine.kernel_properties = kernel_properties       ;
	}


bool initialized_OpenCL = false;

uint32_t compiledYet = 0;

static OpenCL_Engine GPU_Engine;

cl_mem deviceResultBuf = nullptr;

cl_kernel Render_Kernel_Float32   = nullptr;
cl_kernel Render_Kernel_Float32x2 = nullptr;
cl_kernel Render_Kernel_Float64   = nullptr;
cl_kernel Render_Kernel_Float64x2 = nullptr;

void calculate_GPU_Hardware_Hash(FNV1A_Hash& hash) {
	if (initialized_OpenCL == false) { return; }
	const OpenCL_Device_Properties& Device = GPU_Engine.device_properties;
	hash += Device.Maximum_Compute_Units;
	hash += Device.Global_Memory_Size;
	hash += Device.Float32_Config.Flags;
	hash += Device.Float64_Config.Flags;
}

uint8_t printOpenCLError(cl_int errorCode) {
	if (errorCode != 0) {
		const char* errorString = getOpenCLErrorString(errorCode);
		printFlush("OpenCL Error: %" PRId32 " %s\n", errorCode, (errorString != nullptr) ? errorString : "<Unknown>");
		return 1;
	}
	return 0;
}

/* Query OpenCL */

	static void read_DeviceInfo_String(std::string& output_str, const cl_device_info param_name, const cl_device_id device_id) {
		char* temp_str = nullptr;
		size_t temp_size = 0;
		
		if (clGetDeviceInfo(device_id, param_name, 0, nullptr, &temp_size)) {
			output_str = "<error>";
			return;
		}
		
		temp_str = (char*)calloc(temp_size, sizeof(char));
		if (temp_str == nullptr) { output_str = "<error>"; return; }
		
		if (clGetDeviceInfo(device_id, param_name, temp_size, temp_str, nullptr)) {
			output_str = "<error>";
			free(temp_str);
			temp_str = nullptr;
			return;
		}
		output_str.assign(temp_str);
		
		free(temp_str);
		temp_str = nullptr;
	}

	template<typename T>
	static inline void read_DeviceInfo_Value(T& output_val, const cl_device_info param_name, const cl_device_id device_id) {
		clGetDeviceInfo(
			device_id, param_name,
			sizeof(T), &output_val,
			nullptr
		);
	}

	void query_Float_Config(OpenCL_Float_Config& float_config, const uint64_t flags) {
		float_config.Flags = flags;
		float_config.Denormalized                        = flags & CL_FP_DENORM                       ;
		float_config.Infinity_NAN                        = flags & CL_FP_INF_NAN                      ;
		float_config.Round_to_Nearest                    = flags & CL_FP_ROUND_TO_NEAREST             ;
		float_config.Round_to_Zero                       = flags & CL_FP_ROUND_TO_ZERO                ;
		float_config.Round_to_Infinity                   = flags & CL_FP_ROUND_TO_INF                 ;
		float_config.Fused_Multiply_Add                  = flags & CL_FP_FMA                          ;
		float_config.Correctly_Rounded_Divide_SquareRoot = flags & CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT;
		float_config.Soft_Float                          = flags & CL_FP_SOFT_FLOAT                   ;
	}

	void query_OpenCL_Device_Properties(OpenCL_Device_Properties& device_properties, const cl_device_id device_id) {
		read_DeviceInfo_String(device_properties.Name          , CL_DEVICE_NAME   , device_id);
		read_DeviceInfo_String(device_properties.Vendor        , CL_DEVICE_VENDOR , device_id);
		read_DeviceInfo_String(device_properties.Driver_Version, CL_DRIVER_VERSION, device_id);
		read_DeviceInfo_String(device_properties.OpenCL_Version, CL_DEVICE_VERSION, device_id);
		read_DeviceInfo_String(device_properties.Profile       , CL_DEVICE_PROFILE, device_id);

		read_DeviceInfo_Value(device_properties.Maximum_WorkGroup_Size      , CL_DEVICE_MAX_WORK_GROUP_SIZE       , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Compute_Units       , CL_DEVICE_MAX_COMPUTE_UNITS         , device_id);
		read_DeviceInfo_Value(device_properties.Global_Memory_Size          , CL_DEVICE_GLOBAL_MEM_SIZE           , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Memory_Alloc_Size   , CL_DEVICE_MAX_MEM_ALLOC_SIZE        , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Clock_Frequency     , CL_DEVICE_MAX_CLOCK_FREQUENCY       , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Parameter_Size      , CL_DEVICE_MAX_PARAMETER_SIZE        , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Constant_Buffer_Size, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE  , device_id);
		read_DeviceInfo_Value(device_properties.Maximum_Constant_Args       , CL_DEVICE_MAX_CONSTANT_ARGS         , device_id);
		read_DeviceInfo_Value(device_properties.Local_Memory_Size           , CL_DEVICE_LOCAL_MEM_SIZE            , device_id);
		read_DeviceInfo_Value(device_properties.Profiling_Timer_Resolution  , CL_DEVICE_PROFILING_TIMER_RESOLUTION, device_id);
		
		uint64_t Float32_Flags = 0;
		read_DeviceInfo_Value(Float32_Flags, CL_DEVICE_SINGLE_FP_CONFIG, device_id);
		query_Float_Config(device_properties.Float32_Config, Float32_Flags);
		uint64_t Float64_Flags = 0;
		read_DeviceInfo_Value(Float64_Flags, CL_DEVICE_DOUBLE_FP_CONFIG, device_id);
		query_Float_Config(device_properties.Float64_Config, Float64_Flags);
	}

	template<typename T>
	static inline void read_KernelWorkGroupInfo_Value(T& output_val, const cl_device_info param_name, const cl_kernel kernel, const cl_device_id device_id) {
		clGetKernelWorkGroupInfo(
			kernel, device_id, param_name,
			sizeof(T), &output_val,
			nullptr
		);
	}

	void query_OpenCL_Kernel_Properties(OpenCL_Kernel_Properties& kernel_properties, const cl_kernel kernel, const cl_device_id device_id) {
		read_KernelWorkGroupInfo_Value(kernel_properties.WorkGroup_Size                   , CL_KERNEL_WORK_GROUP_SIZE                   , kernel, device_id);
		read_KernelWorkGroupInfo_Value(kernel_properties.Local_Memory_Size                , CL_KERNEL_LOCAL_MEM_SIZE                    , kernel, device_id);
		read_KernelWorkGroupInfo_Value(kernel_properties.Preferred_WorkGroup_Size_Multiple, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, kernel, device_id);
		read_KernelWorkGroupInfo_Value(kernel_properties.Private_Memory_Size              , CL_KERNEL_PRIVATE_MEM_SIZE                  , kernel, device_id);
	}

void query_OpenCL_GPU() {
	#ifndef BUILD_RELEASE
		const OpenCL_Device_Properties& Device = GPU_Engine.device_properties;
		printf("GPU Hardware Information:\n");
		printf("\tDevice Name: %s | %s\n",
			Device.Name.c_str(), Device.Vendor.c_str()
		);
		printf("\tDriver Version: %s | %s | %s\n",
			Device.Driver_Version.c_str(), Device.OpenCL_Version.c_str(), Device.Profile.c_str()
		);
		printf("\tDevice VRAM: %" PRIu64 "MB\n", Device.Global_Memory_Size / 1048576);
		fflush(stdout);
	#endif
}

int32_t terminate_OpenCL() { /* Deallocate resources */
	if (initialized_OpenCL == false) {
		return 0;
	}
	try {
		/* Render Kernels */
			clReleaseKernel(Render_Kernel_Float32  );
			clReleaseKernel(Render_Kernel_Float32x2);
			clReleaseKernel(Render_Kernel_Float64  );
			clReleaseKernel(Render_Kernel_Float64x2);
		clReleaseMemObject(deviceResultBuf);
		clReleaseCommandQueue(GPU_Engine.command_queue);
		clReleaseProgram(GPU_Engine.program);
		clReleaseContext(GPU_Engine.context);
	} catch (...) {
		printFlush("Error: Unable to terminate OpenCL. OpenCL might not be initialized\n");
		return -1;
	}
	initialized_OpenCL = false;
	return 0;
}

int32_t generate_Render_Kernel(cl_kernel& kernel, const char* function_name) {
	cl_int err = 0;
	kernel = clCreateKernel(GPU_Engine.program, function_name, &err); /* Create a kernel */
	if (kernel == nullptr || err != 0) {
		printf("GPU_Engine.kernel Error: %s\n", getOpenCLErrorString(err));
	}
	return err;
}

int32_t init_OpenCL() {
	if (initialized_OpenCL == true) {
		printFlush("Error: OpenCL is already initialized\n");
		return -1;
	}
	cl_int err = 0; // Error Code Flags
	/* OpenCL structures */
	try {
		GPU_Engine.device_id = create_device();
		GPU_Engine.context = clCreateContext(NULL, 1, &GPU_Engine.device_id, NULL, NULL, &err);
		if (GPU_Engine.context == nullptr || err != 0) {
			printf("GPU_Engine.context Error: %s\n", getOpenCLErrorString(err));
		}
		GPU_Engine.program = build_program(GPU_Engine.context, GPU_Engine.device_id, PROGRAM_FILE); /* Build program */
		if (GPU_Engine.program == nullptr || err != 0) {
			printf("GPU_Engine.program Error: %s\n", getOpenCLErrorString(err));
		}
		GPU_Engine.command_queue = clCreateCommandQueue(GPU_Engine.context, GPU_Engine.device_id, 0, &err); /* Create a command queue */
		if (GPU_Engine.command_queue == nullptr || err != 0) {
			printf("GPU_Engine.command_queue Error: %s\n", getOpenCLErrorString(err));
		}
		deviceResultBuf = NULL;
		// Write our data set into the input array in device memory
		//err = clEnqueueWriteBuffer(queue, dreals, CL_TRUE, 0, sizeof(float)*nreals, reals, 0, NULL, NULL);

		err |= generate_Render_Kernel(Render_Kernel_Float32  , "OpenCL_Mandelbrot_Float32"  );
		// err |= generate_Render_Kernel(Render_Kernel_Float32x2, "OpenCL_Mandelbrot_Float32x2");
		err |= generate_Render_Kernel(Render_Kernel_Float64  , "OpenCL_Mandelbrot_Float64"  );
		// err |= generate_Render_Kernel(Render_Kernel_Float64x2, "OpenCL_Mandelbrot_Float64x2");

		query_OpenCL_Device_Properties(GPU_Engine.device_properties, GPU_Engine.device_id);
	} catch(const std::exception& error) {
		printFlush("Error: %s\n",error.what());
		return -1;
	} catch(...) {
		printFlush("Error: Unknown OpenCL Error\n");
		return -1;
	}
	// set_GPU_Information(GPU_Engine);
	initialized_OpenCL = true;
	return 0;
}

//#define printChange(f,x); {static int CHANGE; if (CHANGE != x) { printFlush(f, x); } CHANGE = x;}
#define printErrorChange(f,x); { static int CHANGE = x; if (CHANGE != x) { printFlush(f, x); printOpenCLError(err); } CHANGE = x; }

static void calculate_Global_and_Local_Size(
	size_t& global_work_size, size_t& local_work_size,
	const OpenCL_Kernel_Properties& kernel_properties, size_t pixel_count
) {
	const size_t WorkGroup_Size = kernel_properties.WorkGroup_Size;
	size_t cor = pixel_count % WorkGroup_Size;
	cor = (cor == 0) ? 0 : (WorkGroup_Size - cor);
	local_work_size = WorkGroup_Size;
	global_work_size = pixel_count + cor;
}

template <typename fpX, typename fpColor>
cl_int load_OpenCL_Render(
	cl_kernel& render_kernel,
	const BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param,
	__attribute__((unused)) std::atomic<bool>& ABORT_RENDERING
) {
	
	OpenCL_Kernel_Properties kernel_properties;
	query_OpenCL_Kernel_Properties(kernel_properties, render_kernel, GPU_Engine.device_id);
	set_GPU_Information(GPU_Engine, kernel_properties);
	
	const size_t Buffer_Pixel_Count = (size_t)buf->resX * (size_t)buf->resY;

	/* Fractal Parameters */
		PreCalc_Param<fpX, fpColor> preCalc_Param;
		Generate_PreCalc_Param(
			preCalc_Param,
			buf, ren, param
		);
		uint32_t polarMandelbrotBool = (param.polarMandelbrot == true) ? 1 : 0;
		uint32_t juliaSetBool = (param.juliaSet == true) ? 1 : 0;
		uint32_t formula32 = (uint32_t)param.formula;
		uint32_t GPU_formula = (polarMandelbrotBool << 30) | (juliaSetBool << 29) | formula32;
		fpX fractal_power = (param.polarMandelbrot == true) ? (fpX)param.polarPower : (fpX)param.power;

	/* Debug Values */
		__attribute__((unused)) const uint32_t debug_val_0x00 = 0x00;
		__attribute__((unused)) const uint32_t debug_val_0xFF = 0xFF;
		__attribute__((unused)) const fpX debug_val_0f = (fpX)0.0;
		__attribute__((unused)) const fpX debug_val_1f = (fpX)1.0;

	uint32_t kArg = 0;
	cl_int err = 0;

	err  = clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.realCord      );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.imagCord      );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(uint32_t), &preCalc_Param.maxItr        );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(uint32_t), &preCalc_Param.Image_ResX    );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(uint32_t), &preCalc_Param.Image_ResY    ); 
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.realJulia     );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.imagJulia     );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(uint32_t), &GPU_formula                 );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &fractal_power               );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(uint32_t), &preCalc_Param.sample        );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.rotSin_PC     );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.rotCos_PC     );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.breakoutValue );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.recip_numZ    );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpX)     , &preCalc_Param.neg_recip_numW);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(cl_mem)  , &deviceResultBuf);

	/* Exterior Color */
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_R_Freq_mult_TAU          );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_R_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_R_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_G_Freq_mult_TAU          );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_G_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_G_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_B_Freq_mult_TAU          );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_B_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_B_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Exterior_Alpha);
	/* Interior Color */
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_R_Freq                   );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_R_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_R_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_G_Freq                   );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_G_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_G_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_B_Freq                   );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_B_Phase_mult_TAU         );
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_B_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(render_kernel, kArg++, sizeof(fpColor), &preCalc_Param.Interior_Alpha);

	printErrorChange("KernelArgs: %" PRId32 "\n",err);

	/* Execute Kernel */

	size_t local_work_size = 0, global_work_size = 0;

	#ifdef Enable_OpenCL_Rendering_Partitions
		size_t partitionCount = (ren.GPU_Partitions < resX * resY / 2) ? ren.GPU_Partitions : (resX * resY / 2);
		for (size_t p = 0; p < partitionCount; p++) {
			printfInterval(0.3,"GPU: %s %zu/%zu\n",boolText(ABORT_RENDERING),p,partitionCount);
			if (ABORT_RENDERING == true) {
				printFlush("Aborted GPU partition %zu/%zu\n",p,partitionCount);
				break;
			}
			size_t p0 = ((resX * resY) * p) / partitionCount;
			size_t p1 = ((resX * resY) * (p + 1)) / partitionCount;
			size_t pSize = p1 - p0;
			size_t cor = (pSize) % KernelWorkGroupSize; // Calculates the correction factor to ensure divisibility
			cor = (cor == 0) ? 0 : (KernelWorkGroupSize - cor);
			local_size = KernelWorkGroupSize;
			global_size = pSize + cor; // Number of total work items - localSize must be devisor

			err = clEnqueueNDRangeKernel(GPU_Engine.command_queue, render_kernel, 1, &p0, &global_size, &local_size, 0, NULL, NULL); /* Enqueue kernel */
			printErrorChange("clEnqueueNDRangeKernel: %" PRId32 "\n",err);
		}
		if (ABORT_RENDERING == true) {
			printFlush("Aborted GPU\n");
		} else {
			printFlush("Safe Return\n");
		}
	#else
		calculate_Global_and_Local_Size(global_work_size, local_work_size, kernel_properties, Buffer_Pixel_Count);
		
		err = clEnqueueNDRangeKernel(GPU_Engine.command_queue, render_kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL); /* Enqueue kernel */
		printErrorChange("clEnqueueNDRangeKernel: %" PRId32 "\n", err);
	#endif
	return err;
}

int32_t render_OpenCL_ABS_Mandelbrot(
	const BufferBox* buf, Render_Data ren, ABS_Mandelbrot param,
	const Render_Configurator& GPU_Render_Config,
	std::atomic<bool>& ABORT_RENDERING
) {
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderOpenCL_ABS_Mandelbrot()");
		return -1;
	}
	const size_t Buffer_Size = getBufferBoxSize(buf);

	static dim32_t rX = 0;
	static dim32_t rY = 0;
	if (((rX != buf->resX || rY != buf->resY) && (rX != 0 || rY != 0)) || initialized_OpenCL == false || deviceResultBuf == NULL) {
		// printf("\nr: %" PRId32 " %" PRId32,rX,rY); fflush(stdout);
		cl_mem tempBuf = clCreateBuffer(GPU_Engine.context, CL_MEM_WRITE_ONLY, Buffer_Size, NULL, NULL);
		clReleaseMemObject(deviceResultBuf);
		deviceResultBuf = tempBuf;
	}
	rX = buf->resX;
	rY = buf->resY;
	
	/* Kernel Arguments */
	__attribute__((unused)) cl_int err = 0;
	int_enum GPU_Render_Preset = GPU_Render_Config.get_Render_Preset();
	switch (GPU_Render_Preset) {
		using namespace Rendering_Configuration;
		case Render_Preset_GPU_Float32x2: // TEMP //
		case Render_Preset_GPU_Float32: {
			err = load_OpenCL_Render<fp32, fp32>(
				Render_Kernel_Float32,
				buf, ren, param,
				ABORT_RENDERING
			);
		} break;
		// case Render_Preset_GPU_Float32x2: {
		// 	err = load_OpenCL_Render<fp32x2, fp32>(
		// 		Render_Kernel_Float32x2,
		// 		buf, ren, param,
		// 		ABORT_RENDERING
		// 	);
		// } break;
		case Render_Preset_GPU_Float64x2: // TEMP //
		case Render_Preset_GPU_Float64: {
			err = load_OpenCL_Render<fp64, fp64>(
				Render_Kernel_Float64,
				buf, ren, param,
				ABORT_RENDERING
			);
		} break;
		// case Render_Preset_GPU_Float64x2: {
		// 	err = load_OpenCL_Render<fp64x2, fp64>(
		// 		Render_Kernel_Float32,
		// 		buf, ren, param,
		// 		ABORT_RENDERING
		// 	);
		// } break;
		default: {
			printfInterval(0.5, "Error: Invalid GPU rendering preset: %d\n", GPU_Render_Preset);
		}
	}
	 

	clFinish(GPU_Engine.command_queue); /* Wait for the command queue to get serviced before reading back results */

	clEnqueueReadBuffer(GPU_Engine.command_queue, deviceResultBuf, CL_TRUE, 0, Buffer_Size, buf->vram, 0, NULL, NULL); /* Read the kernel's output */
	
	return 0;
}