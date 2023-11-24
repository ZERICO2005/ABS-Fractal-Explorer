/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef PRINT_LOG_H
#define PRINT_LOG_H

#include "Common_Def.h"
#include "Program_Def.h"

class Print_Log {
	public:
		void init();
		void terminate();
		void clear();
		size_t writeToLog();
		std::string getLine(size_t lineNumber);
		size_t getFirstLineIndex();
		size_t getLastLineIndex();
		size_t getLineCount();
		int printf(const char* fmt, ...);
		int writef(const char* fmt, ...);
		int exportToFile(const char* path);
	private:
		bool initialized;
		std::vector<std::string> textLog;
};


#endif /* PRINT_LOG_H */