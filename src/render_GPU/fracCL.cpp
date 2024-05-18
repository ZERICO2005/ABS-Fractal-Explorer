/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "fracCL_Internal.h"

/* Temporary */ #include "../render_CPU/frac_Multi_Internal.h"

#include "fracCL.h"
#include "buildCL.h"
#include "../fractal.h"
#include "../user_data.h"

/* GPU Information */
	static OpenCL_Engine Public_GPU_Engine;
	std::mutex Public_GPU_Engine_Mutex;

	const OpenCL_Engine get_GPU_Information() {
		std::lock_guard<std::mutex> lock(Public_GPU_Engine_Mutex);
		return Public_GPU_Engine;
	}
	static void set_GPU_Information(const OpenCL_Engine& engine) {
		std::lock_guard<std::mutex> lock(Public_GPU_Engine_Mutex);
		Public_GPU_Engine = engine;
	}


bool initialized_OpenCL = false;

uint32_t compiledYet = 0;

static OpenCL_Engine GPU_Engine;

cl_int err; // Error Code Flags

cl_mem deviceResultBuf = nullptr;

void calculate_GPU_Hardware_Hash(uint64_t& hash) {
	if (initialized_OpenCL == false) { return; }
	const OpenCL_Device_Properties& Device = GPU_Engine.device_properties;
	fnv1a_hash_continous(hash, (uint8_t*)(void*)&
		Device.Maximum_Compute_Units, sizeof(Device.Maximum_Compute_Units)
	);
	fnv1a_hash_continous(hash, (uint8_t*)(void*)&
		Device.Global_Memory_Size, sizeof(Device.Global_Memory_Size)
	);
	fnv1a_hash_continous(hash, (uint8_t*)(void*)&
		Device.Float32_Config.Flags, sizeof(Device.Float32_Config.Flags)
	);
	fnv1a_hash_continous(hash, (uint8_t*)(void*)&
		Device.Float64_Config.Flags, sizeof(Device.Float64_Config.Flags)
	);
}

