/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef PRIME2D_H
#define PRIME2D_H

#include <stdint.h>

void setPrintInt(uint8_t* vram, uint32_t resX, uint32_t resY);

void printInt(int32_t in, uint8_t len, uint8_t base, uint32_t xC, uint32_t yC);

void printUInt(uint32_t in, uint8_t len, uint8_t base, uint32_t xC, uint32_t yC);

void printFloat(double in, uint8_t len, uint8_t dec, uint8_t base, uint32_t xC, uint32_t yC);

#endif /* PRIME2D_H */