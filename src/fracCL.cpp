/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "fracCL.h"
#include "buildCL.h"
#include "fractal.h"
#include "user_data.h"

/*uint32_t getPow2_Step4(uint32_t p) { //Deprecated
   uint32_t n = 16 + (4 * (p % 4));
   p /= 4;
   n <<= p;
   return n;
}*/
bool initialized_OpenCL = false;

uint32_t compiledYet = 0;

OpenCL_Engine engine;
cl_int err; //Error Codes?
size_t local_size, global_size;
//uint8_t* resultBuf;
cl_mem deviceResultBuf;

char* DeviceName;
char* DeviceVendor;
char* DriverVersion;
char* DeviceProfile;
char* DeviceVersion;

cl_uint DeviceMaxComputeUnits;
cl_ulong DeviceMaxWorkGroupSize;
cl_uint DeviceMaxClockFrequency;
cl_ulong DeviceMaxMemAllocSize;
size_t DeviceMaxParameterSize;
cl_ulong DeviceGlobalMemSize;
cl_ulong DeviceMaxConstantBufferSize;
cl_uint DeviceMaxConstantArgs;
cl_ulong DeviceLocalMemSize;
size_t DeviceProfilingTimerResolution;

cl_device_fp_config DeviceFP32Config;
cl_device_fp_config DeviceFP64Config;

size_t KernelWorkGroupSize;
cl_ulong KernelLocalMemSize;
size_t KernelPreferredWorkGroupSizeMultiple;
cl_ulong KernelPrivateMemSize;

int32_t init_OpenCL() {
	if (initialized_OpenCL == true) {
		printFlush("\nError: OpenCL is already initialized");
		return -1;
	}
	/* OpenCL structures */
	//resultBuf = (uint8_t*)malloc(sizeof(uint8_t) * resX * resY * 3); 
	try {
		engine.device = create_device();
		engine.context = clCreateContext(NULL, 1, &engine.device, NULL, NULL, &err);
		engine.program = build_program(engine.context, engine.device, PROGRAM_FILE); /* Build program */
		engine.queue = clCreateCommandQueue(engine.context, engine.device, 0, &err); /* Create a command queue */
		//deviceResultBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, resX*resY*3, NULL, NULL); /* Create data buffer */
		deviceResultBuf = NULL;
		// Write our data set into the input array in device memory
		//err = clEnqueueWriteBuffer(queue, dreals, CL_TRUE, 0, sizeof(float)*nreals, reals, 0, NULL, NULL);
		engine.kernel = clCreateKernel(engine.program, KERNEL_FUNC, &err); /* Create a kernel */
	} catch(const std::exception& error) {
		printFlush("\nError: %s",error.what());
		return -1;
	} catch(...) {
		printFlush("\nError: Unknown OpenCL Error");
		return -1;
	}
	initialized_OpenCL = true;
	return 0;
}

