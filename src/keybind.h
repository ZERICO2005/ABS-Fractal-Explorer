/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "Common_Def.h"
#include "copyBuffer.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/* 0 = ANSI 104key, 1 = Extended 152key, 2 = Complete 242key */
void renderKeyboard(BufferBox buf, uint8_t keyboard , bool includeNumpad);

SDL_KeyCode getHover_Keycode(uint32_t x, uint32_t y);
SDL_Scancode getHover_Scancode(uint32_t x, uint32_t y);

void setColor_Keycode(uint32_t color, SDL_KeyCode code);
void setColor_Scancode(uint32_t color, SDL_Scancode code);
void setRGB_Keycode(uint8_t r, uint8_t g, uint8_t b, SDL_KeyCode code);
void setRGB_Scancode(uint8_t r, uint8_t g, uint8_t b, SDL_Scancode code);
void setHSL_Keycode(fp64 h, fp64 s, fp64 v, SDL_KeyCode code);
void setHSL_Scancode(fp64 h, fp64 s, fp64 v, SDL_Scancode code);

#endif /* KEYBOARD_H */