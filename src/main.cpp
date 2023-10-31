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

int main(int argc, char* argv[]) {
	printFlush("\n%s v%s\nzerico2005 | %s\n",PROGRAM_NAME,PROGRAM_VERSION,PROGRAM_DATE);
	/* Process .fracExp files */
	if (argc >= 2) {
		printFlush("\nArgument Count: %d\n",argc - 1);
	}
	init_Fractal_Engine(argc, argv);
	terminate_Fractal_Engine();
	return 0;
}