uint8_t printOpenCLError(cl_int errorCode) {
	if (errorCode != 0) {
		printFlush("\nOpenCL Error: %d %s",errorCode,getOpenCLErrorString(errorCode));
        return 1;
	}
    return 0;
}
void printFloatingPointConfig(const char* headerText, uint32_t config) {
	/*
    writefToLog("%s",headerText);
    #define configPrint(x,y); if (config & x) { writefToLog(" | " y); }
    configPrint(CL_FP_DENORM,"CL_FP_DENORM");
    configPrint(CL_FP_INF_NAN,"CL_FP_INF_NAN");
    configPrint(CL_FP_ROUND_TO_NEAREST,"CL_FP_ROUND_TO_NEAREST");
    configPrint(CL_FP_ROUND_TO_ZERO,"CL_FP_ROUND_TO_ZERO");
    configPrint(CL_FP_ROUND_TO_INF,"CL_FP_ROUND_TO_INF");
    configPrint(CL_FP_FMA,"CL_FP_FMA");
    configPrint(CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT,"CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT");
    configPrint(CL_FP_SOFT_FLOAT,"CL_FP_SOFT_FLOAT");
	*/
}
void queryOpenCL_GPU() {
    uint8_t enablePrinting = 0;
    #define printDeviceText(f,inDat,outDat); \
	{ \
		size_t retSize; \
		err = clGetDeviceInfo(engine.device,inDat,0,NULL,&retSize); \
		if (printOpenCLError(err) == 0) { \
			outDat = (char*)malloc(retSize); \
			err = clGetDeviceInfo(engine.device,inDat,retSize,outDat,NULL); \
			if (enablePrinting != 0) { \
				printFlush(f,outDat); \
			} else { \
				writefToLog(f,outDat); \
			} printOpenCLError(err); \
		} \
	}

    #define printDeviceStat(f,in,out) err = clGetDeviceInfo(engine.device,in,sizeof(out),&out,NULL); if (enablePrinting != 0) { printFlush(f,out); } else { writefToLog(f,out); } printOpenCLError(err)
	#define printKernelStat(f,in,out) err = clGetKernelWorkGroupInfo(engine.kernel,engine.device,in,sizeof(out),&out,NULL); if (enablePrinting != 0) { printFlush(f,out); } else { writefToLog(f,out); } printOpenCLError(err)

    printFlush("\nGPU Hardware Information:");
		printDeviceText("\n\tCL_DEVICE_NAME: %s",CL_DEVICE_NAME,DeviceName);
		printDeviceText("\n\tCL_DEVICE_VENDOR: %s",CL_DEVICE_VENDOR,DeviceVendor);
		printDeviceText("\n\tCL_DRIVER_VERSION: %s",CL_DRIVER_VERSION,DriverVersion);
		printDeviceText("\n\tCL_DEVICE_VERSION: %s",CL_DEVICE_VERSION,DeviceVersion);
		printDeviceText("\n\tCL_DEVICE_PROFILE: %s",CL_DEVICE_PROFILE,DeviceProfile);
    writefToLog("\nDevice Properties:");
		printDeviceStat("\n\tCL_DEVICE_MAX_WORK_GROUP_SIZE: %llu",CL_DEVICE_MAX_WORK_GROUP_SIZE,DeviceMaxWorkGroupSize);
		printDeviceStat("\n\tCL_DEVICE_MAX_COMPUTE_UNITS: %u",CL_DEVICE_MAX_COMPUTE_UNITS,DeviceMaxComputeUnits);
		printDeviceStat("\n\tCL_DEVICE_GLOBAL_MEM_SIZE: %llu",CL_DEVICE_GLOBAL_MEM_SIZE,DeviceGlobalMemSize);
		writefToLog(" | %lldMB",(DeviceGlobalMemSize / 1048576));
		printDeviceStat("\n\tCL_DEVICE_MAX_MEM_ALLOC_SIZE: %llu",CL_DEVICE_MAX_MEM_ALLOC_SIZE,DeviceMaxMemAllocSize);
		writefToLog(" | %lldMB",(DeviceMaxMemAllocSize / 1048576));
		printDeviceStat("\n\tCL_DEVICE_MAX_CLOCK_FREQUENCY: %uMhz",CL_DEVICE_MAX_CLOCK_FREQUENCY,DeviceMaxClockFrequency);
		printDeviceStat("\n\tCL_DEVICE_MAX_PARAMETER_SIZE: %llu",CL_DEVICE_MAX_PARAMETER_SIZE,DeviceMaxParameterSize);
		printDeviceStat("\n\tCL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %llu",CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,DeviceMaxConstantBufferSize);
		printDeviceStat("\n\tCL_DEVICE_MAX_CONSTANT_ARGS: %u",CL_DEVICE_MAX_CONSTANT_ARGS,DeviceMaxConstantArgs);
		printDeviceStat("\n\tCL_DEVICE_LOCAL_MEM_SIZE: %llu",CL_DEVICE_LOCAL_MEM_SIZE,DeviceLocalMemSize);
		printDeviceStat("\n\tCL_DEVICE_PROFILING_TIMER_RESOLUTION: %llu",CL_DEVICE_PROFILING_TIMER_RESOLUTION,DeviceProfilingTimerResolution);
    writefToLog("\nFloating Point Support:");
		printDeviceStat("\n\tCL_DEVICE_SINGLE_FP_CONFIG: %llu",CL_DEVICE_SINGLE_FP_CONFIG,DeviceFP32Config);
		printFloatingPointConfig("\n\tFP32", DeviceFP32Config);
		printDeviceStat("\n\tCL_DEVICE_DOUBLE_FP_CONFIG: %llu",CL_DEVICE_DOUBLE_FP_CONFIG ,DeviceFP64Config);
		printFloatingPointConfig("\n\tFP64", DeviceFP64Config);

	if (initialized_OpenCL) {
		writefToLog("\nKernel Properties:");
		printKernelStat("\n\tCL_KERNEL_WORK_GROUP_SIZE: %llu",CL_KERNEL_WORK_GROUP_SIZE,KernelWorkGroupSize);
		printKernelStat("\n\tCL_KERNEL_LOCAL_MEM_SIZE: %llu",CL_KERNEL_LOCAL_MEM_SIZE,KernelLocalMemSize);
		printKernelStat("\n\tCL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %llu",CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,KernelPreferredWorkGroupSizeMultiple);
		printKernelStat("\n\tCL_KERNEL_PRIVATE_MEM_SIZE: %llu",CL_KERNEL_PRIVATE_MEM_SIZE,KernelPrivateMemSize);
	}


	printf("\n\tDevice Name: %s | %s",DeviceName,DeviceVendor); fflush(stdout);
	printf("\n\tDriver Version: %s | %s | %s",DriverVersion,DeviceVersion,DeviceProfile); fflush(stdout);
	printf("\n\tDevice VRAM: %lluMB",(DeviceGlobalMemSize / 1048576)); fflush(stdout);
}
int32_t terminate_OpenCL() { 	/* Deallocate resources */
	if (initialized_OpenCL == false) {
		return 0;
	}
	try {
		clReleaseKernel(engine.kernel);
		clReleaseMemObject(deviceResultBuf);
		clReleaseCommandQueue(engine.queue);
		clReleaseProgram(engine.program);
		clReleaseContext(engine.context);
	} catch (...) {
		printFlush("\nError: Unable to terminate OpenCL. OpenCL might not be initialized");
		return -1;
	}
	initialized_OpenCL = false;
	return 0;
}

