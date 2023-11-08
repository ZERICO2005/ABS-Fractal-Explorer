#include "gamma.h"
#include "fracCL.h"
#include "buildCL.h"

/*uint32_t getPow2_Step4(uint32_t p) { //Deprecated
   uint32_t n = 16 + (4 * (p % 4));
   p /= 4;
   n <<= p;
   return n;
}*/
uint32_t compiledYet = 0;

OpenCL_Engine engine;
cl_int err; //Error Codes?
size_t local_size, global_size;
//uint8_t* resultBuf;
cl_mem deviceResultBuf;

size_t WorkGroup_Size;
cl_ulong LocalMem_Size;
size_t WorkGroup_SizeMultiple;
cl_ulong PrivateMem_Size;

void init_OpenCL(uint32_t resX, uint32_t resY) {
	/* OpenCL structures */
	//resultBuf = (uint8_t*)malloc(sizeof(uint8_t) * resX * resY * 3); 
	engine.device = create_device();
	engine.context = clCreateContext(NULL, 1, &engine.device, NULL, NULL, &err);

	engine.program = build_program(engine.context, engine.device, PROGRAM_FILE); /* Build program */
	engine.queue = clCreateCommandQueue(engine.context, engine.device, 0, &err); /* Create a command queue */
	deviceResultBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, resX*resY*3, NULL, NULL); /* Create data buffer */
	// Write our data set into the input array in device memory
	//err = clEnqueueWriteBuffer(queue, dreals, CL_TRUE, 0, sizeof(float)*nreals, reals, 0, NULL, NULL);
	engine.kernel = clCreateKernel(engine.program, KERNEL_FUNC, &err); /* Create a kernel */
}
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
    default: return "Unknown OpenCL error";
    }
}
void printOpenCLError(cl_int errorCode) {
	if (errorCode != 0) {
		printf("\nOpenCL Error: %d %s",errorCode,getOpenCLErrorString(errorCode)); fflush(stdout);
	}
}
void queryOpenCL_GPU() {
	//size_t work_group_size;
	//err = clGetKernelWorkGroupInfo(engine.kernel,engine.device,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&work_group_size,NULL); printf("\nCL_KERNEL_WORK_GROUP_SIZE: %d",work_group_size); fflush(stdout); printOpenCLError(err);
	#define printGPUstat(f,in,out) err = clGetKernelWorkGroupInfo(engine.kernel,engine.device,in,sizeof(out),&out,NULL); printf(f,out); fflush(stdout); printOpenCLError(err)
	printf("\nGPU Hardware Capablilities:"); fflush(stdout);
	printGPUstat("\nCL_KERNEL_WORK_GROUP_SIZE: %d",CL_KERNEL_WORK_GROUP_SIZE,WorkGroup_Size);
	printGPUstat("\nCL_KERNEL_LOCAL_MEM_SIZE: %d",CL_KERNEL_LOCAL_MEM_SIZE,LocalMem_Size);
	printGPUstat("\nCL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %d",CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,WorkGroup_SizeMultiple);
	printGPUstat("\nCL_KERNEL_PRIVATE_MEM_SIZE: %d",CL_KERNEL_PRIVATE_MEM_SIZE,PrivateMem_Size);
}
void terminate_OpenCL() { 	/* Deallocate resources */
	clReleaseKernel(engine.kernel);
	clReleaseMemObject(deviceResultBuf);
	clReleaseCommandQueue(engine.queue);
	clReleaseProgram(engine.program);
	clReleaseContext(engine.context);
}

u32 rX = 0;
u32 rY = 0;
#define printChange(f,x); {static int CHANGE; if (CHANGE != x) { printf(f, x); fflush(stdout); } CHANGE = x;}
void renderFormula_OpenCL(frac* param, uint8_t* data, uint32_t resX, uint32_t resY) {
	if ((rX != resX || rY != resY) && (rX != 0 || rY != 0)) {
		// clReleaseMemObject(deviceResultBuf);
		// deviceResultBuf = clCreateBuffer(engine.context, CL_MEM_WRITE_ONLY, resX*resY*3, NULL, NULL); /* Create data buffer */
		// printf("\nr: %d %d",rX,rY); fflush(stdout);
		// printf("\nr: %d %d",rX,rY); fflush(stdout);
		terminate_OpenCL();
		init_OpenCL(resX,resY);
	}
	rX = resX;
	rY = resY;

	fp32 r = (fp32)param->r;
	fp32 i = (fp32)param->i;
	fp32 zoom = (fp32)param->zoom;
	uint32_t itr = (uint32_t)param->itr;
	fp32 zr0 = (param->julia == 0x2) ? 0.0f : (fp32)param->zr;
	fp32 zi0 = (param->julia == 0x2) ? 0.0f : (fp32)param->zi;
	uint32_t formula = (((uint32_t)param->type - 2) << 30) | ((uint32_t)(param->julia & 0x1) << 29) | (uint32_t)param->formula;
	/* Create kernel arguments */
	err = clSetKernelArg(engine.kernel, 0, sizeof(fp32), &r);
	err |= clSetKernelArg(engine.kernel, 1, sizeof(fp32), &i);
	err |= clSetKernelArg(engine.kernel, 2, sizeof(fp32), &zoom);
	err |= clSetKernelArg(engine.kernel, 3, sizeof(uint32_t), &itr);
	err |= clSetKernelArg(engine.kernel, 4, sizeof(uint32_t), &resX);
	err |= clSetKernelArg(engine.kernel, 5, sizeof(uint32_t), &resY); 
	err |= clSetKernelArg(engine.kernel, 6, sizeof(fp32), &zr0);
	err |= clSetKernelArg(engine.kernel, 7, sizeof(fp32), &zi0);
	err |= clSetKernelArg(engine.kernel, 8, sizeof(uint32_t), &formula);
	err |= clSetKernelArg(engine.kernel, 9, sizeof(cl_mem), &deviceResultBuf);
	printChange("\nKernelArgs: %d",err);

	uint32_t cor = (resX * resY) % WorkGroup_Size; // Calculates the correction factor to ensure divisibility
	cor = (cor == 0) ? 0 : (WorkGroup_Size - cor);
	local_size = WorkGroup_Size;
	global_size = resX * resY + cor; // Number of total work items - localSize must be devisor
	err = clEnqueueNDRangeKernel(engine.queue, engine.kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL); /* Enqueue kernel */
	printChange("\nclEnqueueNDRangeKernel: %d",err);
	
	clFinish(engine.queue); /* Wait for the command queue to get serviced before reading back results */
	clEnqueueReadBuffer(engine.queue, deviceResultBuf, CL_TRUE, 0, sizeof(uint8_t) * resX * resY * 3, data, 0, NULL, NULL); /* Read the kernel's output */
	//for (u32 z = 0; z < resX * resY * 3; z++) { data[z] = resultBuf[z]; }
}