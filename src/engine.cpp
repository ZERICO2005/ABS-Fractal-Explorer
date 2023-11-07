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
#include "imageBuffer.h"

#include "fracMulti.h"
#include "fracCL.h"

ImageBuffer PrimaryBuf[2];
ImageBuffer SecondaryBuf[2];

Fractal_Data fracData;
Render_Data primaryRender;
Render_Data secondaryRender;

void render_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param);

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

int start_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	using namespace Key_Function;
	ImageBuffer* currentBuf = &PrimaryBuf[0];
	ImageBuffer* prevBuf = NULL;
	// static fp64 r = 0.0;
	// static fp64 i = 0.0;
	// static fp64 zoom = 0.0;
	// uint32_t maxItr = 64;
	TimerBox fracTime(1.0/60.0);
	fp64 deltaTime = 0.0;
	while (QUIT_FLAG == false) {
		if (fracTime.timerReset()) {
			deltaTime = fracTime.getDeltaTime();
			setRenderDelta(deltaTime);
			read_Parameters(&fracData,&primaryRender,&secondaryRender);
			static BufferBox renderBox;
			currentBuf->getBufferBox(&renderBox);
			if (currentBuf->vram != NULL) {
				//render_ABS_Mandelbrot(currentBuf,primaryRender,fracData.type.abs_mandelbrot);
				if (ABORT_RENDERING == false) {
					#define FRAC fracData.type.abs_mandelbrot
					//printfInterval(0.4,"\nr: %.6lf i: %.6lf zoom: 10^%.4lf maxItr: %u formula: %llu",FRAC.r,FRAC.i,FRAC.zoom,FRAC.maxItr,FRAC.formula);
					#undef FRAC
					switch(primaryRender.rendering_method) {
						case 0:
							if (fracData.type.abs_mandelbrot.polarMandelbrot == true) {
								renderCPU_Polar_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
							} else {
								renderCPU_ABS_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
							}
							break;
						case 1:
							renderOpenCL_ABS_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING);
							break;
						default:
						printfInterval(0.5,"Unknown rendering method %u",primaryRender.rendering_method);
					}
					if (fracData.type_value == Fractal_ABS_Mandelbrot || fracData.type_value == Fractal_Polar_Mandelbrot) {
						#define FRAC fracData.type.abs_mandelbrot
						fp64 cx0; fp64 cy0;
						fp64 cx1; fp64 cy1;
						i32 offX = (i32)(currentBuf->resX * primaryRender.subSample);
						i32 offY = (i32)(currentBuf->resY * primaryRender.subSample);
						fp64 extraPadding = 0.0;
						pixel_to_coordinate((i32)((fp64)offX * -extraPadding),(i32)((fp64)offY * -extraPadding),&cx0,&cy0,&FRAC,&primaryRender);
						pixel_to_coordinate((i32)((fp64)offX * (extraPadding + 1.0)),(i32)((fp64)offY * (extraPadding + 1.0)),&cx1,&cy1,&FRAC,&primaryRender);
						currentBuf->setTransformationData(cx0,cy0,cx1,cy1);
						currentBuf->rot = FRAC.rot;
						#undef FRAC
					}
				}
				if (read_Abort_Render_Ongoing() == true) {
					write_Abort_Render_Ongoing(false);
				}
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
			write_Image_Buffers(prevBuf);
			{
				BufferBox sizeBuf = read_Buffer_Size();
				clear_Render_Buffers();
				for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
					PrimaryBuf[b].resizeBuffer(sizeBuf.resX,sizeBuf.resY,sizeBuf.channels);
				}
			}
		}
	}
	return 0;
}

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	// printf("\nInit_Engine: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
		// initBufferBox(&PrimaryBuf[b],NULL,320,200,3,0);
		// PrimaryBuf[b].vram = (uint8_t*)malloc(getBufferBoxSize(&PrimaryBuf[b]));
		PrimaryBuf[b] = ImageBuffer(3);
	}
	int32_t init_OpenCL_ret = init_OpenCL();
	if (init_OpenCL_ret != 0) {
		printError("OpenCL failed to initialize, error code: %d",init_OpenCL_ret);
	}
	queryOpenCL_GPU();
	while (read_Render_Ready() == false) {
		std::this_thread::yield();
		start_Engine(QUIT_FLAG,ABORT_RENDERING,Key_Function_Mutex);
	}
	return 0;
}

int terminate_Engine() {
	terminate_OpenCL();
	// for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
	// 	FREE(PrimaryBuf[b].vram);
	// }
	for (size_t b = 0; b < ARRAY_LENGTH(PrimaryBuf); b++) {
		PrimaryBuf[b].deleteBuffer();
	}
	return 0;
}

void render_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param) {
	if (buf == NULL) { return; }
	if (buf->vram == NULL) { return; }
	//printfInterval(0.3,"\nr: %.6lf i: %.6lf zoom: 10^%.4lf maxItr: %u",param.r,param.i,param.zoom,param.maxItr);
	size_t dataPtr = 0;
	fp64 BREAKOUT = param.breakoutValue;
	fp64 cr = 0.0;
	fp64 ci = 0.0;
	uint32_t resX = buf->resX;
	uint32_t resY = buf->resY;

	for (u32 y = 0; y < resY; y++) {
		ci = -((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, param.zoom)) + param.i;
		
		for (u32 x = 0; x < resX; x++) {
			cr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, param.zoom)) + param.r;
			
			fp64 zr = param.zr; // Default 0.0
			fp64 zi = param.zi; // Default 0.0
			fp64 low = 4.0; // Squared
			fp64 temp = 0.0;
			fp64 zs = 0.0;
			
			for (u32 itr = 0; itr < param.maxItr; itr++) {

				temp = zr * zr - abs(zi) * zi + cr;
				zi = zr * zi * 2.0 + ci;
				zr = temp;
				zs = zr * zr + zi * zi;
				
				if (zs < low) {
					low = zs;
				} else if (zs > BREAKOUT) {
					fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) / log2(2.0))); //log2(sqrt(zs)) / log2(base)
					buf->vram[dataPtr] = (uint8_t)floor(param.rA * (127.5 - 127.5 * cos(TAU * (param.rF * smooth + param.rP)))); dataPtr++;
					buf->vram[dataPtr] = (uint8_t)floor(param.gA * (127.5 - 127.5 * cos(TAU * (param.gF * smooth + param.gP)))); dataPtr++;
					buf->vram[dataPtr] = (uint8_t)floor(param.bA * (127.5 - 127.5 * cos(TAU * (param.bF * smooth + param.bP)))); dataPtr++;
					break;
				}

			}
			if (zs <= BREAKOUT) {
				buf->vram[dataPtr] = 0; dataPtr++;
				buf->vram[dataPtr] = 0; dataPtr++;
				buf->vram[dataPtr] = (uint8_t)floor(127.5 - 127.5 * param.iA * cos(log(low) * param.iF + param.iP)); dataPtr++;
			}
		}
	}
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