u32 rX = 0;
u32 rY = 0;
//#define printChange(f,x); {static int CHANGE; if (CHANGE != x) { printFlush(f, x); } CHANGE = x;}
#define printErrorChange(f,x); { static int CHANGE; if (CHANGE != x) { printFlush(f, x); printOpenCLError(err); } CHANGE = x; }

int32_t renderOpenCL_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING) {
	uint64_t clearBufferStart = getNanoTime();
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderOpenCL_ABS_Mandelbrot()");
		return -1;
	}
	uint32_t resX = buf->resX;
    uint32_t resY = buf->resY;
	if (((rX != buf->resX || rY != buf->resY) && (rX != 0 || rY != 0)) || initialized_OpenCL == false || deviceResultBuf == NULL) {
		// clReleaseMemObject(deviceResultBuf);
		// deviceResultBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, resX*resY*3, NULL, NULL); /* Create data buffer */
		// printf("\nr: %d %d",rX,rY); fflush(stdout);
		// printf("\nr: %d %d",rX,rY); fflush(stdout);
		cl_mem tempBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, getBufferBoxSize(buf), NULL, NULL);
		clReleaseMemObject(deviceResultBuf);
		deviceResultBuf = tempBuf;
	}
	rX = buf->resX;
	rY = buf->resY;

    // If it ends with 'P' then it likely means the variable is a precalculated constant

	fp32 r = (fp32)param.r;
	fp32 i = (fp32)param.i;
	uint32_t maxItr = (uint32_t)param.maxItr;
	fp32 zr0 = (param.startingZ == false && param.juliaSet == false) ? 0.0f : (fp32)param.zr;
	fp32 zi0 = (param.startingZ == false && param.juliaSet == false) ? 0.0f : (fp32)param.zi;
	uint32_t polarMandelbrotBool = (param.polarMandelbrot == true) ? 1 : 0;
	uint32_t juliaSetBool = (param.juliaSet == true) ? 1 : 0;
	uint32_t formula32 = (uint32_t)param.formula;
	uint32_t formula = (polarMandelbrotBool << 30) | (juliaSetBool << 29) | formula32;
	fp32 power = (param.polarMandelbrot == true) ? (fp32)param.polarPower : (fp32)param.power;
    uint32_t sample = (uint32_t)ren.sample;
    fp32 rot = (fp32)param.rot;
	fp32 numZ;
	fp32 numW;
	{
		fp64 T_zoomP = pow(10.0, param.zoom);
		fp64 T_numX = ((fp64)((resX * sample) - 1) / 2.0);
		fp64 T_numY = ((fp64)((resY * sample) - 1) / 2.0);
		fp64 T_num0 = (resX >= resY) ? T_numY * T_zoomP : T_numX * T_zoomP; // 0 looks like Theta lol
		numZ = (fp32)(T_num0 / param.sX);
		numW = (fp32)(T_num0 / param.sY);
	}
	fp32 sX = (fp32)param.sX;
    fp32 sY = (fp32)param.sY;
	err = clSetKernelArg(engine.kernel, 0, sizeof(fp32), &r);
	err |= clSetKernelArg(engine.kernel, 1, sizeof(fp32), &i);
	err |= clSetKernelArg(engine.kernel, 2, sizeof(uint32_t), &maxItr);
	err |= clSetKernelArg(engine.kernel, 3, sizeof(uint32_t), &resX);
	err |= clSetKernelArg(engine.kernel, 4, sizeof(uint32_t), &resY); 
	err |= clSetKernelArg(engine.kernel, 5, sizeof(fp32), &zr0);
	err |= clSetKernelArg(engine.kernel, 6, sizeof(fp32), &zi0);
	err |= clSetKernelArg(engine.kernel, 7, sizeof(uint32_t), &formula);
	err |= clSetKernelArg(engine.kernel, 8, sizeof(fp32), &power);
    err |= clSetKernelArg(engine.kernel, 9, sizeof(uint32_t), &sample);
    err |= clSetKernelArg(engine.kernel, 10, sizeof(fp32), &rot);
	err |= clSetKernelArg(engine.kernel, 11, sizeof(fp32), &numZ);
	err |= clSetKernelArg(engine.kernel, 12, sizeof(fp32), &numW);
	err |= clSetKernelArg(engine.kernel, 13, sizeof(cl_mem), &deviceResultBuf);
	printErrorChange("\nKernelArgs: %d",err);

	uint32_t cor = (resX * resY) % KernelWorkGroupSize; // Calculates the correction factor to ensure divisibility
	cor = (cor == 0) ? 0 : (KernelWorkGroupSize - cor);
	local_size = KernelWorkGroupSize;
	global_size = resX * resY + cor; // Number of total work items - localSize must be devisor
	err = clEnqueueNDRangeKernel(engine.queue, engine.kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL); /* Enqueue kernel */
	printErrorChange("\nclEnqueueNDRangeKernel: %d",err);
	
	clFinish(engine.queue); /* Wait for the command queue to get serviced before reading back results */
	clEnqueueReadBuffer(engine.queue, deviceResultBuf, CL_TRUE, 0, getBufferBoxSize(buf), buf->vram, 0, NULL, NULL); /* Read the kernel's output */
	//for (u32 z = 0; z < resX * resY * 3; z++) { data[z] = resultBuf[z]; }
	
	uint64_t clearBufferFinish = getNanoTime() - clearBufferStart;
	static uint64_t sumTime = 0;
	static uint64_t runCount = 0;
	sumTime += clearBufferFinish;
	runCount++;
	fp64 aver = NANO_TO_SECONDS(sumTime) * 1000.0 / (fp64)runCount;
	printfInterval(
		0.3,"\n%5llu: %6.2lfms aver: %6.2lfms",
		runCount,
		NANO_TO_SECONDS(clearBufferFinish) * 1000.0,
		aver
	);
	
	return 0;
}

