/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef ENGINE_H
#define ENGINE_H
#include "Common_Def.h"

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex);

int terminate_Engine();

#endif /* ENGINE_H */