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
#include "copyBuffer.h"
#include "programData.h"

BufferBox PrimaryBuf[2];
BufferBox SecondaryBuf[2];

Function_Status func_stat[Key_Function::Parameter_Function_Count];

void renderFractal(BufferBox* buf, fp64 r, fp64 i, fp64 zoom, uint32_t maxItr);

int setup_fracExp(int argc, char* argv[]) {
	if (argc >= 2) {
		for (int a = 1; a < argc; a++) {
			if (strstr(argv[a],".fracExpKB") != NULL) {
				continue;
			}
			if (strstr(argv[a],".fracExp") != NULL) {
				printFlush("\nFracExp File: %s",argv[a]);
			}
		}
	}
	return 0;
}

int start_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Key_Function_Mutex) {
	using namespace Key_Function;
	BufferBox* currentBuf = &PrimaryBuf[0];
	BufferBox* prevBuf = NULL;
	static fp64 r = 0.0;
	static fp64 i = 0.0;
	static fp64 zoom = 0.0;
	uint32_t maxItr = 64;
	TimerBox fracTime(1.0/60.0);
	fp64 deltaTime = 0.0;
	while (QUIT_FLAG == false) {
		if (fracTime.timerReset()) {
			deltaTime = fracTime.getDeltaTime();
			setRenderDelta(deltaTime);
			printf("\nr: %.6lf i: %.6lf zoom: 10^%.4lf maxItr: %u",r,i,zoom,maxItr);
			read_Function_Status(func_stat);
			if (func_stat[incRealPos].triggered == true) {
				r += 0.6 * pow(10.0,-zoom) * deltaTime;
			}
			if (func_stat[decRealPos].triggered == true) {
				r -= 0.6 * pow(10.0,-zoom) * deltaTime;
			}
			if (func_stat[incImagPos].triggered == true) {
				i += 0.6 * pow(10.0,-zoom) * deltaTime;
			}
			if (func_stat[decImagPos].triggered == true) {
				i -= 0.6 * pow(10.0,-zoom) * deltaTime;
			}
			if (func_stat[incZoom].triggered == true) {
				zoom += 0.3 * deltaTime;
			}
			if (func_stat[decZoom].triggered == true) {
				zoom -= 0.3 * deltaTime;
			}
			if (func_stat[resetCoordinates].triggered == true) {
				r = 0.0; i = 0.0; zoom = 0.0;
			}
			if (func_stat[incMaxItr].triggered == true) {
				maxItr += (uint32_t)(128.0 * deltaTime);
			}
			if (func_stat[decMaxItr].triggered == true) {
				maxItr -= (uint32_t)(128.0 * deltaTime);
			}
			if (func_stat[resetMaxItr].triggered == true) {
				maxItr = 192;
			}
			valueLimit(r,-10.0,10.0);
			valueLimit(i,-10.0,10.0);
			valueLimit(zoom,-20.0,40.0);
			valueLimit(maxItr,16,262144);
			if (currentBuf->vram != NULL) {
				renderFractal(currentBuf,r,i,zoom,maxItr);
			}
			/*printFlush("\n\n");
			for (size_t y = 0; y < currentBuf->resY; y += 6) {
				for (size_t x = 0; x < currentBuf->resX; x += 4) {
					printf("%X",currentBuf->vram[((y * currentBuf->resX) + x) * 3]/16);
				}
				printf("\n");
			}
			printFlush("\n");*/
			/* Swap and output buffers */
			//clear_Render_Buffers();
			prevBuf = currentBuf;
			currentBuf = (currentBuf == &PrimaryBuf[0]) ? &PrimaryBuf[1] : &PrimaryBuf[0];
			write_Render_Buffers(prevBuf);
			{
				BufferBox sizeBuf = read_Buffer_Size();
				if (sizeBuf.resX != currentBuf->resX || sizeBuf.resY != currentBuf->resY || sizeBuf.channels != currentBuf->channels || sizeBuf.padding != currentBuf->padding) {
					clear_Render_Buffers();
					for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
						bool reallocateBuffers = (getBufferBoxSize(&sizeBuf) != getBufferBoxSize(&PrimaryBuf[b])) ? true : false;
						PrimaryBuf[b].resX = sizeBuf.resX;
						PrimaryBuf[b].resY = sizeBuf.resY;
						PrimaryBuf[b].channels = sizeBuf.channels;
						PrimaryBuf[b].padding = sizeBuf.padding;
						if (reallocateBuffers == true) {
							PrimaryBuf[b].vram = (uint8_t*)realloc(PrimaryBuf[b].vram, getBufferBoxSize(&PrimaryBuf[b]));
						}
					}
				}
			}
		}
	}
	return 0;
}

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Key_Function_Mutex) {
	// printf("\nInit_Engine: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
		initBufferBox(&PrimaryBuf[b],NULL,320,200,3,0);
		PrimaryBuf[b].vram = (uint8_t*)malloc(getBufferBoxSize(&PrimaryBuf[b]));
	}
	while (read_Render_Ready() == false) {
		std::this_thread::yield();
		start_Engine(QUIT_FLAG,Key_Function_Mutex);
	}
	return 0;
}

int terminate_Engine() {
	for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
		FREE(PrimaryBuf[b].vram);
	}
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
				zi = zr * zi * 2.0 + ci;
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