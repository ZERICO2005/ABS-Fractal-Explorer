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

#include "render_CPU/frac_Multi.h"

#include "render_GPU/fracCL.h"

#include "render_Configuration.hpp"

TimerBox fracTime;
ABS_Mandelbrot fracData;
Render_Data primaryRender;
Render_Data secondaryRender;
ImageBuffer* currentBuf = nullptr;
ImageBuffer* previewBuf = nullptr;

static Render_Configurator Engine_Config;
static Render_Configurator Super_Engine_Config;

void get_GPU_Hardware_Hash(uint64_t& hash) { 
	//calculate_GPU_Hardware_Hash(hash);
}

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
	static ABS_Mandelbrot image_fractal_data = {0};
	static BufferBox image_box = {0};
	if (receive_Image_Render(&image_fractal_data,&image_render_data,&image_file_format,&image_quality)) {
		Super_Engine_Config.suggest_Render_Preset(
			(Rendering_Configuration::Rendering_Preset)image_render_data.render_preset
		);
		memset(&image_box,0,sizeof(BufferBox));
		image_box.resX = image_render_data.resX;
		image_box.resY = image_render_data.resY;
		image_box.channels = image_render_data.channels;
		image_box.padding = (uint8_t)image_render_data.padding;
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
		memset(image_box.vram, 0, image_box_size);

		printf("\n\nRendering Super Screenshot:");
		printf("\n\t%" PRIu32 "x%" PRIu32 " %" PRIu32 " samples", image_box.resX, image_box.resY, image_render_data.sample * image_render_data.sample);
		printf(", %" PRIu32 " iterations", image_fractal_data.maxItr);
		if (Super_Engine_Config.current_Render_Method_CPU() == true) {
			printf("\n\tFP%zu CPU rendering, %" PRIu32 " threads", Super_Engine_Config.get_Current_Float_Size(), image_render_data.CPU_Threads);
		} else if (Super_Engine_Config.current_Render_Method_GPU() == true) {
			printf("\n\tFP%zu GPU rendering", Super_Engine_Config.get_Current_Float_Size());
		}
		printf("\n\tClick \"Abort Rendering\" (or use task manager) to cancel.");
		fflush(stdout);
		nano64_t image_stopwatch = getNanoTime();

		if (Super_Engine_Config.current_Render_Method_CPU() == true) {
			renderCPU_ABS_Mandelbrot(
				&image_box, image_render_data, image_fractal_data,
				Super_Engine_Config,
				ABORT_RENDERING, primaryRender.CPU_Threads
			);
		} else if (Super_Engine_Config.current_Render_Method_GPU() == true) {
			render_OpenCL_ABS_Mandelbrot(
				&image_box, image_render_data, image_fractal_data,
				ABORT_RENDERING
			);
		} else {
			printfInterval(0.5,"Error: Super Screenshot, unknown rendering method %" PRId32, Super_Engine_Config.get_Render_Method());
		}

		nano64_t image_render_time = getNanoTime() - image_stopwatch;
		int32_t time_mili = (int32_t)((nano64_t)image_render_time / (((nano64_t)1000000)) % (nano64_t)1000);
		int32_t time_seconds = (int32_t)((nano64_t)(image_render_time / ((nano64_t)1000000 * (nano64_t)1000)) % (nano64_t)60);
		int32_t time_minutes = (int32_t)((nano64_t)(image_render_time / ((nano64_t)1000000 * (nano64_t)1000 * (nano64_t)60)) % (nano64_t)60);
		int32_t time_hours = (int32_t)(image_render_time / ((nano64_t)1000000 * (nano64_t)1000 * (nano64_t)60 * (nano64_t)60));
		printFlush("\n\tRendered in: %02" PRId32 ":%02" PRId32 ":%02" PRId32 ".%03" PRId32,time_hours,time_minutes,time_seconds,time_mili);		
		printFlush("\n\tSaving Super Screenshot");
		{
			nano64_t curTime = getNanoTime();
			curTime /= 1000;
			char id_number[64]; memset(id_number,'\0',sizeof(id_number));
			snprintf(id_number,sizeof(id_number),"_id-%" PRIu64,image_fractal_data.formula);
			const char* fractal_name = (image_fractal_data.polarMandelbrot == true) ? FractalTypeFileText[Fractal_Polar_Mandelbrot] : FractalTypeFileText[Fractal_ABS_Mandelbrot];
			const char* format_str = "Super_%s-%s_(%" PRId64 ")";
			size_t size = (size_t)snprintf(nullptr, 0, format_str, fractal_name, id_number,curTime);
			size++;
			char* name = (char*)calloc(size,sizeof(char));
			snprintf(name, size, format_str, fractal_name, id_number, curTime);
			std::string path = read_Screenshot_Path();
			switch(image_file_format) {
				case Image_File_Format::PNG:
					valueRestore(image_quality, 1, 9, 8);
					writePNGImage(&image_box, path.c_str(), name, image_quality);
					break;
				case Image_File_Format::JPG:
					valueRestore(image_quality, 30, 100, 95);
					writeJPGImage(&image_box, path.c_str(), name, image_quality);
					break;
				default:
					image_file_format = Image_File_Format::PNG;
					image_quality = 8;
					writePNGImage(&image_box, path.c_str(), name, image_quality);
			};
			FREE(name);
		}
		FREE(image_box.vram);
	}
	return 0;
}

