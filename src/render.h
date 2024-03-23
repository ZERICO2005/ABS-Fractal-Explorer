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

#include "display_GUI.h"

/* Holds resolution, sampling, precision and rendering method */
struct Render_Data {
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
}; typedef struct Render_Data Render_Data;

namespace Rendering_Method {
	enum Rendering_Method_Enum {
		CPU_Rendering, GPU_Rendering, Rendering_Method_Count
	};
}

namespace OPENCV_Interpolation {
	enum OPENCV_Interpolation_Enum {
		OPENCV_INTER_NEAREST, OPENCV_INTER_LINEAR, OPENCV_INTER_CUBIC, OPENCV_INTER_AREA, OPENCV_INTER_LANCZOS4, OPENCV_INTER_COUNT
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

/* ================ */

// #include "copyBuffer.h"
// #include "fractal.h"
// #include "keybind.h"
// #include "engine.h"
// #include "fracExpKB.h"
// #include "fileManager.h"
// #include "imageBuffer.h"
// #include "imageTransform.h"

// #include <SDL2/SDL.h>
// #include <opencv2/opencv.hpp>

// #include "imgui.h"
// #include "imgui_impl_sdl2.h"
// #include "imgui_impl_sdlrenderer2.h"
// #include "programData.h"
// #include "user_data.h"

int exportScreenshot();
int exportSuperScreenshot();

// /* Fractal movement stuff */
// void moveCord(fp128* x, fp128* y, fp64 angle, fp64 speed);

// void moveCord(fp64* x, fp64* y, fp64 angle, fp64 speed);
// bool funcTimeDelay(Key_Function::Key_Function_Enum func);

// bool funcTimeDelay(Key_Function::Key_Function_Enum func, fp64 freq);

// void correctFrameTime();

// void updateFrameTimeNano(uint64_t frameTime);

// void updateFrameTime(fp64 frameTime);

// void updateFrameRate(fp64 frameRate);

// int utitledFileNameGenerator(char* buf, size_t maxLen);

void correctUsernameText(char* buf, size_t len);

// void correctTextFloat(char* buf, size_t len, uint8_t level);

/* Display */

struct DisplayInfo_Legacy {
	uint32_t resX;
	uint32_t resY;
	int32_t posX;
	int32_t posY;
	uint32_t refreshRate;
	uint8_t bbp;
	const char* name;
}; typedef struct DisplayInfo_Legacy DisplayInfo_Legacy;

DisplayInfo_Legacy* getDisplayInfo(size_t i = 1);
DisplayInfo_Legacy* getCurrentDisplayInfo();

/* Keys */

void updateKeys();
void recolorKeyboard();
void initKeys();
bool keyPressed(uint32_t key);
int setup_fracExpKB(int argc, char* argv[]);

void init_KeyBind_PresetList();
void clear_KeyBind_PresetList();
int get_currentKBPreset_Pos();
void set_currentKBPreset_Pos(int pos);
void remove_currentKBPreset();


#endif /* RENDER_H */