/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef RENDER_H
#define RENDER_H

#include "Common_Def.h"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

int setup_fracExpKB(int argc, char* argv[]);

int init_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Key_Function_Mutex);

int terminate_Render();

void newFrame();

/* SDL_Scancode */
bool keyPressed(uint32_t key);

#endif /* RENDER_H */