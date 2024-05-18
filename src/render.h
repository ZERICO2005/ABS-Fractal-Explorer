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

#include "menu_Interface/display_GUI.h"
#include "displayInfo.h"

#include <SDL_keycode.h>
#include <SDL_scancode.h>

/* Holds resolution, sampling, precision and rendering method */
struct Render_Data {
	/* Dimensions */
	dim32_t resX;
	dim32_t resY;
	int32_t offsetX;
	int32_t offsetY;
	dim32_t subResX;
	dim32_t subResY;
	/* Format */
	size_t padding;
	uint8_t bpp;
	size_t channels;
	uint8_t flip;
	bool subPixelRendering;
	/* Rendering Parameters */
	int32_t sample;
	int32_t subSample;
	/* Preview */
	bool previewRender;
	fp64 areaMult;
	uint32_t resDiv;
	/* Rendering Method */
	bool export_Image;
	//uint32_t rendering_method;
	uint32_t CPU_Threads;
	uint32_t GPU_Partitions;

	int_enum render_precision;
	int_enum render_method;
	int_enum render_preset;

}; typedef struct Render_Data Render_Data;

namespace Legacy_Rendering_Method {
	enum Legacy_Rendering_Method_Enum {
		CPU_Rendering, GPU_Rendering, Rendering_Method_Count
	};
}

namespace OPENCV_Interpolation {
	enum OPENCV_Interpolation_Enum {
		OPENCV_INTER_NEAREST, OPENCV_INTER_LINEAR, OPENCV_INTER_CUBIC, OPENCV_INTER_AREA, OPENCV_INTER_LANCZOS4, OPENCV_INTER_COUNT
	};		
}

int setup_fracExpKB(int argc, char* argv[]);

int init_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING);

int terminate_Render();

void newFrame();

/* Framerate stuff */

void updateFrameTimeNano(nano64_t frameTime);
void updateFrameTimeSeconds(fp64 frameTime);
void updateFrameRate(fp64 frameRate);

namespace Image_File_Format {
	enum Image_File_Format_Enum {
		PNG,JPG,TGA,BMP,HDR,Image_File_Format_Count
	};
}

namespace Namespace_Image_Render_Bounding_Box {
	enum Enum_Image_Render_Bounding_Box {
		Fill_Area, Fit_Area, Fit_Width, Fit_Height, Expand_from_Center, // Stretch_to_Area
		Image_Render_Bounding_Box_Count
	};
	const char* const Image_Render_Bounding_Box_Text[] = {"Fill Area", "Fit Area", "Fit Width", "Fit Height", "Expand from Center"/*, "Stretch to Area"*/};
};

uint64_t get_Hardware_Hash();

/* Window Configuration */

void set_Window_Fullscreen_Mode(Display_Fullscreen::Display_Fullscreen_Enum fullscreen_mode);

Display_Fullscreen::Display_Fullscreen_Enum get_Window_Fullscreen_Mode();

void toggle_Window_Fullscreen_Mode();

void calculate_init_window_size(
	const dim32_t& dispResX, const dim32_t& dispResY,
	dim32_t& initResX, dim32_t& initResY,
	dim32_t& initPosX, dim32_t& initPosY
);

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

// void updateFrameTimeNano(nano64_t frameTime);

// void updateFrameTime(fp64 frameTime);

// void updateFrameRate(fp64 frameRate);

// int utitledFileNameGenerator(char* buf, size_t maxLen);

void correctUsernameText(char* buf, size_t len);

// void correctTextFloat(char* buf, size_t len, uint8_t level);

/* Keys */

void updateKeys();
void recolorKeyboard();
void initKeys();
bool keyPressed(SDL_Scancode key);
int setup_fracExpKB(int argc, char* argv[]);

void init_KeyBind_PresetList();
void clear_KeyBind_PresetList();
int32_t get_currentKBPreset_Pos();
void set_currentKBPreset_Pos(int pos);
void remove_currentKBPreset();

#endif /* RENDER_H */