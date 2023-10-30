/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACEXPKB_H
#define FRACEXPKB_H

#include "Common_Def.h"
#include "fractal.h"
#include "engine.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/* Contains the fractals data along with its hash, name, and description */
struct FracExpKB_KeyBind {
	Key_Function::Key_Function_Enum func;
	Key_Function::Key_Function_Enum key;
}; typedef struct FracExpKB_Keybind FracExpKB_Keybind;

/* Contains lots of metadata */
struct FracExpKB_File {
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
			uint64_t hash_keybind[4];
	/* File_Information */
		char* File_Platform;
		uint64_t File_Created;
		uint64_t File_Modified;
		uint64_t File_Opened;
		char* File_Username; /* User MUST opt-in for this information to be saved */
	/* Keybind_List */
		uint32_t keybind_count;
		FracExpKB_Keybind* keybind_list;
}; typedef struct FracExpKB_File FracExpKB_File;

int write_FracExp_File(FracExpKB_File* ptr, char* path);
int read_FracExp_File(FracExpKB_File* ptr, char* path);


#endif /* FRACEXPKB_H */