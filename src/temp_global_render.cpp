/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "temp_global_render.h"

#include "render.h"

#include "programData.h"
#include "copyBuffer.h"
#include "fractal.h"
#include "keybind.h"
#include "engine.h"
#include "fracExpKB.h"
#include "fileManager.h"
#include "imageBuffer.h"
#include "imageTransform.h"

#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "user_data.h"

#include "display_GUI.h"

/* Resolution */
	// #define RESX_Default 800
	// #define RESY_Default 600
	// #define RESX_Minimum 400
	// #define RESY_Minimum 320
	// #define RESX_Maximum 8192
	// #define RESY_Maximum 4608
	// #define RESX_Margin 16
	// #define RESY_Margin 16

/* Screenshots */

	// #define default_Super_Screenshot_ResX 1920
	// #define default_Super_Screenshot_ResY 1080
	// #define default_Super_Screenshot_Sample 3
	// #define default_Super_Screenshot_MaxItr 16384
	// #define default_Super_Screenshot_ThreadMultiplier 6

	int32_t super_screenshot_resX = default_Super_Screenshot_ResX;
	int32_t super_screenshot_resY = default_Super_Screenshot_ResY;
	int32_t super_screenshot_super_sample = default_Super_Screenshot_Sample;
	int32_t super_screenshot_maxItr = default_Super_Screenshot_MaxItr;
	int32_t super_screenshot_threadMultiplier = default_Super_Screenshot_ThreadMultiplier;
	int32_t super_screenshot_maxThreads = 1;

	Render_Data primarySuperRenderData = {0};
	Render_Data secondarySuperRenderData = {0};

/* ImGui Stuff */
	ImGuiIO* io_IMGUI = nullptr;
	int32_t ImGui_WINDOW_MARGIN = 8;

/* SDL2 */
	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_Texture* kTexture = nullptr; // Keyboard graphic

/* Rendering */
	
	//#define Use_OpenCV_Scaler
	
	SDL_Texture* scale_tex = nullptr;
	SDL_Surface* scale_surface = nullptr;
	
	bool exportFractalBuffer = false;
	bool exportSuperFractalBuffer = false;

	BufferBox* rendered_buf = nullptr;

	bool Abort_Rendering_Flag = false;
	bool Waiting_To_Abort_Rendering = false;
	ImageBuffer Master;
	
	BufferBox TestGraphic = {0};
	
	ImageBuffer* Primary_Image = nullptr;
	ImageBuffer* Primary_Image_Preview = nullptr;
	ImageBuffer* Secondary_Image = nullptr;
	ImageBuffer* Secondary_Image_Preview = nullptr;

	//#define MANUAL_FRAME_RATE_OVERRIDE
	fp64 FRAME_RATE = 60.0; // Double the max screen refresh rate
	const fp64 FRAME_RATE_OFFSET = 0.01;
	nano64_t FRAME_RATE_NANO = 0;
	//#define  Default_Frame_Rate_Multiplier 1.0
	fp64 DeltaTime = 0.0;
	nano64_t END_SLEEP_HEADROOM = SECONDS_TO_NANO(0.02);

	fp64 Frame_Time_Display = 0.0;
	fp64 Render_Time_Display = 0.0;

	uint32_t RESY_UI = 128;

	nano64_t abortTimer = 0; // How long it is taking to abort the rendering jobs
	
	bool bootup_Fractal_Frame_Rendered = false;

	TimerBox GUI_FrameTimer;
	nano64_t GUI_FrameTimeNano = SECONDS_TO_NANO(1.0/60.0);
	fp64 GUI_FrameTime = 1.0/60.0;
	fp64 GUI_FrameRate = 60.0;

/* Fractals */

	Fractal_Data frac = {0};
	Render_Data primaryRenderData = {0};
	Render_Data secondaryRenderData = {0};
	
	//#define Update_Level(level); update_level = ((level) > update_level) ? (level) : update_level;

/* Keyboard */

	const uint8_t* SDL_Keyboard_State = nullptr;

	// struct Key_Status {
	// 	SDL_Scancode key;
	// 	bool pressed;
	// 	uint64_t timePressed;
	// 	uint64_t timeReleased;
	// }; typedef struct Key_Status Key_Status;

	size_t KeyBind_PresetCount = 0;
	std::list<KeyBind_Preset> KeyBind_PresetList;
	KeyBind_Preset* currentKBPreset = nullptr;

	Key_Status Key_List[SDL_NUM_SCANCODES];

	Function_Status func_stat[Key_Function::Parameter_Function_Count];

/* Menus */

	//enum Menu_Enum {GUI_Menu_None, GUI_Menu_Coordinates, GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds, GUI_Menu_Count};

	bool Lock_Key_Inputs = false;

	//#define BufAndLen(x) x,ARRAY_LENGTH(x)

	int buttonSelection = -1;
	bool ShowTheXButton = true;
	//bool yeildSwitch = true;

	// const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

	// #ifndef BUILD_RELEASE
	// 	const char* buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
	// #else
	// 	const char* buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds"};
	// #endif

/* Other */

	User_Configuration_Data config_data = {0};
	bool SaveUsernameInFiles = false; /* This MUST be False by Default */
	//#define FileUsernameLength 32
	char FileUsername[FileUsernameLength] = {0};
	bool SaveHardwareInfoInFiles = false; /* This MUST be False by Default */