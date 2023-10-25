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
#include "copyBuffer.h"

BufferBox Render;

int start_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
	while (QUIT_FLAG == false) {
		
	}
	return 0;
}

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
	printf("\nInit_Engine: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	initBufferBox(&Render,NULL,720,480,3,4);
	Render.vram = (uint8_t*)malloc(getBufferBoxSize(&Render));
	start_Engine(QUIT_FLAG,Console_Mutex);
	return 0;
}

int terminate_Engine() {
	FREE(Render.vram);
	return 0;
}

void renderFractal(BufferBox* buf, fp64 r, fp64 i, fp64 zoom, uint32_t maxItr) {
	if (buf == NULL) { return; }
	if (buf->vram == NULL) { return; }
	size_t dataPtr = 0;
	fp64 BREAKOUT = 256.0;
	fp64 cr = 0.0;
	fp64 ci = 0.0;
	uint32_t resX = buf->resX;
	uint32_t resY = buf->resY;
		
	for (u32 y = 0; y < resY; y++) {
		ci = -((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + i;
		
		for (u32 x = 0; x < resX; x++) {
			cr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + r;
			
			fp64 zr = 0.0; // Default 0.0
			fp64 zi = 0.0; // Default 0.0
			fp64 low = 4.0; // Squared
			fp64 temp = 0.0;
			fp64 zs = 0.0;
			
			for (u32 itr = 0; itr < maxItr; itr++) {

				temp = zr * zr - abs(zi) * zi + cr;
				zi = zr * zi * 2 + ci;
				zr = temp;
				zs = zr * zr + zi * zi;
				
				if (zs < low) {
					low = zs;
				} else if (zs > BREAKOUT) {
					fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) / log2(2.0))); //log2(sqrt(zs)) / log2(base)
					buf->vram[dataPtr] = (uint8_t)floor(0.9 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++;
					buf->vram[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.9)))); dataPtr++;
					buf->vram[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.1)))); dataPtr++;
					break;
				}

			}
			if (zs <= BREAKOUT) {
				buf->vram[dataPtr] = 0; dataPtr++;
				buf->vram[dataPtr] = 0; dataPtr++;
				buf->vram[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;
			}
		}
	}
}