int render_Engine(std::atomic<bool>& ABORT_RENDERING) {
	if (currentBuf == nullptr || currentBuf->vram == nullptr || currentBuf->allocated() == false) {
		return -1;
	}
	using namespace Key_Function;
	static BufferBox renderBox;
	currentBuf->getBufferBox(&renderBox);
	//render_ABS_Mandelbrot(currentBuf,primaryRender,fracData.type.abs_mandelbrot);
	if (ABORT_RENDERING == false) {
		//ABS_Mandelbrot& FRAC = fracData.type.abs_mandelbrot;
		//printfInterval(0.4,"\nr: %.6lf i: %.6lf zoom: 10^%.4lf maxItr: %u formula: %" PRIu64,FRAC.r,FRAC.i,FRAC.zoom,FRAC.maxItr,FRAC.formula);

		if (Engine_Config.current_Render_Method_CPU() == true) {
			renderCPU_ABS_Mandelbrot(
				&renderBox, primaryRender, fracData,
				Engine_Config,
				ABORT_RENDERING, primaryRender.CPU_Threads
			);
		} else if (Engine_Config.current_Render_Method_GPU() == true) {
			render_OpenCL_ABS_Mandelbrot(
				&renderBox, primaryRender, fracData,
				ABORT_RENDERING
			);
		} else {
			printfInterval(0.5, "Unknown rendering method %" PRId32, Engine_Config.get_Render_Method());
		}
		{
			const ABS_Mandelbrot& FRAC = fracData;
			fpCord cx00; fpCord cy00;
			fpCord cx11; fpCord cy11;
			fpCord cx01; fpCord cy01;
			fpCord cx10; fpCord cy10;
			int32_t offX = (int32_t)(currentBuf->resX * primaryRender.subSample);
			int32_t offY = (int32_t)(currentBuf->resY * primaryRender.subSample);
			fp64 extraPadding = 0.0;
			pixel_to_coordinate((int32_t)((fp64)offX * -extraPadding       ), (int32_t)((fp64)offY * -extraPadding       ), cx00, cy00, FRAC, primaryRender.resX, primaryRender.resY);
			pixel_to_coordinate((int32_t)((fp64)offX * (extraPadding + 1.0)), (int32_t)((fp64)offY * (extraPadding + 1.0)), cx11, cy11, FRAC, primaryRender.resX, primaryRender.resY);
			pixel_to_coordinate((int32_t)((fp64)offX * -extraPadding       ), (int32_t)((fp64)offY * (extraPadding + 1.0)), cx01, cy01, FRAC, primaryRender.resX, primaryRender.resY);
			pixel_to_coordinate((int32_t)((fp64)offX * (extraPadding + 1.0)), (int32_t)((fp64)offY * -extraPadding       ), cx10, cy10, FRAC, primaryRender.resX, primaryRender.resY);
			currentBuf->setTransformationData(cx00,cy00,cx11,cy11,cx01,cy01,cx10,cy10);
			currentBuf->setFractalState(FRAC.r, FRAC.i, FRAC.zoom, FRAC.rot, FRAC.sX, FRAC.sY);
		}
	}
	return 0;
}

