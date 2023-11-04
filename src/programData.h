/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef PROGRAMDATA_H
#define PROGRAMDATA_H

#include "Common_Def.h"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "engine.h"
#include "render.h"
#include "fractal.h"
#include "copyBuffer.h"

/*
** Handling shared data between threads
*/

bool read_Render_Ready();
void write_Render_Ready(bool f);

struct Function_Status {
	bool triggered;
	uint64_t resetTime;
	uint64_t freqTime;
}; typedef struct Function_Status Function_Status;

int read_Function_Status(Function_Status* list);
int write_Function_Status(Function_Status* list);

void read_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary);
void write_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary);

void write_Buffer_Size(BufferBox size);
BufferBox read_Buffer_Size();
/* Used to ensure buffers are not read during a swap */
int clear_Render_Buffers();
int read_Render_Buffers(BufferBox* primary);
int write_Render_Buffers(BufferBox* primary);

void setRenderDelta(fp64 t);
fp64 getRenderDelta();

#endif /* PROGRAMDATA_H */