uint8_t printOpenCLError(cl_int errorCode) {
	if (errorCode != 0) {
		printFlush("\nOpenCL Error: %" PRId32 " %s", errorCode, getOpenCLErrorString(errorCode));
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
	const OpenCL_Device_Properties& Device = GPU_Engine.device_properties;
	printf("\nGPU Hardware Information:");
	printf("\n\tDevice Name: %s | %s",
		Device.Name.c_str(), Device.Vendor.c_str()
	);
	printf("\n\tDriver Version: %s | %s | %s",
		Device.Driver_Version.c_str(), Device.OpenCL_Version.c_str(), Device.Profile.c_str()
	);
	printf("\n\tDevice VRAM: %" PRIu64 "MB", Device.Global_Memory_Size / 1048576);
	fflush(stdout);
}

int32_t terminate_OpenCL() { /* Deallocate resources */
	if (initialized_OpenCL == false) {
		return 0;
	}
	try {
		clReleaseKernel(GPU_Engine.kernel);
		clReleaseMemObject(deviceResultBuf);
		clReleaseCommandQueue(GPU_Engine.command_queue);
		clReleaseProgram(GPU_Engine.program);
		clReleaseContext(GPU_Engine.context);
	} catch (...) {
		printFlush("\nError: Unable to terminate OpenCL. OpenCL might not be initialized");
		return -1;
	}
	initialized_OpenCL = false;
	return 0;
}

int32_t init_OpenCL() {
	if (initialized_OpenCL == true) {
		printFlush("\nError: OpenCL is already initialized");
		return -1;
	}
	/* OpenCL structures */
	try {
		GPU_Engine.device_id = create_device();
		GPU_Engine.context = clCreateContext(NULL, 1, &GPU_Engine.device_id, NULL, NULL, &err);
		GPU_Engine.program = build_program(GPU_Engine.context, GPU_Engine.device_id, PROGRAM_FILE); /* Build program */
		GPU_Engine.command_queue = clCreateCommandQueue(GPU_Engine.context, GPU_Engine.device_id, 0, &err); /* Create a command queue */
		deviceResultBuf = NULL;
		// Write our data set into the input array in device memory
		//err = clEnqueueWriteBuffer(queue, dreals, CL_TRUE, 0, sizeof(float)*nreals, reals, 0, NULL, NULL);
		GPU_Engine.kernel = clCreateKernel(GPU_Engine.program, KERNEL_FUNC, &err); /* Create a kernel */
		query_OpenCL_Device_Properties(GPU_Engine.device_properties, GPU_Engine.device_id);
		query_OpenCL_Kernel_Properties(GPU_Engine.kernel_properties, GPU_Engine.kernel, GPU_Engine.device_id);
	} catch(const std::exception& error) {
		printFlush("\nError: %s",error.what());
		return -1;
	} catch(...) {
		printFlush("\nError: Unknown OpenCL Error");
		return -1;
	}
	set_GPU_Information(GPU_Engine);
	initialized_OpenCL = true;
	return 0;
}

//#define printChange(f,x); {static int CHANGE; if (CHANGE != x) { printFlush(f, x); } CHANGE = x;}
#define printErrorChange(f,x); { static int CHANGE = x; if (CHANGE != x) { printFlush(f, x); printOpenCLError(err); } CHANGE = x; }

static void calculate_Global_and_Local_Size(
	size_t& global_work_size, size_t& local_work_size,
	const OpenCL_Engine& engine, size_t pixel_count
) {
	const size_t WorkGroup_Size = engine.kernel_properties.WorkGroup_Size;
	size_t cor = pixel_count % WorkGroup_Size;
	cor = (cor == 0) ? 0 : (WorkGroup_Size - cor);
	local_work_size = WorkGroup_Size;
	global_work_size = pixel_count + cor;
}

int32_t render_OpenCL_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING) {
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderOpenCL_ABS_Mandelbrot()");
		return -1;
	}
	const size_t Buffer_Size = getBufferBoxSize(buf);
	const size_t Buffer_Pixel_Count = (size_t)buf->resX * (size_t)buf->resY;

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

	/* Fractal Parameters */
		PreCalc_Param<fp32, fp32> preCalc_Param;
		Generate_PreCalc_Param(
			preCalc_Param,
			buf, ren, param
		);
		uint32_t polarMandelbrotBool = (param.polarMandelbrot == true) ? 1 : 0;
		uint32_t juliaSetBool = (param.juliaSet == true) ? 1 : 0;
		uint32_t formula32 = (uint32_t)param.formula;
		uint32_t GPU_formula = (polarMandelbrotBool << 30) | (juliaSetBool << 29) | formula32;
		fp32 fractal_power = (param.polarMandelbrot == true) ? (fp32)param.polarPower : (fp32)param.power;

	/* Debug Values */
		__attribute__((unused)) const uint32_t debug_val_0x00 = 0x00;
		__attribute__((unused)) const uint32_t debug_val_0xFF = 0xFF;
		__attribute__((unused)) const fp32 debug_val_0f = 0.0f;
		__attribute__((unused)) const fp32 debug_val_1f = 1.0f;
	
	/* Kernel Arguments */
	
	uint32_t kArg = 0;

	err  = clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.realCord      );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.imagCord      );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(uint32_t), &preCalc_Param.maxItr        );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(uint32_t), &preCalc_Param.Image_ResX    );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(uint32_t), &preCalc_Param.Image_ResY    ); 
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.realJulia     );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.imagJulia     );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(uint32_t), &GPU_formula                 );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &fractal_power               );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(uint32_t), &preCalc_Param.sample        );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.rotSin_PC     );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.rotCos_PC     );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.breakoutValue );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.recip_numZ    );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32)    , &preCalc_Param.neg_recip_numW);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(cl_mem)  , &deviceResultBuf);

	/* Exterior Color */
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_R_Freq_mult_TAU          );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_R_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_R_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_G_Freq_mult_TAU          );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_G_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_G_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_B_Freq_mult_TAU          );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_B_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_B_Amp_mult_Exterior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Exterior_Alpha);
	/* Interior Color */
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_R_Freq                   );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_R_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_R_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_G_Freq                   );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_G_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_G_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_B_Freq                   );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_B_Phase_mult_TAU         );
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_B_Amp_mult_Interior_Alpha);
	err |= clSetKernelArg(GPU_Engine.kernel, kArg++, sizeof(fp32), &preCalc_Param.Interior_Alpha);

	printErrorChange("\nKernelArgs: %" PRId32,err);

	/* Execute Kernel */

	size_t local_work_size = 0, global_work_size = 0;

	// size_t partitionCount = (ren.GPU_Partitions < resX * resY / 2) ? ren.GPU_Partitions : (resX * resY / 2);
	// for (size_t p = 0; p < partitionCount; p++) {
	// 	printfInterval(0.3,"\nGPU: %s %zu/%zu",boolText(ABORT_RENDERING),p,partitionCount);
	// 	if (ABORT_RENDERING == true) {
	// 		printFlush("\nAborted GPU partition %zu/%zu",p,partitionCount);
	// 		break;
	// 	}
	// 	size_t p0 = ((resX * resY) * p) / partitionCount;
	// 	size_t p1 = ((resX * resY) * (p + 1)) / partitionCount;
	// 	size_t pSize = p1 - p0;
	// 	size_t cor = (pSize) % KernelWorkGroupSize; // Calculates the correction factor to ensure divisibility
	// 	cor = (cor == 0) ? 0 : (KernelWorkGroupSize - cor);
	// 	local_size = KernelWorkGroupSize;
	// 	global_size = pSize + cor; // Number of total work items - localSize must be devisor

	// 	err = clEnqueueNDRangeKernel(GPU_Engine.command_queue, GPU_Engine.kernel, 1, &p0, &global_size, &local_size, 0, NULL, NULL); /* Enqueue kernel */
	// 	printErrorChange("\nclEnqueueNDRangeKernel: %" PRId32,err);
	// }
	// if (ABORT_RENDERING == true) {
	// 	printFlush("\nAborted GPU");
	// } else {
	// 	printFlush("\nSafe Return");
	// }
	
	calculate_Global_and_Local_Size(global_work_size, local_work_size, GPU_Engine, Buffer_Pixel_Count);
	
	err = clEnqueueNDRangeKernel(GPU_Engine.command_queue, GPU_Engine.kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL); /* Enqueue kernel */
	printErrorChange("\nclEnqueueNDRangeKernel: %" PRId32, err);

	clFinish(GPU_Engine.command_queue); /* Wait for the command queue to get serviced before reading back results */

	clEnqueueReadBuffer(GPU_Engine.command_queue, deviceResultBuf, CL_TRUE, 0, Buffer_Size, buf->vram, 0, NULL, NULL); /* Read the kernel's output */
	
	return 0;
}