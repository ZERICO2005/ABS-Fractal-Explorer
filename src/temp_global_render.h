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
	#define RESX_Default 800
	#define RESY_Default 600
	#define RESX_Minimum 400
	#define RESY_Minimum 320
	#define RESX_Maximum 8192
	#define RESY_Maximum 4608
	#define RESX_Margin 16
	#define RESY_Margin 16

/* Screenshots */

	#define default_Super_Screenshot_ResX 1920
	#define default_Super_Screenshot_ResY 1080
	#define default_Super_Screenshot_Sample 3
	#define default_Super_Screenshot_MaxItr 16384
	#define default_Super_Screenshot_ThreadMultiplier 6

	extern int32_t super_screenshot_resX;
	extern int32_t super_screenshot_resY;
	extern int32_t super_screenshot_super_sample;
	extern int32_t super_screenshot_maxItr;
	extern int32_t super_screenshot_threadMultiplier;
	extern int32_t super_screenshot_maxThreads;

	extern Render_Data primarySuperRenderData;
	extern Render_Data secondarySuperRenderData;

/* ImGui Stuff */
	extern ImGuiIO* io_IMGUI;
	extern int32_t ImGui_WINDOW_MARGIN;
	extern ImGuiWindowFlags ImGui_WINDOW_FLAGS;

/* SDL2 */
	extern SDL_Renderer* renderer;
	extern SDL_Window* window;
	extern SDL_Texture* texture;
	extern SDL_Texture* kTexture; // Keyboard graphic

/* Rendering */
	
	#define Use_OpenCV_Scaler
	
	extern SDL_Texture* scale_tex;
	extern SDL_Surface* scale_surface;
	
	extern bool exportFractalBuffer;
	extern bool exportSuperFractalBuffer;

	extern BufferBox* rendered_buf;

	extern bool Abort_Rendering_Flag;
	extern bool Waiting_To_Abort_Rendering;
	extern ImageBuffer Master;
	
	extern BufferBox TestGraphic;
	#define fullColorTestGraphic
	extern fp64 TestGraphicSpeed;
	
	extern ImageBuffer* Primary_Image;
	extern ImageBuffer* Primary_Image_Preview;
	extern ImageBuffer* Secondary_Image;
	extern ImageBuffer* Secondary_Image_Preview;

	extern int Frame_Interpolation_Method;
	//#define MANUAL_FRAME_RATE_OVERRIDE
	extern fp64 FRAME_RATE; // Double the max screen refresh rate
	extern const fp64 FRAME_RATE_OFFSET;
	extern uint64_t FRAME_RATE_NANO;
	#define  Default_Frame_Rate_Multiplier 1.0
	extern const uint8_t color_square_divider; //5 dark, 4 dim, 3 ambient, 2 bright, 1 the sun
	extern fp64 DeltaTime;
	extern uint64_t END_SLEEP_HEADROOM;

	extern fp64 Frame_Time_Display;
	extern fp64 Render_Time_Display;

	extern uint32_t RESY_UI;

	extern uint64_t abortTimer; // How long it is taking to abort the rendering jobs

	extern bool bootup_Fractal_Frame_Rendered;

	extern TimerBox GUI_FrameTimer;
	extern uint64_t GUI_FrameTimeNano;
	extern fp64 GUI_FrameTime;
	extern fp64 GUI_FrameRate;

/* Fractals */

	extern Fractal_Data frac;
	extern Render_Data primaryRenderData;
	extern Render_Data secondaryRenderData;
	
	#define Update_Level(level); update_level = ((level) > update_level) ? (level) : update_level;

/* Images */

	extern Image_File_Format::Image_File_Format_Enum screenshotFileType;
	extern uint32_t User_PNG_Compression_Level;
	extern uint32_t User_JPG_Quality_Level;

/* Keyboard */

	extern const uint8_t* KEYS;

	struct Key_Status {
		SDL_Scancode key;
		bool pressed;
		uint64_t timePressed;
		uint64_t timeReleased;
	}; typedef struct Key_Status Key_Status;

	extern size_t KeyBind_PresetCount;
	extern std::list<KeyBind_Preset> KeyBind_PresetList;
	extern KeyBind_Preset* currentKBPreset;

	extern Key_Status Key_List[SDL_NUM_SCANCODES];

	extern Function_Status func_stat[Key_Function::Parameter_Function_Count];

/* Menus */

	enum Menu_Enum {GUI_Menu_None, GUI_Menu_Coordinates, GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds, GUI_Menu_Count};

	#define stretchValue(s) pow(2.0,-abs(s))
	#define zoomDefault(p) (-log10(getABSFractalMaxRadius((fp64)(p))) - 0.01)

	extern bool Lock_Key_Inputs;

	#define BufAndLen(x) x,ARRAY_LENGTH(x)

	extern int buttonSelection;
	extern bool ShowTheXButton;
	//bool yeildSwitch;

	static const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

	#ifndef BUILD_RELEASE
		static const char* buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
	#else
		static const char* buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds"};
	#endif

/* Displays */

	// Amount of displays detected
	extern uint32_t DISPLAY_COUNT;
	/* Display Bootup */
		namespace Display_Bootup {
			enum Display_Bootup_Enum {
				Automatic,First,Last,Specific,Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,HighResolution,HighFrameRate,LowResolution,LowFrameRate,Length
			};
		};
		//extern uint32_t SPECIFIC_BOOTUP_DISPLAY; // Supposed to be save data
		extern uint32_t Display_Match[Display_Bootup::Length];
		//extern Display_Bootup::Display_Bootup_Enum Display_Bootup_Type;
		extern bool useDefaultWindowSize;

	// struct DisplayInfo {
	// 	uint32_t resX;
	// 	uint32_t resY;
	// 	int32_t posX;
	// 	int32_t posY;
	// 	uint32_t refreshRate;
	// 	uint8_t bbp;
	// 	const char* name;
	// }; typedef struct DisplayInfo DisplayInfo;
	extern DisplayInfo* DisplayList;

	// Counts from ONE
	extern uint32_t CURRENT_DISPLAY;

/* Other */

	extern bool SaveUsernameInFiles; /* This MUST be False by Default */
	#define FileUsernameLength 32
	extern char FileUsername[FileUsernameLength];
	extern bool SaveHardwareInfoInFiles; /* This MUST be False by Default */
	extern User_Configuration_Data config_data;

#endif /* TEMP_GLOBAL_RENDER_H */