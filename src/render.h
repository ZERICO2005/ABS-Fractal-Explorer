/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef RENDER_H
#define RENDER_H
#include "Common_Def.h"

int init_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex);

int terminate_Render();

void newFrame();

#endif /* RENDER_H */