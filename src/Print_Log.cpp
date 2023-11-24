/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "Print_Log.h"

Print_Log::Print_Log() {
	Print_Log::init();
}
Print_Log::~Print_Log() {
	Print_Log::terminate();
}

void Print_Log::init() {
	initialized = true;
}

void Print_Log::terminate() {
	Print_Log::clear();
	initialized = false;
}

void Print_Log::clear() {
	textLog.clear();
}

std::string Print_Log::getLine(size_t lineNumber) {
	if (initialized == false || lineNumber >= textLog.size()) {
		return std::string();
	}
	return textLog[lineNumber];
}
size_t Print_Log::getLineCount() {
	if (initialized == false) {
		return 0;
	}
	return textLog.size();
}

int Print_Log::printf(const char* fmt, ...)  {
	
}
void writef(const char* fmt, ...) {

}

int Print_Log::exportToFile(const char* path) {
	if (path == nullptr) { return -1; }
	FILE *ptrW;
	ptrW = fopen(path, "wb");
	if (ptrW == nullptr) {
		printError("Unable to open %s",path);
		return -1;
	}
	for (size_t i = 0; i < textLog.size(); i++) {
		fprintf(ptrW,"%s",textLog[i].c_str());
	}
	/* Write Data */
	if (fclose(ptrW) != 0) {
		printError("\nFailed to close file: %s",path);
		return -1;
	}
	return 0;
}