int start_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
	using namespace Key_Function;
	
	fracTime.setFreq(read_FrameTime());
	nano64_t deltaTime = 0.0;

	// int_enum render_update_level = Change_Level::Full_Reset;
	nano64_t render_update_timecode = 0;
	while (QUIT_FLAG == false) {
		/* Update things */
		read_Parameters(&fracData,&primaryRender,&secondaryRender);
		//render_update_level = read_Update_Level();

		if (render_update_timecode != read_Update_Timecode() && ABORT_RENDERING == false) {
			render_update_timecode = read_Update_Timecode();
			if (currentBuf != nullptr) {
				BufferBox sizeBuf = read_Buffer_Size();
				currentBuf->resizeBuffer(
					sizeBuf.resX / (dim32_t)primaryRender.subSample,
					sizeBuf.resY / (dim32_t)primaryRender.subSample,
					sizeBuf.channels
				);
			}
			Engine_Config.suggest_Render_Preset(
				(Rendering_Configuration::Rendering_Preset)primaryRender.render_preset
			);
			// printfInterval(0.3,"\nEngine: %s | %s %s",
			// 	Rendering_Configuration::Rendering_Preset_Name[Engine_Config.get_Render_Preset()],
			// 	Rendering_Configuration::Rendering_Preset_Name[primaryRender.render_preset],
			// 	Rendering_Configuration::Rendering_Preset_Name[secondaryRender.render_preset]
			// );
			//printFlush("\nRender: %07llu",(render_update_timecode/1000) % 10000000);
			render_Engine(ABORT_RENDERING);
			//printFlush("\nExport: %07llu",(render_update_timecode/1000) % 10000000);
			next_Write_Cycle_Pos(&currentBuf, Primary_Full);
			deltaTime = fracTime.getDeltaTimeNano();
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
		
		fracTime.setFreq(read_FrameTime());

		while (fracTime.timerReset() == false) {
			if (read_Abort_Render_Ongoing() == true) {
				write_Abort_Render_Ongoing(false);
			}
		}
	}
	return 0;
}

bool init_GPU_Renderer(bool& GPU_Float16, bool& GPU_Float32, bool& GPU_Float64) {
	#ifdef Enable_OpenCL
		int32_t init_OpenCL_ret = init_OpenCL();
		if (init_OpenCL_ret != 0) {
			printError("OpenCL failed to initialize, error code: %d", init_OpenCL_ret);
			{ /* GPU Float Support */
				GPU_Float16 = false;
				GPU_Float32 = false;
				GPU_Float64 = false;
			}
			return false;
		}
		query_OpenCL_GPU();
		{ /* GPU Float Support */
			GPU_Float16 = false;
			GPU_Float32 = true;
			GPU_Float64 = false;
		}
		return true;
	#else 
		printFlush("\nNote: OpenCL GPU rendering is disabled");

		{ /* GPU Float Support */
			GPU_Float16 = false;
			GPU_Float32 = false;
			GPU_Float64 = false;
		}
		return false;
	#endif
}

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
	bool GPU_Float16 = false;
	bool GPU_Float32 = false;
	bool GPU_Float64 = false;
	bool OpenCL_Initialized = init_GPU_Renderer(
		GPU_Float16,
		GPU_Float32,
		GPU_Float64
	);

	Engine_Config.reset_Render_Configurator(GPU_Float16, GPU_Float32, GPU_Float64);
	Super_Engine_Config.reset_Render_Configurator(GPU_Float16, GPU_Float32, GPU_Float64);
	if (OpenCL_Initialized == true) {
		Engine_Config.suggest_Render_Preset(Rendering_Configuration::Render_Preset_GPU_Float32);
		Super_Engine_Config.suggest_Render_Preset(Rendering_Configuration::Render_Preset_GPU_Float32);
	} else {
		Engine_Config.suggest_Render_Precision(Rendering_Configuration::Render_Precision_Float64);
		Super_Engine_Config.suggest_Render_Precision(Rendering_Configuration::Render_Precision_Float64);
	}

	clear_Cycle_Buffers();
	reset_Image_Render();
	write_Engine_Render_Configuration(Engine_Config);
	write_Engine_Ready(true);
	
	while (read_Render_Ready() == false) {
		if (QUIT_FLAG == true) {
			printWarning("Engine thread exiting initialization: QUIT_FLAG == true");
			return -1;
		}
		std::this_thread::yield();
	}
	
	start_Engine(QUIT_FLAG,ABORT_RENDERING);
	return 0;
}

int terminate_Engine() {
	#ifdef Enable_OpenCL
		terminate_OpenCL();
	#endif
	delete_Cycle_Buffers();
	return 0;
}