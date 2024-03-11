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
#include "programData.h"

std::atomic<bool> QUIT_FLAG(false);
std::atomic<bool> ABORT_RENDERING(false);
std::mutex Key_Function_Mutex;

int terminate_Fractal_Engine() {
	terminate_Render();
	terminate_Engine();
	return 0;
}

int init_Fractal_Engine(int argc, char* argv[]) {
	init_Cycle_Buffers();
	setup_fracExp(argc,argv);
	setup_fracExpKB(argc,argv);
	std::thread Thread_Engine(init_Engine, std::ref(QUIT_FLAG), std::ref(ABORT_RENDERING), std::ref(Key_Function_Mutex));
    std::thread Thread_Render(init_Render, std::ref(QUIT_FLAG), std::ref(ABORT_RENDERING), std::ref(Key_Function_Mutex));
	
    Thread_Engine.join();
    Thread_Render.join();
	return 0;
}

// int write_ini_file(const char* path) {
// 	if (path == nullptr) { printError("write_ini_file has nullptr path"); return -1; }
// 	// if (config_data.Configuration_Behaviour.AutoSave_Config_File == false) { return 0; }        
// 	FILE* file = nullptr;
// 	file = fopen(path, "r");
// 	if (file != nullptr) { // File already exists
// 		fclose(file);
// 		return 0;
// 	}
// 	file = fopen(path, "w");
// 	if (file == nullptr) {
// 		printError("Unable to write_ini_file to %s", path);
// 		return -1;
// 	}

// 	fprintf(file,
// 		"[Desktop Entry]\n"\
// 		"Name=ABS-Fractal-Explorer\n"\
// 		"Exec=ABS-Fractal-Explorer.exe\n"\
// 		"Icon=ABS-Icon.ico\n"\
// 		"Type=Application"
// 	);

//     fclose(file);

// 	return 0;
// }

//#define print_pointer_sizes

int main(int argc, char* argv[]) {
	printFlush("\n%s v%s\nzerico2005 | %s\n",PROGRAM_NAME,PROGRAM_VERSION,PROGRAM_DATE);
	#ifdef print_pointer_sizes
		printf("\nCHAR_BIT: %2d bits",CHAR_BIT);
		printf("\nsizeof(void*)       = %3zu | %3zub",sizeof(void*),sizeof(void*) * CHAR_BIT);
		printf("\nsizeof(size_t)      = %3zu | %3zub",sizeof(size_t),sizeof(size_t) * CHAR_BIT);
		printf("\nsizeof(char)        = %3zu | %3zub",sizeof(char),sizeof(char) * CHAR_BIT);
		printf("\nsizeof(wchar_t)     = %3zu | %3zub",sizeof(wchar_t),sizeof(wchar_t) * CHAR_BIT);
		printf("\nsizeof(short)       = %3zu | %3zub",sizeof(short),sizeof(short) * CHAR_BIT);
		printf("\nsizeof(int)         = %3zu | %3zub",sizeof(int),sizeof(int) * CHAR_BIT);
		printf("\nsizeof(long)        = %3zu | %3zub",sizeof(long),sizeof(long) * CHAR_BIT);
		printf("\nsizeof(long long)   = %3zu | %3zub",sizeof(long long),sizeof(long long) * CHAR_BIT);\
		printf("\nsizeof(float)       = %3zu | %3zub",sizeof(float),sizeof(float) * CHAR_BIT);
		printf("\nsizeof(bool)        = %3zu | %3zub",sizeof(bool),sizeof(bool) * CHAR_BIT);
		printf("\nsizeof(double)      = %3zu | %3zub",sizeof(double),sizeof(double) * CHAR_BIT);
		printf("\nsizeof(long double) = %3zu | %3zub",sizeof(long double),sizeof(long double) * CHAR_BIT);
		printf("\n");
		fflush(stdout);
	#endif

	/* Process .fracExp files */
	if (argc >= 2) {
		printFlush("\nArgument Count: %d\n",argc - 1);
	}
	//write_ini_file("./ABS-Fractal-Explorer.ini");
	init_Fractal_Engine(argc, argv);
	terminate_Fractal_Engine();
	return 0;
}