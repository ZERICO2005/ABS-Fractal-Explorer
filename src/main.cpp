/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "engine.h"
#include "render.h"

std::atomic<bool> QUIT_FLAG(false);
std::mutex Console_Mutex;

int terminate_Fractal_Engine() {
	terminate_Render();
	terminate_Engine();
	return 0;
}

int init_Fractal_Engine(int argc, char* argv[]) {
	setup_fracExp(argc,argv);
	setup_fracExpKB(argc,argv);
	std::thread Thread_Engine(init_Engine, std::ref(QUIT_FLAG), std::ref(Console_Mutex));
    std::thread Thread_Render(init_Render, std::ref(QUIT_FLAG), std::ref(Console_Mutex));
	
    Thread_Engine.join();
    Thread_Render.join();
	return 0;
}

//	#define print_pointer_sizes

int main(int argc, char* argv[]) {
	printFlush("\n%s v%s\nzerico2005 | %s\n",PROGRAM_NAME,PROGRAM_VERSION,PROGRAM_DATE);
	#ifdef print_pointer_sizes
		printf("\nsizeof(void*)       = %3zu | %3zub",sizeof(void*),sizeof(void*) * 8);
		printf("\nsizeof(size_t)      = %3zu | %3zub",sizeof(size_t),sizeof(size_t) * 8);
		printf("\nsizeof(char)        = %3zu | %3zub",sizeof(char),sizeof(char) * 8);
		printf("\nsizeof(short)       = %3zu | %3zub",sizeof(short),sizeof(short) * 8);
		printf("\nsizeof(int)         = %3zu | %3zub",sizeof(int),sizeof(int) * 8);
		printf("\nsizeof(long)        = %3zu | %3zub",sizeof(long),sizeof(long) * 8);
		printf("\nsizeof(long long)   = %3zu | %3zub",sizeof(long long),sizeof(long long) * 8);
		printf("\nsizeof(float)       = %3zu | %3zub",sizeof(float),sizeof(float) * 8);
		printf("\nsizeof(double)      = %3zu | %3zub",sizeof(double),sizeof(double) * 8);
		printf("\nsizeof(long double) = %3zu | %3zub",sizeof(long double),sizeof(long double) * 8);
		printf("\n");
		fflush(stdout);
	#endif
	/* Process .fracExp files */
	if (argc >= 2) {
		printFlush("\nArgument Count: %d\n",argc - 1);
	}
	init_Fractal_Engine(argc, argv);
	terminate_Fractal_Engine();
	return 0;
}