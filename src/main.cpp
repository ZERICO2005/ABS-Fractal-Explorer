/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "engine.h"
#include "render.h"

std::atomic<bool> QUIT_FLAG(false);
std::mutex Console_Mutex;

int terminate_Easy_GUI() {
	terminate_Render();
	terminate_Engine();
	return 0;
}

int init_Easy_GUI() {
	std::thread Thread_Engine(init_Engine, std::ref(QUIT_FLAG), std::ref(Console_Mutex));
    std::thread Thread_Render(init_Render, std::ref(QUIT_FLAG), std::ref(Console_Mutex));
	
    Thread_Engine.join();
    Thread_Render.join();
	return 0;
}

int main(int argc, char* argv[]) {
	printFlush("\nEasy-GUI v%s | zerico2005 | %s\n",EASY_GUI_VERSION,EASY_GUI_DATE);
	init_Easy_GUI();
	terminate_Easy_GUI();
	return 0;
}