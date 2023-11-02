/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef KEYBIND_H
#define KEYBIND_H
#include "Common_Def.h"
#include "copyBuffer.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/* 0 = ANSI 104key, 1 = Extended 152key, 2 = Complete 242key */
void renderKeyboard(
	BufferBox* buf, int32_t ResX, fp64 minScaleX, fp64 maxScaleX,
	uint8_t KeyboardSize, bool includeNumpad,
	int32_t curX, int32_t curY, bool click, SDL_Scancode* code
);

// Default colors
void initKeyboardGraphics(fp64 hue, fp64 sat, fp64 val);
void terminateKeyboardGraphics();

SDL_KeyCode getHover_Keycode(uint32_t x, uint32_t y);
SDL_Scancode getHover_Scancode(uint32_t x, uint32_t y);

void setRGB_Scancode(uint8_t r, uint8_t g, uint8_t b, SDL_Scancode code);
void setColor_Scancode(uint32_t color, SDL_Scancode code);
void setHSV_Scancode(fp64 h, fp64 s, fp64 v, SDL_Scancode code);

#endif /* KEYBIND_H */