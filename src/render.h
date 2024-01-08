/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef RENDER_H
#define RENDER_H

#include "Common_Def.h"
#include "Program_Def.h"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/* Holds resolution, sampling, precision and rendering method */
struct _Render_Data {
	/* Dimensions */
	uint32_t resX;
	uint32_t resY;
	int32_t offsetX;
	int32_t offsetY;
	uint32_t subResX;
	uint32_t subResY;
	/* Format */
	uint32_t padding;
	uint8_t bpp;
	uint8_t channels;
	uint8_t flip;
	bool subPixelRendering;
	/* Rendering Parameters */
	uint32_t sample;
	uint32_t subSample;
	/* Preview */
	bool previewRender;
	fp64 areaMult;
	uint32_t resDiv;
	/* Rendering Method */
	bool export_Image;
	uint32_t rendering_method;
	uint32_t CPU_Precision;
	uint32_t CPU_Threads;
	uint32_t GPU_Precision;
	uint32_t GPU_Partitions;
}; typedef struct _Render_Data Render_Data;

namespace Rendering_Method {
	enum Rendering_Method_Enum {
		CPU_Rendering, GPU_Rendering, Rendering_Method_Count
	};
}

int setup_fracExpKB(int argc, char* argv[]);

int init_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex);

int terminate_Render();

void newFrame();

/* SDL_Scancode */
bool keyPressed(uint32_t key);

/* Framerate stuff */

void updateFrameTimeNano(uint64_t frameTime);
void updateFrameTime(fp64 frameTime);
void updateFrameRate(fp64 frameRate);

namespace Image_File_Format {
	enum Image_File_Format_Enum {
		PNG,JPG,TGA,BMP,HDR,Image_File_Format_Count
	};
}

#endif /* RENDER_H */