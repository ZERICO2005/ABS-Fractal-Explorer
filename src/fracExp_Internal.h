/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACEXP_INTERNAL_H
#define FRACEXP_INTERNAL_H

#include "Common_Def.h"

struct _Param_List {
	size_t pos;
	size_t len;
	size_t depth;
	bool isValue;
}; typedef _Param_List Param_List;

void printText(char* text, size_t len);


void printParamText(char* raw, Param_List* item);

void printParamTextN(char* raw, Param_List* item);

size_t clean_frac_raw(char* raw, size_t len);

void generate_Param_List(char* raw, size_t len,Param_List** param_list_ptr,size_t* param_len_ptr);
bool strictCompareText(char* strA, size_t lenA, char* strB, size_t lenB);
int64_t getNumberFromText(char* str, size_t len, uint8_t base);

Param_List* getParameter(char* raw, char* path, Param_List** param_list_ptr,size_t* param_len_ptr);

char paramCharAt(char* raw, Param_List* item, size_t index);
void getTextFromParam(char* raw, Param_List* item, char* buf, size_t len);
void copyHex(char* raw, Param_List* item, uint64_t* hex, uint64_t* hash, size_t len);

int correctWindowsFileName(char** ret, char* path);
int fixWindowsFileName(char** ret, char* path);


#endif /* FRACEXP_INTERNAL_H */