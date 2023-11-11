/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "buildCL.h"

const char* FractalOpenCL_SRC =  "	typedef uchar	uint8_t;\n\
	typedef ushort	uint16_t;\n\
	typedef uint	uint32_t;\n\
	typedef ulong	uint64_t;\n\
	typedef char	int8_t;\n\
	typedef short	int16_t;\n\
	typedef int		int32_t;\n\
	typedef long	int64_t;\n\
\n\
	typedef uchar	u8;\n\
	typedef ushort	u16;\n\
	typedef uint	u32;\n\
	typedef ulong	u64;\n\
	typedef char	i8;\n\
	typedef short	i16;\n\
	typedef int		i32;\n\
	typedef long	i64;\n\
\n\
	typedef half	fp16;\n\
	typedef float	fp32;\n\
	#define PI		3.141592653589793f\n\
	#define TAU		6.283185307179586f\n\
	#define EULER	2.718281828459045f\n\
	#define BREAKOUT 4096.0f\n\
 \n\
__kernel void renderFracCLPoint(fp32 r, fp32 i, fp32 zoom, u32 maxItr, u32 resX, u32 resY, fp32 zr0, fp32 zi0, uint32_t formula, __global uint8_t* resultBuf) {\n\
	u32 id = get_global_id(0);\n\
	u8 type = ((formula & 0xC0000000) >> 30) + 2;\n\
	fp32 y = (fp32)(id / resX);\n\
	fp32 x = (fp32)(id % resX);\n\
	id *= 3;\n\
	fp32 cr,ci,zr,zi;\n\
	if (formula & 0x20000000) {\n\
		zi = -(((y - (((fp32)resY - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + i;\n\
		zr = (((x - (((fp32)resX - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + r;\n\
		cr = zr0;\n\
		ci = zi0;\n\
	} else {\n\
		ci = -(((y - (((fp32)resY - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + i;\n\
		cr = (((x - (((fp32)resX - 1.0f) / 2.0f)) / (((fp32)resY - 1.0f) / 2.0f)) / pow(10.0f, zoom)) + r;\n\
		zr = zr0;\n\
		zi = zi0;\n\
	}\n\
	formula &= 0x1FFFFFFF;\n\
	fp32 low = 4.0f;\n\
	fp32 temp = 0.0f;\n\
	fp32 zs = 0.0f;\n\
	\n\
	if (type == 2) {\n\
		fp32 zr1, zr2, zi1, zi2, s1, s2, s3;\n\
		uint8_t f[8];\n\
		for (uint8_t q = 0; q < 8; q++) {\n\
			f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		s1 = (f[0]) ? -1.0f : 1.0f;\n\
		s2 = (f[1]) ? -1.0f : 1.0f;\n\
		s3 = (f[2]) ? -2.0f : 2.0f;\n\
		for (u32 itr = 0; itr < maxItr; itr++) {\n\
			zr1 = (f[3]) ? fabs(zr) : zr;\n\
			zi1 = (f[4]) ? fabs(zi) : zi;\n\
			zr2 = (f[5]) ? fabs(zr) : zr;\n\
			zi2 = (f[6]) ? fabs(zi) : zi;\n\
			if (f[7] == 0) {\n\
				temp = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;\n\
				zi = (zr2 * zi2 * s3) + ci;\n\
				zr = temp;\n\
			} else {\n\
				temp = s1 * fabs((zr1 * zr) - s2 * (zi1 * zi)) + cr;\n\
				zi = (zr2 * zi2 * s3) + ci;\n\
				zr = temp;\n\
			}\n\
			zs = zr * zr + zi * zi;\n\
			if (zs < low) {\n\
				low = zs;\n\
			} else if (zs > BREAKOUT) {\n\
				fp32 smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(2.0f))); //log2(sqrt(zs)) / log2(base)\n\
				resultBuf[id] = (uint8_t)floor(0.9f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.5f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.9f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));\n\
				break;\n\
			}\n\
		}\n\
	} else if (type == 3) {\n\
		fp32 zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;\n\
		uint8_t f[14];\n\
		for (uint8_t q = 0; q < 14; q++) {\n\
			f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		s1 = (f[0]) ? -1.0f: 1.0;\n\
		s2 = (f[1]) ? -3.0f: 3.0;\n\
		s3 = (f[2]) ? -3.0f: 3.0;\n\
		s4 = (f[3]) ? -1.0f: 1.0;\n\
		s5 = (f[4]) ? -1.0f: 1.0;\n\
		s6 = (f[5]) ? -1.0f: 1.0;\n\
		for (u32 itr = 0; itr < maxItr; itr++) {\n\
			zr1 = (f[6]) ? fabs(zr) : zr;\n\
			zi1 = (f[7]) ? fabs(zi) : zi;\n\
			zr2 = (f[8]) ? fabs(zr) : zr;\n\
			zi2 = (f[9]) ? fabs(zi) : zi;\n\
			zr3 = (f[10]) ? fabs(zr) : zr;\n\
			zi3 = (f[11]) ? fabs(zi) : zi;\n\
			if (f[12] == 0) {\n\
				if (f[13] == 0) {\n\
				temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
				zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
				zr = temp;\n\
				} else {\n\
				temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
				zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
				zr = temp;\n\
				}\n\
			} else {\n\
				if (f[13] == 0) {\n\
				temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
				zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
				zr = temp;\n\
				} else {\n\
				temp = s5 * fabs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\n\
				zi = s6 * fabs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\n\
				zr = temp;\n\
				}\n\
			}\n\
			zs = zr * zr + zi * zi;\n\
			if (zs < low) {\n\
				low = zs;\n\
			} else if (zs > BREAKOUT) {\n\
				fp32 smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(3.0f)));\n\
				resultBuf[id] = (uint8_t)floor(0.9f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.5f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.9f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));\n\
				break;\n\
			}\n\
		}\n\
	} else if (type == 4) {\n\
		fp32 zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;\n\
		uint8_t f[17];\n\
		for (uint8_t q = 0; q < 17; q++) {\n\
			f[q] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		s1 = (f[0]) ? -1.0f: 1.0;\n\
		s2 = (f[1]) ? -6.0f: 6.0;\n\
		s3 = (f[2]) ? -1.0f: 1.0;\n\
		s4 = (f[3]) ? -4.0f: 4.0;\n\
		s5 = (f[4]) ? -4.0f: 4.0;\n\
		s6 = (f[5]) ? -1.0f: 1.0;\n\
		s7 = (f[6]) ? -1.0f: 1.0;\n\
		for (u32 itr = 0; itr < maxItr; itr++) {\n\
			zr1 = (f[7]) ? fabs(zr) : zr;\n\
			zi1 = (f[8]) ? fabs(zi) : zi;\n\
			zr2 = (f[9]) ? fabs(zr) : zr;\n\
			zi2 = (f[10]) ? fabs(zi) : zi;\n\
			zr3 = (f[11]) ? fabs(zr) : zr;\n\
			zi3 = (f[12]) ? fabs(zi) : zi;\n\
			zr4 = (f[13]) ? fabs(zr) : zr;\n\
			zi4 = (f[14]) ? fabs(zi) : zi;\n\
\n\
			if (f[15] == 0) {\n\
				if (f[16] == 0) {\n\
					temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
					zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
					zr = temp;\n\
				} else {\n\
					temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
					zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
					zr = temp;\n\
				}\n\
			} else {\n\
				if (f[16] == 0) {\n\
					temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
					zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
					zr = temp;\n\
				} else {\n\
					temp = s6 * fabs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\n\
					zi = s7 * fabs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\n\
					zr = temp;\n\
				}\n\
			}\n\
			zs = zr * zr + zi * zi;\n\
			if (zs < low) {\n\
				low = zs;\n\
			} else if (zs > BREAKOUT) {\n\
				fp32 smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(4.0f)));\n\
				resultBuf[id] = (uint8_t)floor(0.9f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.5f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.9f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));\n\
				break;\n\
			}\n\
		}\n\
	} else if (type == 5) {\n\
		fp32 zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;\n\
		uint8_t fS[6];\n\
		uint8_t fA[10];\n\
		uint8_t fO[4];\n\
		for (uint8_t q = 0; q < 6; q++) {\n\
			fS[q] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		for (uint8_t q = 6; q < 8; q++) {\n\
			fO[q - 6] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		for (uint8_t q = 8; q < 18; q++) {\n\
			fA[q - 8] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		for (uint8_t q = 18; q < 20; q++) {\n\
			fO[q - 16] = ((formula >> q) & 1) ? 1 : 0;\n\
		}\n\
		s1 = (fS[0]) ? -1.0f: 1.0;\n\
		s2 = (fS[1]) ? -10.0f: 10.0;\n\
		s3 = (fS[2]) ? -5.0f: 5.0;\n\
		s4 = (fS[3]) ? -5.0f: 5.0;\n\
		s5 = (fS[4]) ? -10.0f: 10.0;\n\
		s6 = (fS[5]) ? -1.0f: 1.0;\n\
		s7 = (fO[0]) ? -1.0f: 1.0;\n\
		s8 = (fO[1]) ? -1.0f: 1.0;\n\
		for (u32 itr = 0; itr < maxItr; itr++) {\n\
			zr1 = (fA[0]) ? fabs(zr) : zr;\n\
			zi1 = (fA[1]) ? fabs(zi) : zi;\n\
			zr2 = (fA[2]) ? fabs(zr) : zr;\n\
			zi2 = (fA[3]) ? fabs(zi) : zi;\n\
			zr3 = (fA[4]) ? fabs(zr) : zr;\n\
			zi3 = (fA[5]) ? fabs(zi) : zi;\n\
			zr4 = (fA[6]) ? fabs(zr) : zr;\n\
			zi4 = (fA[7]) ? fabs(zi) : zi;\n\
			zr5 = (fA[8]) ? fabs(zr) : zr;\n\
			zi5 = (fA[9]) ? fabs(zi) : zi;\n\
\n\
			if (fO[2] == 0) {\n\
				if (fO[3] == 0) {\n\
					temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
					zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
					zr = temp;\n\
				} else {\n\
					temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
					zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
					zr = temp;\n\
				}\n\
			} else {\n\
				if (fO[3] == 0) {\n\
					temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
					zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
					zr = temp;\n\
				} else {\n\
					temp = s7 * fabs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\n\
					zi = s8 * fabs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\n\
					zr = temp;\n\
				}\n\
			}\n\
			zs = zr * zr + zi * zi;\n\
			if (zs < low) {\n\
				low = zs;\n\
			} else if (zs > BREAKOUT) {\n\
				fp32 smooth = log(1.0f + fmax(0.0f, (fp32)itr - log2(log2(zs) / 2.0f) / log2(5.0f)));\n\
				resultBuf[id] = (uint8_t)floor(0.9f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.5f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.9f)))); id++;\n\
				resultBuf[id] = (uint8_t)floor(1.0f * (127.5f - 127.5f * cos(6.283185307f * (0.45f * smooth + 0.1f))));\n\
				break;\n\
			}\n\
		}\n\
	}\n\
	\n\
	if (zs <= BREAKOUT) {\n\
		resultBuf[id] = 0; id++;\n\
		resultBuf[id] = 0; id++;\n\
		resultBuf[id] = (uint8_t)floor(127.5f - 127.5f * cos(log(low) / 2.0f));\n\
	}\n\
}";

