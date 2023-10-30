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

char* fracExpKB_raw;

void create_FracExp_File(char* raw, char* input) {
	if (raw == NULL || input == NULL) {
		return;
	}
	FILE *ptrF;
	ptrF = fopen(input, "rb");
	if (ptrF == NULL) {
		return;
	}
	fseek(ptrF, 0, SEEK_END);
	size_t len = ftell(ptrF);
	fseek(ptrF, 0, SEEK_SET);
	fracExpKB_raw = (char*)malloc(len + 1);
	fread(fracExpKB_raw, 1, len, ptrF); raw[len] = '\0'; // Manual string termination
	fclose(ptrF);
	generate_Param_List(raw,len);

	printFlush("\n\nRetriving Data\n");
	Param_List* item = NULL;
	#define integerFromParam(base) ((item != NULL) ? getNumberFromText(&frac_raw[item->pos], item->len,base) : 0)
	//FracExp_Header
	item = getParameter("FracExp_Header/FracExp_Version_Major");

	FREE(fracExpKB_raw);
	FREE(param_list);
	printFlush("\n");
}

int read_FracExp_File(FracExpKB_File* ptr, char* path) {
	if (ptr == NULL || path == NULL) {
		return -1;
	}
	return 0;
}

int write_FracExp_File(FracExpKB_File* ptr, char* path) {
	if (ptr == NULL || path == NULL) {
		return -1;
	}
	return 0;
}





