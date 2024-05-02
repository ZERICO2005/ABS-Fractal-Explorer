/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef TEMP_GLOBAL_RENDER_H
#define TEMP_GLOBAL_RENDER_H

#include "Common_Def.h"
#include "Program_Def.h"
#include "render.h"

#include "programData.h"
#include "copyBuffer.h"
#include "fractal.h"
#include "keybind.h"
#include "engine.h"
#include "fracExp_Files/fracExpKB.h"
#include "fileManager.h"
#include "imageBuffer.h"
#include "imageTransform.h"

#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "user_data.h"

#include "menu_Interface/display_GUI.h"

#include "framePacer.hpp"

#include "render_Configuration.hpp"

extern Frame_Pacer GUI_Frame_Pacer;

/* Resolution */
	constexpr dim32_t RESX_Default = 800;
	constexpr dim32_t RESY_Default = 600;
	constexpr dim32_t RESX_Minimum = 400;
	constexpr dim32_t RESY_Minimum = 320;
	constexpr dim32_t RESX_Maximum = 8192;
	constexpr dim32_t RESY_Maximum = 4608;
	constexpr dim32_t RESX_Margin = 16;
	constexpr dim32_t RESY_Margin = 16;

/* Screenshots */

	#define default_Super_Screenshot_ResX 1920
	#define default_Super_Screenshot_ResY 1080
	#define default_Super_Screenshot_Sample 3
	#define default_Super_Screenshot_MaxItr 16384
	#define default_Super_Screenshot_ThreadMultiplier 6

	extern dim32_t super_screenshot_resX;
	extern dim32_t super_screenshot_resY;
	extern int32_t super_screenshot_super_sample;
	extern uint32_t super_screenshot_maxItr;
	extern int32_t super_screenshot_threadMultiplier;
	extern int32_t super_screenshot_maxThreads;

	extern Render_Data primarySuperRenderData;
	extern Render_Data secondarySuperRenderData;

/* ImGui Stuff */
	extern ImGuiIO* io_IMGUI;
	extern dim32_t ImGui_WINDOW_MARGIN;

/* SDL2 */
	extern SDL_Renderer* renderer;
	extern SDL_Window* window;
	extern SDL_Texture* texture;
	extern SDL_Texture* kTexture; // Keyboard graphic

/* Rendering */

	// Rounds the initial screen resolution for debug purposes
	//#define ROUND_INIT_WINDOW_RESX
	
	extern Render_Configurator Render_Config;

	extern SDL_Texture* scale_tex;
	extern SDL_Surface* scale_surface;
	
	extern bool exportFractalBuffer;
	extern bool exportSuperFractalBuffer;

	extern BufferBox* rendered_buf;

	extern bool Abort_Rendering_Flag;
	extern bool Waiting_To_Abort_Rendering;
	extern ImageBuffer Master;
	
	extern ImageBuffer* Primary_Image;
	extern ImageBuffer* Primary_Image_Preview;
	extern ImageBuffer* Secondary_Image;
	extern ImageBuffer* Secondary_Image_Preview;

	extern fp64 DeltaTime;
	extern nano64_t END_SLEEP_HEADROOM;

	extern fp64 Frame_Time_Display;
	extern fp64 Render_Time_Display;

	extern dim32_t RESY_UI;

	extern nano64_t abortTimer; // How long it is taking to abort the rendering jobs

	extern bool bootup_Fractal_Frame_Rendered;

	extern TimerBox GUI_FrameTimer;
	extern nano64_t GUI_FrameTimeNano;
	extern fp64 GUI_FrameTimeSeconds;
	extern fp64 GUI_FrameRate;

/* Fractals */

	extern ABS_Mandelbrot current_Fractal;
	extern Render_Data primaryRenderData;
	extern Render_Data secondaryRenderData;

/* Keyboard */

	extern const uint8_t* SDL_Keyboard_State;

	struct Key_Status {
		SDL_Scancode key;
		bool pressed;
		nano64_t timePressed;
		nano64_t timeReleased;
	}; typedef struct Key_Status Key_Status;

	extern size_t KeyBind_PresetCount;
	extern std::list<KeyBind_Preset> KeyBind_PresetList;
	extern KeyBind_Preset* currentKBPreset;

	extern Key_Status Key_List[SDL_NUM_SCANCODES];

	extern Function_Status func_stat[Key_Function::Parameter_Function_Count];

/* Menus */

	enum Menu_Enum {
		GUI_Menu_None, GUI_Menu_Coordinates, GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds, GUI_Menu_Status,
		GUI_Menu_Count
	};
	
	extern bool Lock_Key_Inputs;

	#define BufAndLen(x) x,ARRAY_LENGTH(x)

	extern int buttonSelection;
	extern bool ShowTheXButton;
	//bool yeildSwitch;

	const char* const WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

	#ifndef BUILD_RELEASE
		const char* const buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds", "Status"};
	#else
		const char* const buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds", "Status"};
	#endif

/* Other */

	extern bool SaveUsernameInFiles; /* This MUST be False by Default */
	#define FileUsernameLength 32
	extern char FileUsername[FileUsernameLength];
	extern bool SaveHardwareInfoInFiles; /* This MUST be False by Default */
	extern User_Configuration_Data config_data;


#endif /* TEMP_GLOBAL_RENDER_H */