// int32_t renderFormula_OpenCL(ABS_Fractal* param, ImageBuffer* buf) {
//     uint32_t resX = buf->resX;
//     uint32_t resY = buf->resY;
// 	if (((rX != buf->resX || rY != buf->resY) && (rX != 0 || rY != 0)) || initialized_OpenCL == false) {
// 		// clReleaseMemObject(deviceResultBuf);
// 		// deviceResultBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, resX*resY*3, NULL, NULL); /* Create data buffer */
// 		// printf("\nr: %d %d",rX,rY); fflush(stdout);
// 		// printf("\nr: %d %d",rX,rY); fflush(stdout);
// 		cl_mem tempBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, buf->resX * buf->resY * 3, NULL, NULL);
// 		clReleaseMemObject(deviceResultBuf);
// 		deviceResultBuf = tempBuf;
// 	}
// 	rX = buf->resX;
// 	rY = buf->resY;

//     // If it ends with 'P' then it likely means the variable is a precalculated constant

// 	fp32 r = (fp32)param->r;
// 	fp32 i = (fp32)param->i;
	
// 	uint32_t itr = (uint32_t)param->itr;
// 	fp32 zr0 = (param->julia == 0x2) ? 0.0f : (fp32)param->zr;
// 	fp32 zi0 = (param->julia == 0x2) ? 0.0f : (fp32)param->zi;
// 	uint32_t formula = (((uint32_t)param->type - 2) << 30) | ((uint32_t)(param->julia & 0x1) << 29) | (uint32_t)param->formula;
//     uint32_t sample = (uint32_t)param->sample;
//     fp32 rot = (fp32)param->rot;
// 	fp32 numZ;
// 	fp32 numW;
// 	{
// 		fp64 T_zoomP = pow(10.0, param->zoom);
// 		fp64 T_numX = ((fp64)((resX * sample) - 1) / 2.0);
// 		fp64 T_numY = ((fp64)((resY * sample) - 1) / 2.0);
// 		fp64 T_num0 = (resX >= resY) ? T_numY * T_zoomP : T_numX * T_zoomP; // 0 looks like Theta lol
// 		numZ = (fp32)(T_num0 / param->sX);
// 		numW = (fp32)(T_num0 / param->sY);
// 	}

