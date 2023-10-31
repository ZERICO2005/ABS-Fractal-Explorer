/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "fracExp_Internal.h"
#include "fracExpKB.h"

void create_FracExpKB_File(KeyBind_Preset* preset, char* path) {
	if (preset == NULL || path == NULL) {
		printError("Unable to create_FracExp_File due to NULL parameters");
		return; 
	}
	/* Setup */
	char *pathF;
	correctWindowsFileName(&pathF,path);
	if (pathF == NULL) {
		printError("Unable to format pathF, switching to path");
		pathF = path;
	}
	FILE *ptrF;
	ptrF = fopen(path, "rb");
	if (ptrF == NULL) {
		printError("Unable to open %s",path);
		return;
	}
	fseek(ptrF, 0, SEEK_END);
	size_t len = ftell(ptrF);
	fseek(ptrF, 0, SEEK_SET);
	char* fracExpKB_raw = (char*)malloc(len + 1);
	if (fracExpKB_raw == NULL) {
		printError("Unable to allocate memory for fracExpKB_raw");
		return;
	}
	fread(fracExpKB_raw, 1, len, ptrF); fracExpKB_raw[len] = '\0'; // Manual string termination
	fclose(ptrF);
	FREE(pathF);
	
	Param_List* param_list; size_t param_len = 0;
	generate_Param_List(fracExpKB_raw,len,&param_list,&param_len);

	printFlush("\n\nRetriving Data\n");
	Param_List* item = NULL;
	#define integerFromParam(base) ((item != NULL) ? getNumberFromText(&raw[item->pos], item->len,base) : 0)
	//FracExp_Header
	item = getParameter(fracExpKB_raw,(char*)"FracExp_Header/FracExp_Version_Major",&param_list,&param_len);
	if (item == NULL) {
		printf("\nnull item");
	}
	FREE(fracExpKB_raw);
	//FREE(param_list);
	printFlush("\n");
		
	//size_t param_start;  size_t param_end;
}

int read_FracExpKB_File(KeyBind_Preset* ptr, char* path) {
	if (ptr == NULL || path == NULL) {
		printError("Unable to read_FracExp_File due to NULL parameters");
		return -1;
	}
	create_FracExpKB_File(ptr,path);
	return 0;
}

int write_FracExpKB_File(KeyBind_Preset* ptr, char* path) {
	if (ptr == NULL || path == NULL) {
		printError("Unable to write_FracExp_File due to NULL parameters");
		return -1;
	}
	return 0;
}