cl_device_id create_device() {
   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL); // GPU
   if(err == CL_DEVICE_NOT_FOUND) { 
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL); // CPU
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }
   return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {
   
   #ifdef BUILD_FRAC_FROM_FILE
      cl_program program;
      FILE *program_handle;
      char *program_buffer, *program_log;
      size_t program_size, log_size;
      int err;

      /* Read program file and place content into buffer */
      program_handle = fopen(filename, "r");
      if(program_handle == NULL) {
         perror("Couldn't find the program file");
         exit(1);
      }
      fseek(program_handle, 0, SEEK_END);
      program_size = ftell(program_handle);
      rewind(program_handle);
      program_buffer = (char*)malloc(program_size + 1);
      program_buffer[program_size] = '\0';
      fread(program_buffer, sizeof(char), program_size, program_handle);
      fclose(program_handle);

      /* Create program from file 
      Creates a program from the source code in the add_numbers.cl file. 
      Specifically, the code reads the file's content into a char array 
      called program_buffer, and then calls clCreateProgramWithSource. */

      program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, &program_size, &err);
      if(err < 0) {
         perror("Couldn't create the program");
         exit(1);
      }
      free(program_buffer);

      /* Build program 
      The fourth parameter accepts options that configure the compilation. 
      These are similar to the flags used by gcc. For example, you can 
      define a macro with the option -DMACRO=VALUE and turn off optimization 
      with -cl-opt-disable.
      */
      err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
      if(err < 0) {
         /* Find size of log and print to std output */
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
         program_log = (char*) malloc(log_size + 1);
         program_log[log_size] = '\0';
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
         printf("%s\n", program_log);
         free(program_log);
         exit(1);
      }
   #endif
   #ifdef BUILD_FRAC_FROM_CONST
      cl_program program;
      FILE *program_handle;
      char *program_log;
      size_t program_size, log_size;
      int err;

      program_size = strlen(FractalOpenCL_SRC);
      program = clCreateProgramWithSource(ctx, 1, (const char**)&FractalOpenCL_SRC, &program_size, &err);
      if(err < 0) {
         perror("Couldn't create the program");
         exit(1);
      }
      err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
      if(err < 0) {
         /* Find size of log and print to std output */
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
         program_log = (char*) malloc(log_size + 1);
         program_log[log_size] = '\0';
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
         printf("%s\n", program_log);
         free(program_log);
         exit(1);
      }
   #endif
   
   return program;
}