//     fp32 sX = (fp32)param->sX;
//     fp32 sY = (fp32)param->sY;
// 	/* Create kernel arguments */
// 	err = clSetKernelArg(engine.kernel, 0, sizeof(fp32), &r);
// 	err |= clSetKernelArg(engine.kernel, 1, sizeof(fp32), &i);
// 	err |= clSetKernelArg(engine.kernel, 2, sizeof(uint32_t), &itr);
// 	err |= clSetKernelArg(engine.kernel, 3, sizeof(uint32_t), &resX);
// 	err |= clSetKernelArg(engine.kernel, 4, sizeof(uint32_t), &resY); 
// 	err |= clSetKernelArg(engine.kernel, 5, sizeof(fp32), &zr0);
// 	err |= clSetKernelArg(engine.kernel, 6, sizeof(fp32), &zi0);
// 	err |= clSetKernelArg(engine.kernel, 7, sizeof(uint32_t), &formula);
//     err |= clSetKernelArg(engine.kernel, 8, sizeof(uint32_t), &sample);
//     err |= clSetKernelArg(engine.kernel, 9, sizeof(fp32), &rot);
// 	err |= clSetKernelArg(engine.kernel, 10, sizeof(fp32), &numZ);
// 	err |= clSetKernelArg(engine.kernel, 11, sizeof(fp32), &numW);
// 	err |= clSetKernelArg(engine.kernel, 12, sizeof(cl_mem), &deviceResultBuf);
// 	printErrorChange("\nKernelArgs: %d",err);

// 	uint32_t cor = (resX * resY) % KernelWorkGroupSize; // Calculates the correction factor to ensure divisibility
// 	cor = (cor == 0) ? 0 : (KernelWorkGroupSize - cor);
// 	local_size = KernelWorkGroupSize;
// 	global_size = resX * resY + cor; // Number of total work items - localSize must be devisor
// 	err = clEnqueueNDRangeKernel(engine.queue, engine.kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL); /* Enqueue kernel */
// 	printErrorChange("\nclEnqueueNDRangeKernel: %d",err);
	
// 	clFinish(engine.queue); /* Wait for the command queue to get serviced before reading back results */
// 	clEnqueueReadBuffer(engine.queue, deviceResultBuf, CL_TRUE, 0, sizeof(uint8_t) * buf->resX * buf->resY * 3, buf->vram, 0, NULL, NULL); /* Read the kernel's output */
// 	//for (u32 z = 0; z < resX * resY * 3; z++) { data[z] = resultBuf[z]; }
// 	return 0;
// }
