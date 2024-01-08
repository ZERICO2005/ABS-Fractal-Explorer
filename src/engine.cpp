/*
**	Author: zerico2005 (2023-2024)
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

#include "fileManager.h"

#include "fracMulti.h"
#include "fracCL.h"

Fractal_Data fracData;
Render_Data primaryRender;
Render_Data secondaryRender;
ImageBuffer* currentBuf = nullptr;
ImageBuffer* previewBuf = nullptr;

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

int super_render_code(std::atomic<bool>& ABORT_RENDERING) {
	static uint32_t image_file_format = Image_File_Format::PNG;
	static uint8_t image_quality = 8;
	static Render_Data image_render_data = {0};
	static Fractal_Data image_fractal_data = {0};
	static BufferBox image_box = {0};
	if (receive_Image_Render(&image_fractal_data,&image_render_data,&image_file_format,&image_quality)) {
		memset(&image_box,0,sizeof(BufferBox));
		image_box.resX = image_render_data.resX;
		image_box.resY = image_render_data.resY;
		image_box.channels = image_render_data.channels;
		image_box.padding = image_render_data.padding;
		image_box.vram = nullptr;
		size_t image_box_size = getBufferBoxSize(&image_box);
		if (image_box_size < 16) {
			printError("Super Screenshot has invalid resolution parameters");
			return -1;
		} else if (image_box_size > (size_t)INT32_MAX) {
			printError("Super Screenshot is too large, invalid resolution parameters");
			return -1;
		}
		image_box.vram = (uint8_t*)malloc(image_box_size);
		if (image_box.vram == nullptr) {
			printError("Unable to allocate memory for super screenshot");
			return -1;
		}
		memset(image_box.vram,0,image_box_size);

		printf("\n\nRendering Super Screenshot:");
		printf("\n\t%ux%u %u samples",image_box.resX,image_box.resY,image_render_data.sample * image_render_data.sample);
		if (image_fractal_data.type_value == Fractal_ABS_Mandelbrot) {
			printf(", %u iterations",image_fractal_data.type.abs_mandelbrot.maxItr);
		} else if (image_fractal_data.type_value == Fractal_Polar_Mandelbrot) {
			printf(", %u iterations",image_fractal_data.type.polar_mandelbrot.maxItr);
		}
		switch(image_render_data.rendering_method) {
			case Rendering_Method::CPU_Rendering:
				printf("\n\tFP%u CPU rendering, %u threads",image_render_data.CPU_Precision,image_render_data.CPU_Threads);
			break;
			case Rendering_Method::GPU_Rendering:
				printf("\n\tFP%u GPU rendering",image_render_data.GPU_Precision);
			break;
		};
		printf("\n\tClick \"Abort Rendering\" (or use task manager) to cancel.");
		fflush(stdout);
		uint64_t image_stopwatch = getNanoTime();
		switch(image_render_data.rendering_method) {
			case Rendering_Method::CPU_Rendering:
				if (image_fractal_data.type.abs_mandelbrot.polarMandelbrot == true) {
					renderCPU_Polar_Mandelbrot(&image_box,image_render_data,image_fractal_data.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
				} else {
					renderCPU_ABS_Mandelbrot(&image_box,image_render_data,image_fractal_data.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
				}
				break;
			case Rendering_Method::GPU_Rendering:
				renderOpenCL_ABS_Mandelbrot(&image_box,image_render_data,image_fractal_data.type.abs_mandelbrot,ABORT_RENDERING);
				break;
			default:
				printfInterval(0.5,"Error: Super Screenshot, unknown rendering method %u",image_render_data.rendering_method);
				return -1;
		};
		uint64_t image_render_time = getNanoTime() - image_stopwatch;
		uint32_t time_mili = (uint32_t)(image_render_time / (((uint64_t)1000000)) % (uint64_t)1000);
		uint32_t time_seconds = (uint32_t)((image_render_time / ((uint64_t)1000000 * (uint64_t)1000)) % (uint64_t)60);
		uint32_t time_minutes = (uint32_t)((image_render_time / ((uint64_t)1000000 * (uint64_t)1000 * (uint64_t)60)) % (uint64_t)60);
		uint32_t time_hours = (uint32_t)(image_render_time / ((uint64_t)1000000 * (uint64_t)1000 * (uint64_t)60 * (uint64_t)60));
		printFlush("\n\tRendered in: %02u:%02u:%02u.%03u",time_hours,time_minutes,time_seconds,time_mili);		
		printFlush("\n\tSaving Super Screenshot");
		{
			uint64_t curTime = getNanoTime();
			curTime /= 1000;
			char id_number[64]; memset(id_number,'\0',sizeof(id_number));
			if (image_fractal_data.type_value == Fractal_ABS_Mandelbrot) {
				snprintf(id_number,sizeof(id_number),"_id-%llu",image_fractal_data.type.abs_mandelbrot.formula);
			} else if (image_fractal_data.type_value == Fractal_Polar_Mandelbrot) {
				snprintf(id_number,sizeof(id_number),"_id-%llu",image_fractal_data.type.polar_mandelbrot.formula);
			}
			size_t size = snprintf(nullptr,0,"Super_%s%s_(%llu)",FractalTypeFileText[image_fractal_data.type_value],id_number,curTime);
			size++;
			char* name = (char*)calloc(size,sizeof(char));
			snprintf(name,size,"Super_%s%s_(%llu)",FractalTypeFileText[image_fractal_data.type_value],id_number,curTime);
			char path[] = "./";
			switch(image_file_format) {
				case Image_File_Format::PNG:
					valueLimit(image_quality,1,9);
					writePNGImage(&image_box,path,name,image_quality);
				break;
				case Image_File_Format::JPG:
					valueLimit(image_quality,30,100);
					writeJPGImage(&image_box,path,name,image_quality);
				break;
				default:
					image_file_format = Image_File_Format::PNG;
					image_quality = 8;
					writePNGImage(&image_box,path,name,image_quality);
			};
		}
		FREE(image_box.vram);
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
			case Rendering_Method::CPU_Rendering:
				if (fracData.type.abs_mandelbrot.polarMandelbrot == true) {
					renderCPU_Polar_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
				} else {
					renderCPU_ABS_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING, primaryRender.CPU_Threads);
				}
				break;
			case Rendering_Method::GPU_Rendering:
				renderOpenCL_ABS_Mandelbrot(&renderBox,primaryRender,fracData.type.abs_mandelbrot,ABORT_RENDERING);
				break;
			default:
			printfInterval(0.5,"Unknown rendering method %u",primaryRender.rendering_method);
		}
		if (fracData.type_value == Fractal_ABS_Mandelbrot || fracData.type_value == Fractal_Polar_Mandelbrot) {
			#define FRAC fracData.type.abs_mandelbrot
			fp128 cx00; fp128 cy00;
			fp128 cx11; fp128 cy11;
			fp128 cx01; fp128 cy01;
			fp128 cx10; fp128 cy10;
			i32 offX = (i32)(currentBuf->resX * primaryRender.subSample);
			i32 offY = (i32)(currentBuf->resY * primaryRender.subSample);
			fp64 extraPadding = 0.0;
			pixel_to_coordinate((i32)((fp64)offX * -extraPadding),(i32)((fp64)offY * -extraPadding),&cx00,&cy00,&FRAC,&primaryRender);
			pixel_to_coordinate((i32)((fp64)offX * (extraPadding + 1.0)),(i32)((fp64)offY * (extraPadding + 1.0)),&cx11,&cy11,&FRAC,&primaryRender);
			pixel_to_coordinate((i32)((fp64)offX * -extraPadding),(i32)((fp64)offY * (extraPadding + 1.0)),&cx01,&cy01,&FRAC,&primaryRender);
			pixel_to_coordinate((i32)((fp64)offX * (extraPadding + 1.0)),(i32)((fp64)offY * -extraPadding),&cx10,&cy10,&FRAC,&primaryRender);
			currentBuf->setTransformationData(cx00,cy00,cx11,cy11,cx01,cy01,cx10,cy10);
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

	int render_update_level = Change_Level::Full_Reset;
	uint64_t render_update_timecode = 0;
	while (QUIT_FLAG == false) {
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
			if (read_Abort_Render_Ongoing() == true) {
				write_Abort_Render_Ongoing(false);
			}
		}
		if (render_update_timecode == read_Update_Timecode() && ABORT_RENDERING == false) {
			clear_Update_Level();
			super_render_code(ABORT_RENDERING);
		} else {
			reset_Image_Render();
		}

		while (fracTime.timerReset() == false) {
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
	reset_Image_Render();
	write_Engine_Ready(true);
	while (read_Render_Ready() == false) {
		if (QUIT_FLAG == true) {
			printWarning("Engine thread exiting initialization: QUIT_FLAG == true");
			return -1;
		}
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