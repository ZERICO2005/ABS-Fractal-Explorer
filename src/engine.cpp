/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "engine.h"
#include "render.h"
#include "copyBuffer.h"
#include "programData.h"
#include "imageBuffer.h"

#include "fracMulti.h"
#include "fracCL.h"

Fractal_Data fracData;
Render_Data primaryRender;
Render_Data secondaryRender;
ImageBuffer* currentBuf = nullptr;

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

int render_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	if (currentBuf == nullptr || currentBuf->vram == nullptr || currentBuf->allocated() == false) {
		return -1;
	}
	using namespace Key_Function;
	static BufferBox renderBox;
	currentBuf->getBufferBox(&renderBox);
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
		} else if (fracData.type_value == Fractal_Sierpinski_Carpet) {
			#define CARPET fracData.type.sierpinski_carpet
			#undef CARPET
		}
	}
	return 0;
}

int start_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	using namespace Key_Function;
	
	TimerBox fracTime(1.0 / (60.0 + 0.01));
	fp64 deltaTime = 0.0;

	//int render_update_level = Change_Level::Full_Reset;
	uint64_t render_update_timecode = 0;
	next_Write_Cycle_Pos(&currentBuf,Primary_Full);
	while (QUIT_FLAG == false) {
		if (fracTime.timerReset()) {
			/* Update things */
			read_Parameters(&fracData,&primaryRender,&secondaryRender);
			//render_update_level = read_Update_Level();
			if (render_update_timecode != read_Update_Timecode() && ABORT_RENDERING == false) {
				render_update_timecode = read_Update_Timecode();
				if (currentBuf != nullptr) {
					BufferBox sizeBuf = read_Buffer_Size();
					currentBuf->resizeBuffer(sizeBuf.resX / primaryRender.subSample,sizeBuf.resY / primaryRender.subSample,sizeBuf.channels);
				}
				//printFlush("\nRender: %07llu",(render_update_timecode/1000) % 10000000);
				render_Engine(QUIT_FLAG,ABORT_RENDERING,Key_Function_Mutex);
				//printFlush("\nExport: %07llu",(render_update_timecode/1000) % 10000000);
				next_Write_Cycle_Pos(&currentBuf,Primary_Full);
				deltaTime = fracTime.getDeltaTime();
				setRenderDelta(deltaTime);
			}
			if (render_update_timecode == read_Update_Timecode() && ABORT_RENDERING == false) {
				clear_Update_Level();
			}
			if (read_Abort_Render_Ongoing() == true) {
				write_Abort_Render_Ongoing(false);
			}
		}
	}
	return 0;
}

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	int32_t init_OpenCL_ret = init_OpenCL();
	if (init_OpenCL_ret != 0) {
		printError("OpenCL failed to initialize, error code: %d",init_OpenCL_ret);
	}
	queryOpenCL_GPU();
	clear_Cycle_Buffers();
	while (read_Render_Ready() == false) {
		std::this_thread::yield();
	}
	start_Engine(QUIT_FLAG,ABORT_RENDERING,Key_Function_Mutex);
	return 0;
}

int terminate_Engine() {
	terminate_OpenCL();
	delete_Cycle_Buffers();
	return 0;
}