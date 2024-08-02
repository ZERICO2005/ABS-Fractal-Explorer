/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "engine.h"
#include "render.h"
#include "programData.h"
#include "fileManager.h"

#include <cstdio>

std::atomic<bool> QUIT_FLAG(false);
std::atomic<bool> ABORT_RENDERING(false);

int terminate_Fractal_Engine() {
	terminate_Render();
	terminate_Engine();
	terminate_FileInterface();
	return 0;
}

int init_Fractal_Engine(int argc, char* argv[]) {
	init_FileInterface(argc, argv);
	init_Cycle_Buffers();
	setup_fracExp(argc,argv);
	setup_fracExpKB(argc,argv);
	std::thread Thread_Engine(init_Engine, std::ref(QUIT_FLAG), std::ref(ABORT_RENDERING));
    std::thread Thread_Render(init_Render, std::ref(QUIT_FLAG), std::ref(ABORT_RENDERING));
	
    Thread_Engine.join();
    Thread_Render.join();
	return 0;
}

/** Debugging Functions */
__attribute__((unused)) void print_pointer_sizes() {
	printf("CHAR_BIT: %2d bits\n", CHAR_BIT);
	// printf("sizeof(void)        = %3zu | %3zub\n", sizeof(void)       , sizeof(void)        * CHAR_BIT);
	printf("sizeof(void*)       = %3zu | %3zub\n", sizeof(void*)      , sizeof(void*)       * CHAR_BIT);
	printf("sizeof(size_t)      = %3zu | %3zub\n", sizeof(size_t)     , sizeof(size_t)      * CHAR_BIT);
	printf("sizeof(ptrdiff_t)   = %3zu | %3zub\n", sizeof(ptrdiff_t)  , sizeof(ptrdiff_t)   * CHAR_BIT);
	printf("sizeof(intptr_t)    = %3zu | %3zub\n", sizeof(intptr_t)   , sizeof(intptr_t)    * CHAR_BIT);
	printf("sizeof(char)        = %3zu | %3zub\n", sizeof(char)       , sizeof(char)        * CHAR_BIT);
	printf("sizeof(wchar_t)     = %3zu | %3zub\n", sizeof(wchar_t)    , sizeof(wchar_t)     * CHAR_BIT);
	printf("sizeof(bool)        = %3zu | %3zub\n", sizeof(bool)       , sizeof(bool)        * CHAR_BIT);
	printf("sizeof(short)       = %3zu | %3zub\n", sizeof(short)      , sizeof(short)       * CHAR_BIT);
	printf("sizeof(int)         = %3zu | %3zub\n", sizeof(int)        , sizeof(int)         * CHAR_BIT);
	printf("sizeof(long)        = %3zu | %3zub\n", sizeof(long)       , sizeof(long)        * CHAR_BIT);
	printf("sizeof(long long)   = %3zu | %3zub\n", sizeof(long long)  , sizeof(long long)   * CHAR_BIT);
	printf("sizeof(float)       = %3zu | %3zub\n", sizeof(float)      , sizeof(float)       * CHAR_BIT);
	printf("sizeof(double)      = %3zu | %3zub\n", sizeof(double)     , sizeof(double)      * CHAR_BIT);
	printf("sizeof(long double) = %3zu | %3zub\n", sizeof(long double), sizeof(long double) * CHAR_BIT);
	fflush(stdout);
}

static void write_Program_RelativeFilePath(const char* path) {
	if (path == nullptr) {
		set_RelativeFilePath("./");
		printError("argv[0] is nullptr");
		return;
	}
	size_t len = strlen(path);
	if (len == 0) {
		set_RelativeFilePath("./");
	}
	size_t match_pos = 0;
	for (size_t i = 0; i < len; i++) {
		if (
			#ifdef PLATFORM_WINDOWS
			(path[i] == '\\') ||
			#endif
			(path[i] == '/')
		) {
			match_pos = i;
		}
	}
	std::string path_str = path;
	set_RelativeFilePath(path_str.substr(0, match_pos + 1));
}

int main(int argc, char* argv[]) {
	printf("\n%s v%s\nzerico2005 | %s\n",PROGRAM_NAME,PROGRAM_VERSION,PROGRAM_DATE);
	fflush(stdout);
	if (argc >= 1) { write_Program_RelativeFilePath(argv[0]); }

	// print_pointer_sizes();

	const Supported_CPU_Instruction& Available_CPU_Instruction = get_Available_CPU_Instruction();
	if (count_Supported_CPU_Instruction(Available_CPU_Instruction) == 0) {
		printf("Warning: Unable to obtain available CPU instruction set extensions\n");
		fflush(stdout);
	}

	/* Process .fracExp files */
	if (argc >= 2) {
		// printFlush("\nArgument Count: %d\n",argc - 1);
		bool printed_cpu_info = false;
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-cpu-info") == 0 && printed_cpu_info == false) {
				print_Supported_CPU_Instruction(Available_CPU_Instruction);
				printed_cpu_info = true;
			}
		}
	}
	
	//write_ini_file("./ABS-Fractal-Explorer.ini");
	int ret_init = init_Fractal_Engine(argc, argv);
	int ret_terminate = terminate_Fractal_Engine();

	if (ret_init == 0 && ret_terminate == 0) {
		printf("\nProgram Exited Normally\n");
	} else {
		printf(
			"\nProgram Exited with Error/Warning:\n"\
			"\tinit: %d\n\tterminate: %d\n",
			ret_init,ret_terminate
		);
	}
	fflush(stdout);

	return 0;
}
