/*
**	Author: zerico2005 (2023)
**	Project: ABS_Fractal_Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACEXP_H
#define FRACEXP_H

#include "Common_Def.h"
#include "fractal.h"

/* Contains the fractals data along with its hash, name, and description */
struct _Fractal_Header {
	uint64_t hash[4];
	char* name;
	char* description;
	Fractal_Data fractal;
}; typedef struct _Fractal_Header Fractal_Header;

/* Contains lots of metadata */
struct _FracExp_File {
	/* Header */
		/* FracExp */
			uint32_t FracExp_Version_Major;
			uint32_t FracExp_Version_Minor;
			uint32_t FracExp_Version_Patch;
			uint32_t FracExp_Version_Build;
			char* FracExp_Version_Tags;
		/* ABS_Fractal_Explorer */
			uint32_t ABS_Fractal_Explorer_Version_Major;
			uint32_t ABS_Fractal_Explorer_Version_Minor;
			uint32_t ABS_Fractal_Explorer_Version_Patch;
			uint32_t ABS_Fractal_Explorer_Version_Build;
			char* ABS_Fractal_Explorer_Version_Tags;
		/* Hashes */
			uint64_t hash_all[4];
			uint64_t hash_critical[4];
			uint64_t hash_file[4];
			uint64_t hash_time[4];
			uint64_t hash_hardware[4];
			uint64_t hash_fractal[4];
	/* File_Information */
		char* File_Platform;
		uint64_t File_Created;
		uint64_t File_Modified;
		uint64_t File_Opened;
		char* File_Username; /* User MUST opt-in for this information to be saved */
	/* Hardware_Information (User MUST opt-in for this information to be saved) */
		uint32_t Hardware_CPU_Threads;
		uint64_t Hardware_System_RAM;
		char* Hardware_GPU_Name;
		char* Hardware_OpenCL_Support;
		char* Hardware_OpenCL_Profile;
	/* Fractal_List */
		uint32_t fractal_count;
		Fractal_Header* fractal_list;
}; typedef struct _FracExp_File FracExp_File;

int write_FracExp_File(FracExp_File* ptr, char* path);
int read_FracExp_File(FracExp_File* ptr, char* path);


#endif /* FRACEXP_H */