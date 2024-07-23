/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "render.h"
#include "temp_global_render.h"

#include "copyBuffer.h"

#include "fractal.h"
#include "keybind.h"
#include "engine.h"
#include "fracExp_Files/fracExpKB.h"
#include "fileManager.h"

#include "imageBuffer.h"
#ifdef Enable_OpenCV_Scaler
	#include "imageTransform.h"
#endif
#include "frame_Transformation.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "programData.h"
#include "user_data.h"

#include "menu_Interface/display_GUI.h"
#include "displayInfo.h"

#include "fnv1a_hash.hpp"

#include "update_Parameters.h"

static bool force_quit_flag = false;

constexpr uint8_t color_square_divider = 2; // 5 dark, 4 dim, 3 ambient, 2 bright, 1 the sun

// ImageBuffer primaryFracImage;
// ImageBuffer secondaryFracImage;


int exportScreenshot();
int exportSuperScreenshot();

void updateRenderData(Render_Data* rDat) {
	if (rDat == NULL) { return; }
	rDat->resX = Master.resX;
	rDat->resY = Master.resY - RESY_UI;
	rDat->padding = 0;
	rDat->channels = Master.channels;
}
 
void initRenderData(Render_Data* rDat) {
	if (rDat == NULL) { return; }
	updateRenderData(rDat);
	rDat->offsetX = 0;
	rDat->offsetY = 0;
	rDat->subResX = 0;
	rDat->subResY = 0;
	rDat->bpp = 8;
	rDat->flip = 0;
	rDat->subPixelRendering = false;
	rDat->sample = 1;
	rDat->subSample = 1;
	rDat->previewRender = false;
	rDat->areaMult = 1.0;
	rDat->resDiv = 1;
	if ((uint32_t)std::thread::hardware_concurrency() <= 1) {
		rDat->CPU_Threads = 1;
	} else {
		rDat->CPU_Threads = (uint32_t)std::thread::hardware_concurrency() - 1;
	}
	rDat->GPU_Partitions = 1;
	rDat->export_Image = false;
	rDat->export_Image = false;
}

void Bootup_initRenderData() {
	initRenderData(&primaryRenderData);
	initRenderData(&secondaryRenderData);
	initRenderData(&primarySuperRenderData);
	initRenderData(&secondarySuperRenderData);
	primarySuperRenderData.resX = default_Super_Screenshot_ResX;
	primarySuperRenderData.resY = default_Super_Screenshot_ResY;
	primarySuperRenderData.sample = default_Super_Screenshot_Sample;
	secondarySuperRenderData.resX = default_Super_Screenshot_ResX;
	secondarySuperRenderData.resY = default_Super_Screenshot_ResY;
	secondarySuperRenderData.sample = default_Super_Screenshot_Sample;
}

/* Keyboard and Scancodes */

void process_SDL2_event(__attribute__((unused)) SDL_Event& event) {
	// switch (event.type) {
	// 	case SDL_KEYDOWN:
	// 	case SDL_KEYUP:

	// 	default:
	// 		return;
	// }
}

// const uint8_t* SDL_Keyboard_State;

// struct _Key_Status {
// 	SDL_Scancode key;
// 	bool pressed;
// 	uint64_t timePressed;
// 	uint64_t timeReleased;
// }; typedef struct _Key_Status Key_Status;

//size_t KeyBind_PresetCount;
//KeyBind_Preset* KeyBind_List;
// KeyBind_Preset defaultKeyBind = {"Default Key-bind",ARRAY_LENGTH(defaultKeyBind),defaultKeyBind};
// KeyBind_Preset importedKeyBind = {"Blank",0,NULL};
// KeyBind_Preset* currentKeyBind = &defaultKeyBind;

//std::list<KeyBind> currentKeyBind = defaultKeyBind;
//std::list<KeyBind> importedKeyBind; // Deprecate this

/* KeyBind_Preset */
	// std::list<KeyBind_Preset> KeyBind_PresetList;
	// KeyBind_Preset* currentKBPreset;

	void init_KeyBind_PresetList() {
		if (KeyBind_PresetList.empty() == true) {
			KeyBind_Preset temp_KeyBind;
			initDefaultKeyBind(&temp_KeyBind.kList);
			temp_KeyBind.name = "Default";
			KeyBind_PresetList.push_front(temp_KeyBind);
			currentKBPreset = &KeyBind_PresetList.front();
		}
	}
	void clear_KeyBind_PresetList() {
		KeyBind_PresetList.clear();
	}
	int32_t get_currentKBPreset_Pos() {
		std::size_t index = 0;
		for (const auto& element : KeyBind_PresetList) {
			if (&element == currentKBPreset) {
				return (int32_t)index;
			}
			index++;
		}
		return -1;
	}
	void set_currentKBPreset_Pos(int pos) {
		if (pos < 0 || pos >= (int)KeyBind_PresetList.size()) {
			return;
		}
		std::list<KeyBind_Preset>::iterator iterKBP = KeyBind_PresetList.begin();
		std::advance(iterKBP, pos);
		if (iterKBP != KeyBind_PresetList.end()) {
			currentKBPreset = &(*iterKBP);
		}
	}
	void remove_currentKBPreset() {
		if (KeyBind_PresetList.size() <= 1) {
			return;
		}
		int pos = 0;
		for (std::list<KeyBind_Preset>::iterator iterKBP = KeyBind_PresetList.begin(); iterKBP != KeyBind_PresetList.end(); iterKBP++) {
			if (currentKBPreset == &(*iterKBP)) {
				KeyBind_PresetList.erase(iterKBP);
				if (pos == 0) {
					currentKBPreset = &KeyBind_PresetList.front();
				} else {
					set_currentKBPreset_Pos(pos - 1);
				}
				return;
			}
			pos++;
		}
	}
	const char* currentKBPreset_Name() {
		return currentKBPreset->name.c_str();
	}
	std::list<KeyBind> currentKeyBind() {
		return currentKBPreset->kList;
	}

// Key_Status Key_List[SDL_NUM_SCANCODES];

// Function_Status func_stat[Key_Function::Parameter_Function_Count];

void updateKeys() {
	for (size_t t = 0; t < ARRAY_LENGTH(func_stat); t++) {
		func_stat[t].triggered = false;
	}
	SDL_Keyboard_State = SDL_GetKeyboardState(NULL);
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (SDL_Keyboard_State[i] != 0) { // Key Pressed
			if (Key_List[i].pressed == false) {
				Key_List[i].timePressed = getNanoTime();
				Key_List[i].pressed = true;
			}
		} else {
			Key_List[i].timeReleased = getNanoTime();
			Key_List[i].pressed = false;
		}

		for (const auto& bind : currentKBPreset->kList) {
			if (bind.key == (SDL_Scancode)i) {
				Key_Function::Key_Function_Enum func = bind.func;
				if (Lock_Key_Inputs == true) { // Only listens to FUNCTION key-binds when key inputs are locked
					if (func <= Key_Function::FORMULA /* Key_Function::SCREEN_SPLIT */ || func >= Key_Function::FUNCTIONS) {
						continue;
					}
				}
				if (Key_List[i].pressed == true) {
						func_stat[func].triggered = true;
				}
			}
		}
	}
	//write_Function_Status(func_stat);
}

void recolorKeyboard() {
	{
		using namespace Key_Function;
		initKeyboardGraphics(0.0,0.0,0.5);
		struct init_key_HSV {
			enum Key_Function_Enum type;
			fp64 h; fp64 s; fp64 v;
		};
		init_key_HSV InitKeyHSV[] = {
			{NONE,0.0,0.0,0.5},
			{COORDINATES,0.0,0.7,1.0},{TRANSFORMATIONS,180.0,0.7,1.0},{JULIA,300.0,0.6,1.0},
			{PARAMETERS,59.9,1.0,1.0},{POLAR,240.0,0.7,0.7},{FORMULA,210.0,0.3,0.8},
			/* {SCREEN_SPLIT,30.0,0.3,1.0}, */ {FUNCTIONS,120.0,0.8,0.8},{RENDERING,180.0,0.4,0.6},
		};
		struct init_key_RGB {
			enum Key_Function_Enum type;
			uint8_t r; uint8_t g; uint8_t b;
		};
		init_key_RGB InitKeyRGB[ARRAY_LENGTH(InitKeyHSV)];
		for (size_t i = 0; i < ARRAY_LENGTH(InitKeyHSV); i++) {
			getRGBfromHSV(&(InitKeyRGB[i].r),&(InitKeyRGB[i].g),&(InitKeyRGB[i].b),InitKeyHSV[i].h,InitKeyHSV[i].s,InitKeyHSV[i].v);
		}
		// if (currentKBPreset->kList.size() <= 0) {
		// 	currentKBPreset->kList = defaultKeyBind;
		// }
		for (size_t s = SDL_SCANCODE_UNKNOWN + 1; s < SDL_NUM_SCANCODES; s++) {
			size_t keyColorSet = 0;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == (SDL_Scancode)s) {
					if (keyColorSet != 0) { // Detects if different function types are binded to a key
						setRGB_Scancode(0xFF,0xFF,0xFF,(SDL_Scancode)s);
						break; // Idempotent
					} 
					for (size_t r = Key_Function::NONE + 1; r < ARRAY_LENGTH(Key_Function_Map); r++) {
						if (bind.func <= Key_Function_Map[r] && bind.func != Key_Function::NONE) {
							setRGB_Scancode(InitKeyRGB[r].r,InitKeyRGB[r].g,InitKeyRGB[r].b,(SDL_Scancode)s);
							keyColorSet = r;
							break;
						}
					}
				}
			}
		}
	}
}

void initKeys() {
	recolorKeyboard();
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		Key_List[i].timePressed = getNanoTime();
		Key_List[i].key = (SDL_Scancode)i;
		Key_List[i].pressed = false;
		Key_List[i].timeReleased = getNanoTime();
	}
	updateKeys();
}

bool keyPressed(SDL_Scancode key) {
	if (key > SDL_NUM_SCANCODES) {
		return false;
	}
	if (Key_List[key].pressed == true) {
		return true;
	}
	return false;
}

int setup_fracExpKB(int argc, char* argv[]) {
	init_KeyBind_PresetList();
	size_t importedKeyBinds = 0;
	if (argc >= 2) {
		KeyBind_Preset* temp_KeyBind = currentKBPreset;
		for (int a = 1; a < argc; a++) {
			if (strstr(argv[a],".fracExpKB") != NULL) {
				printFlush("FracExp_KeyBind File: %s\n",argv[a]);
				if (import_KeyBindPresets(&KeyBind_PresetList,&temp_KeyBind,argv[a]) == 0) {
					importedKeyBinds++;
					if (importedKeyBinds == 1) {
						currentKBPreset = temp_KeyBind;
					}
				}
			}
		}
		printFlush("\n");
	}

	return 0;
}


// static const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

// #ifndef BUILD_RELEASE
// 	const char* buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
// #else
// 	const char* buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds"};
// #endif

void getRenderBufferBoxFromMaster(BufferBox& box) {
	Master.getBufferBox(&box);
	if (
		(validateBufferBox(&box, true) == false) ||
		(RESY_UI > Master.resY)
	) { return; }
	box.resY = Master.resY - RESY_UI;
	size_t pitch = getBufferBoxPitch(&box);
	box.vram = &box.vram[pitch * (size_t)RESY_UI];
}

void force_resizeWindow(dim32_t resX, dim32_t resY) {
		if (resX < RESX_Minimum) { resX = RESX_Minimum; }
		if (resY < RESY_Minimum) { resY = RESY_Minimum; }
		if (resX > RESX_Maximum) { resX = RESX_Maximum; }
		if (resY > RESY_Maximum) { resY = RESY_Maximum; }
		//if (x & 0x3) { x &= 0xFFFFFFFC; } // Sets resX to a multiple of 4 so I don't have to deal with padded and unpadded image buffers
		
		SDL_SetWindowSize(window, resX, resY);
		SDL_RenderSetLogicalSize(renderer, resX, resY);
		Master.resX = resX;
		Master.resY = resY;

		updateRenderData(&primaryRenderData);
		updateRenderData(&secondaryRenderData);
		write_Update_Level(Change_Level::Full_Reset);
		Master.resizeBuffer(resX, resY, IMAGE_BUFFER_CHANNELS);
		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, Master.resX, Master.resY);

		write_Buffer_Size({nullptr, Master.resX,Master.resY - RESY_UI, IMAGE_BUFFER_CHANNELS, 0});
}

bool windowResizingCode(dim32_t* resX = nullptr, dim32_t* resY = nullptr) {
	bool reVal = false;
	dim32_t x = 0, y = 0;
	static dim32_t rX = 0, rY = 0;
	SDL_GetWindowSize(window,&x,&y);
	if ((rX != x || rY != y) && (rX != 0 && rY != 0)) {
		force_resizeWindow(x, y);
		if (resX != nullptr) { *resX = x; }
		if (resY != nullptr) { *resY = y; }
		reVal = true;
	}
	rX = x;
	rY = y;
	return reVal;
}

void set_Window_Fullscreen_Mode(Display_Fullscreen::Display_Fullscreen_Enum fullscreen_mode) {
	int32_t dispResX, dispResY, initResX, initResY, initPosX, initPosY;
	const DisplayInfo* disp = getDisplayFromWindowPosition(window);
	if (disp == nullptr) {
		return;
	}
	disp->getResolution(dispResX, dispResY);
	disp->getResolution(initResX, initResY);
	disp->getPosition(initPosX,initPosY);
	switch (fullscreen_mode) {
		// case Display_Fullscreen::Fullscreen:
		// 	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		// 	break;
		case Display_Fullscreen::Windowed_Fullscreen:
			if (dispResX >= RESX_Minimum && dispResY >= RESY_Minimum) {
				force_resizeWindow(dispResX, dispResY);
				SDL_SetWindowPosition(window, initPosX, initPosY);
			}
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			windowResizingCode(nullptr, nullptr);
			break;
		// case Display_Fullscreen::Borderless_Fullscreen:
		// 	SDL_SetWindowFullscreen(window, SDL_WINDOW_BORDERLESS);
		// 	break;
		case Display_Fullscreen::Windowed:
		default:
			{
				SDL_SetWindowFullscreen(window, 0);
				calculate_init_window_size(dispResX, dispResY, initResX, initResY, initPosX, initPosY);
				force_resizeWindow(initResX, initResY);
				SDL_SetWindowPosition(window, initPosX, initPosY);
			}
			break;
	};
}

Display_Fullscreen::Display_Fullscreen_Enum get_Window_Fullscreen_Mode() {
	dim32_t resX, resY;
	const DisplayInfo* disp = getDisplayFromWindowPosition(window);
	if (disp == nullptr) {
		return Display_Fullscreen::Windowed;
	}
	disp->getResolution(resX, resY);
	if ((dim32_t)Master.resX == resX && (dim32_t)Master.resY == resY) {
		return Display_Fullscreen::Windowed_Fullscreen;
	}
	return Display_Fullscreen::Windowed;
}

void toggle_Window_Fullscreen_Mode() {
	dim32_t resX, resY;
	const DisplayInfo* disp = getDisplayFromWindowPosition(window);
	if (disp != nullptr) {
		disp->getResolution(resX, resY);
		if ((dim32_t)Master.resX == resX && (dim32_t)Master.resY == resY) {
			set_Window_Fullscreen_Mode(Display_Fullscreen::Windowed);
			return;
		}
	}
	set_Window_Fullscreen_Mode(Display_Fullscreen::Windowed_Fullscreen);
}

void correctTextFloat(char* buf, size_t len, uint8_t level) { /* Strips characters */
	size_t p = 0;
	for (size_t i = 0; i < strnlen(buf,len); i++) {
		if (!((buf[i] >= '0' && buf[i] <= '9') || (buf[i] == '-' || buf[i] == '.'))) {
			continue;
		} else if ((level >= 1) && !(buf[i] == '^' || buf[i] == 'e' || buf[i] == 'E')) {
			continue;
		}
		buf[p] = buf[i];
		p++;
	}
	for (;p < len; p++) {
		buf[p] = '\0';
	}
}

void correctFrameTime() {
	valueClamp(GUI_FrameTimeNano, FRAMETIME_MINIMUM, FRAMETIME_MAXIMUM);
	valueClamp(GUI_FrameTimeSeconds, NANO_TO_SECONDS(FRAMETIME_MINIMUM), NANO_TO_SECONDS(FRAMETIME_MAXIMUM));
	valueClamp(GUI_FrameRate, FRAMERATE_MINIMUM, FRAMERATE_MAXIMUM);
	GUI_FrameTimer.setFreq(GUI_FrameTimeNano);
	write_FrameTime(GUI_FrameTimeNano);
}

void updateFrameTimeNano(nano64_t frameTimeNano) {
	GUI_FrameTimeNano = frameTimeNano;
	GUI_FrameTimeSeconds = NANO_TO_SECONDS(frameTimeNano);
	GUI_FrameRate = NANO_TO_FRAMERATE(frameTimeNano);
	correctFrameTime();
}

void updateFrameTimeSeconds(fp64 frameTimeSeconds) {
	GUI_FrameTimeNano = SECONDS_TO_NANO(frameTimeSeconds);
	GUI_FrameTimeSeconds = frameTimeSeconds;
	GUI_FrameRate = SECONDS_TO_FRAMERATE(frameTimeSeconds);
	correctFrameTime();
}

void updateFrameRate(fp64 frameRate) {
	GUI_FrameTimeNano = FRAMERATE_TO_NANO(frameRate);
	GUI_FrameTimeSeconds = FRAMERATE_TO_SECONDS(frameRate);
	GUI_FrameRate = frameRate;
	correctFrameTime();
}


int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
	
	GUI_FrameTimer = TimerBox(GUI_FrameTimeNano);

	TimerBox maxFrameReset = TimerBox(1.0 / 5.0); /* Keeps track of longest frame times */
	write_Update_Level(Change_Level::Full_Reset);

	if (read_Engine_Render_Configuration(Render_Config) == false) {
		printError("pDat_Engine_Render_Configuration is not initialized");
	}

	while (QUIT_FLAG == false) {
		{ // Accesses ABORT_RENDERING only when Abort_Rendering_Flag changes to reduce unnecessary accesses
			static bool Abort_Rendering_Change = Abort_Rendering_Flag;
			if (Abort_Rendering_Flag != Abort_Rendering_Change) {
				ABORT_RENDERING = Abort_Rendering_Flag;
				Abort_Rendering_Change = Abort_Rendering_Flag;
				if (Abort_Rendering_Flag == false) {
					write_Update_Level(Change_Level::Full_Reset);
				}
			}
		}
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			process_SDL2_event(event);
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				ABORT_RENDERING = true;
				QUIT_FLAG = true;
			}
		}
		if (force_quit_flag == true) {
			QUIT_FLAG = true;
			ABORT_RENDERING = true;
		}
		updateKeys();
		if (GUI_FrameTimer.timerReset()) {
			nano64_t DeltaTimeNano = GUI_FrameTimer.getDeltaTimeNano();
			DeltaTime = NANO_TO_SECONDS(DeltaTimeNano);
			nano64_t RenderTime = getRenderDelta();
			{
				static nano64_t maxFrameTime = 0.0;
				static nano64_t maxRenderTime = 0.0;
				if (maxFrameReset.timerReset()) {
					Frame_Time_Display = maxFrameTime;
					Render_Time_Display = maxRenderTime;
					maxFrameTime = 0;
					maxRenderTime = 0;
				}
				if (DeltaTimeNano > maxFrameTime) {
					maxFrameTime = DeltaTimeNano;
				}
				if (RenderTime > maxRenderTime) {
					maxRenderTime = RenderTime;
				}
			}
			windowResizingCode();
			updateFractalParameters(current_Fractal, force_quit_flag);
			write_Parameters(&current_Fractal, &primaryRenderData, &secondaryRenderData);
			newFrame();
		} else {
			if (GUI_FrameTimer.timeToTimerReadyNano() > SECONDS_TO_NANO(1.0e-3)) {
				std::this_thread::yield();
			}
		}
	}
	return 0;
}

// int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
// 	uint64_t yeildTimeNano = 80000; /* 80 micro seconds */
// 	uint64_t FRAME_RATE_NANO = SECONDS_TO_NANO(1.0 / FRAME_RATE);
// 	//printFlush("yeildTimeNano: %llu | %lf\n",yeildTimeNano,NANO_TO_SECONDS(yeildTimeNano));
// 	TimerBox frameTimer = TimerBox(1.0/FRAME_RATE);
// 	TimerBox maxFrameReset = TimerBox(1.0/5.0); /* Keeps track of longest frame times */
// 	write_Update_Level(Change_Level::Full_Reset);
// 	while (QUIT_FLAG == false) {
// 		{ // Accesses ABORT_RENDERING only when Abort_Rendering_Flag changes to reduce unnecessary accesses
// 			static bool Abort_Rendering_Change = Abort_Rendering_Flag;
// 			if (Abort_Rendering_Flag != Abort_Rendering_Change) {
// 				ABORT_RENDERING = Abort_Rendering_Flag;
// 				Abort_Rendering_Change = Abort_Rendering_Flag;
// 				if (Abort_Rendering_Flag == false) {
// 					write_Update_Level(Change_Level::Full_Reset);
// 				}
// 			}
// 		}
// 		SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             ImGui_ImplSDL2_ProcessEvent(&event);
//             if (event.type == SDL_QUIT) {
// 				ABORT_RENDERING = true;
//                 QUIT_FLAG = true;
//             }
//         }
// 		updateKeys();
// 		/* Poorly implemented, please correct */
// 		if (frameTimer.timerReady() == false) {
// 			uint64_t yeildLimit = frameTimer.timeElapsedNano();
// 			if (yeildLimit < yeildTimeNano) {
// 				//uint64_t yeildStart = getNanoTime();
// 				std::this_thread::yield();
// 				uint64_t yeildEnd = getNanoTime() - yeildStart;
// 				yeildCount++;
// 				if (frameTimer.timerReady() == true) {
// 					yeildError++; yeildPrint = true;
// 					printFlush("%.3lfus\n",(frameTimer.timeElapsed() - (1.0/FRAME_RATE)) * 1.0e6);
// 				} else {
// 					yeildSave += yeildEnd;
// 				}
// 				*/
// 			}
// 		}
		
// 		/*
// 		#define TIME_SCALE 1000
// 		if (frameTimer.timerReady() == false) {
// 			uint64_t timeElapsed = frameTimer.timeElapsedNano();
// 			uint64_t timeLeft = frameTimer.timeToTimerReadyNano();
// 			printFlush("\nElapsed: %6" PRIu64 "us TimeLeft:  %6" PRIu64 "us\n",timeElapsed/TIME_SCALE, timeLeft/TIME_SCALE);
// 			if (timeLeft > END_SLEEP_HEADROOM) {
// 				uint64_t sleepTime = timeLeft - END_SLEEP_HEADROOM;
// 				std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
// 				uint64_t remainingTime = frameTimer.timeToTimerReadyNano();
// 				printFlush("Sleep:   %6" PRIu64 "us Remaining: %6" PRIu64 "us\n",sleepTime/TIME_SCALE,remainingTime/TIME_SCALE);
// 			}
// 		}
// 		*/
// 		if (frameTimer.timerReset()) {
// 			nano64_t DeltaTimeNano = GUI_FrameTimer.getDeltaTimeNano();
// 			DeltaTime = NANO_TO_SECONDS(DeltaTimeNano);
// 			nano64_t RenderTime = getRenderDelta();
// 			{
// 				static nano64_t maxFrameTime = 0;
// 				static nano64_t maxRenderTime = 0;
// 				if (maxFrameReset.timerReset()) {
// 					Frame_Time_Display = maxFrameTime;
// 					Render_Time_Display = maxRenderTime;
// 					maxFrameTime = 0;
// 					maxRenderTime = 0;
// 					/*
// 					if (yeildPrint == true || yeildSwitch == true) {
// 						yeildSwitch = false;
// 						yeildPrint = false;
// 						printFlush("\nYeild Count: " PRIu64 " Errors: %" PRIu64 "\n",yeildCount, yeildError);
// 						printFlush("Time Saved: %.3lfms\n",NANO_TO_SECONDS(yeildSave) * 1.0e3);
// 						yeildSum += yeildCount;
// 						printFlush("Yeild Error: %.3lfms per error\n\n",((fp64)(getNanoTime() - yeildTimer) / (fp64)yeildError) / 1.0e6);
// 						yeildCount = 0;
// 					}
// 					*/				
// 				}
// 				if (DeltaTimeNano > maxFrameTime) {
// 					maxFrameTime = DeltaTimeNano;
// 				}
// 				if (RenderTime > maxRenderTime) {
// 					maxRenderTime = RenderTime;
// 				}
// 			}
// 			if (FRAME_RATE_NANO < 2 * yeildTimeNano) {
// 				yeildTimeNano = 0;
// 			} else {
// 				yeildTimeNano = FRAME_RATE_NANO - yeildTimeNano;
// 			}
// 			windowResizingCode();
// 			updateFractalParameters();
// 			write_Parameters(&frac,&primaryRenderData,&secondaryRenderData);
// 			newFrame();
// 		}
// 	}
// 	return 0;
// }

uint64_t get_Hardware_Hash() {
	FNV1A_Hash hardwareHash;
	uint8_t value8 = 0x0; uint16_t value16 = 0x0; uint32_t value32 = 0x0;
	value8 = PROGRAM_V_MAJOR;
	hardwareHash += value8;
	value32 = (uint32_t)std::thread::hardware_concurrency();
	hardwareHash += value32;
	value32 = (uint32_t)SDL_GetCPUCacheLineSize();
	hardwareHash += value32;
	value32 = (uint32_t)SDL_GetSystemRAM();
	hardwareHash += value32;
	value16 = (uint16_t)count_Supported_CPU_Instruction(get_Available_CPU_Instruction());
	hardwareHash += value16;
	// #ifdef Enable_OpenCL
	// 	get_GPU_Hardware_Hash(hardwareHash);
	// #endif
	return hardwareHash.get_hash();
}

void init_config_data() {
	std::string import_path = get_RelativeFilePath();
	import_path += "config.fracExpConfig";
	if (import_config_data(config_data, import_path.c_str()) == 0) {
		if (config_data.Automatic_Behaviour.AutoLoad_Config_File == false) {
			default_User_Configuration_Data(config_data, true);
			return;
		}
		clean_User_Configuration_Data(config_data);
		if (config_data.Rendering_Settings.Hardware_Hash != get_Hardware_Hash()) {
			config_data.File_Paths.Path_Screenshot = get_RelativeFilePath();
			config_data.File_Paths.Path_FracExpKeybind = get_RelativeFilePath();
		}
		write_Screenshot_Path(config_data.File_Paths.Path_Screenshot.c_str());
	} else {
		default_User_Configuration_Data(config_data, true);
	}
}

void terminate_config_data() {
	if (config_data.Automatic_Behaviour.AutoSave_Config_File == false) {
		return;
	}
	config_data.Display_Preferences.Display_Config_Hash = getDisplayConfigHash();
	config_data.Rendering_Settings.Hardware_Hash = get_Hardware_Hash();

	int32_t windowPosX, windowPosY; SDL_GetWindowPosition(window, &windowPosX, &windowPosY);
	dim32_t windowResX, windowResY; SDL_GetWindowSize(window, &windowResX, &windowResY);
	const DisplayInfo* currentDisplay = getDisplayFromPosition(
		windowPosX + (windowResX / 2), windowPosY + (windowResY / 2)
	);
	config_data.Display_Preferences.Previous_Display_Used =
		(currentDisplay != nullptr) ? currentDisplay->getIndex() : 0;
	
	std::string export_path = get_RelativeFilePath();
	export_path += "config.fracExpConfig";
	export_config_data(config_data, export_path.c_str());
	// char filePath[324]; memset(filePath,'\0',sizeof(filePath));
	// saveFileInterface(filePath,ARRAY_LENGTH(filePath));
	// export_config_data(config_data,"./config.fracExpConfig");
}

// Returns the index of the display to be used. Returns 0 on failure
int32_t loadDisplayInformation(
	const User_Display_Preferences& display_config,
	dim32_t& initResX, dim32_t& initResY,
	dim32_t& initPosX, dim32_t& initPosY
) {
	int32_t displayCount = reloadDisplays();
	if (displayCount <= 0) {
		printError("Failed to detect displays");
		return 0;
	}
	#ifndef BUILD_RELEASE
		printf("\tDisplay Count: %" PRId32 "\n",displayCount);
	#endif
	int32_t cursorPosX, cursorPosY;
	SDL_GetGlobalMouseState(&cursorPosX, &cursorPosY);
	const DisplayInfo* disp = getBootupDisplay(
		display_config,
		RESX_Minimum, RESY_Minimum,
		cursorPosX, cursorPosY
	);
	if (disp == nullptr) {
		printError("unable to getBootupDisplay");
		return 0;
	}
	disp->getResolution(initResX,initResY);
	disp->getCornerNW(initPosX,initPosY);
	return disp->getIndex();
}

void calculate_init_window_size(
	const dim32_t& dispResX, const dim32_t& dispResY,
	dim32_t& initResX, dim32_t& initResY,
	dim32_t& initPosX, dim32_t& initPosY
) {
	// initPosX = 0;
	// initPosY = 0;
	initResX -= RESX_Margin;
	initResY -= RESY_Margin;
	if (config_data.Display_Preferences.ScaleWindowToScreenSize == false) {
		if (initResX > RESX_Default) {
			initResX = RESX_Default;
		} else if (initResX < RESX_Minimum) {
			initResX = RESX_Minimum;
		}
		if (initResY > RESY_Default) {
			initResY = RESY_Default;
		} else if (initResY < RESY_Minimum) {
			initResY = RESY_Minimum;
		}
	} else {
		initResX = calcMinMaxRatio(initResX, RESX_Minimum, RESX_Default, config_data.Display_Preferences.Bootup_Window_Scale);
		if (initResX > RESX_Maximum) {
			initResX = RESX_Maximum;
		}
		initResY = calcMinMaxRatio(initResY, RESY_Minimum, RESY_Default, config_data.Display_Preferences.Bootup_Window_Scale);
		if (initResY > RESX_Maximum) {
			initResY = RESX_Maximum;
		}
	}
	initPosX += (dispResX - initResX) / 2;
	initPosY += (dispResY - initResY) / 2;
	// Rounds the initial screen resolution for debug purposes
	#ifdef ROUND_INIT_WINDOW_RESX
		initResX = (initResX / 16) * 16;
	#endif
}

int init_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
	init_config_data();
	const User_Display_Preferences& Display_Preferences = config_data.Display_Preferences;

	//SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printFatalError("SDL_Init(SDL_INIT_EVERYTHING) failed to initialize");
		return -1;
	}
	#ifndef BUILD_RELEASE
		printf("System Information:\n");
	#endif
	dim32_t dispResX, dispResY;
	dim32_t initResX, initResY, initPosX, initPosY;
	int32_t initDisplayIndex = loadDisplayInformation(Display_Preferences, initResX, initResY, initPosX, initPosY);
	if (initDisplayIndex == 0) {
		printCriticalError("init_Render failed to loadDisplayInformation");
	}
	dispResX = initResX;
	dispResY = initResY;
	//printFlush("Old: %" PRId32 "x%" PRId32 " %" PRId32 ",%" PRId32 "\n",initResX,initResY,initPosX,initPosY);
	calculate_init_window_size(
		dispResX, dispResY,
		initResX, initResY,
		initPosX, initPosY
	);
	switch (Display_Preferences.Bootup_Fullscreen) {
		case Display_Fullscreen::Windowed_Fullscreen:
			break;
		case Display_Fullscreen::Windowed:
		default:
			calculate_init_window_size(
				dispResX, dispResY,
				initResX, initResY,
				initPosX, initPosY
			);
	}
	//printFlush("New: %" PRId32 "x%" PRId32 " %" PRId32 ",%" PRId32 "\n",initResX,initResY,initPosX,initPosY);
	#ifndef BUILD_RELEASE
		printf("\tOperating System: %s\n", SDL_GetPlatform());
		printf("\tSystem RAM: %" PRId32 "MB\n", SDL_GetSystemRAM());
	#endif
	// Allocate Buffers
	//initBufferBox(&Master,NULL,initResX,initResY,IMAGE_BUFFER_CHANNELS);
	Master = ImageBuffer(initResX,initResY,IMAGE_BUFFER_CHANNELS);
	
	window = SDL_CreateWindow(
		PROGRAM_NAME " v" PROGRAM_VERSION " " PROGRAM_DATE,
		initPosX, initPosY,
		Master.resX, Master.resY,
		SDL_WINDOW_RESIZABLE
	);
	SDL_SetWindowMinimumSize(window, RESX_Minimum, RESY_Minimum);
	SDL_SetWindowMaximumSize(window, RESX_Maximum, RESY_Maximum);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, Master.resX, Master.resY);
	set_Window_Fullscreen_Mode(Display_Fullscreen::Windowed);
	if ((Display_Fullscreen::Display_Fullscreen_Enum)Display_Preferences.Bootup_Fullscreen == Display_Fullscreen::Windowed_Fullscreen) {
		set_Window_Fullscreen_Mode(Display_Fullscreen::Windowed_Fullscreen);
	}
	
	write_Buffer_Size({nullptr, Master.resX, Master.resY - RESY_UI, IMAGE_BUFFER_CHANNELS, 0});

	super_screenshot_maxThreads = (int32_t)std::thread::hardware_concurrency();
	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io_IMGUI = &ImGui::GetIO();
	io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io_IMGUI->IniFilename = nullptr;
	set_IMGUI_Theme((Display_GUI::IMGUI_Theme)config_data.GUI_Settings.GUI_Theme);
	ImGui_ImplSDLRenderer2_Init(renderer);
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);

	{ /* Init FPS */
		fp64 initFPS = 60.0; // Default fallback value
		const DisplayInfo* dispFPS = getInitDisplayRefreshRate(
			Display_Preferences, window,
			RESX_Minimum, RESY_Minimum
		);
		if (dispFPS == nullptr) {
			initFPS = Display_Preferences.Constant_RefreshRate_Value;
		} else {
			int32_t mult_Value = Display_Preferences.Maximum_FPS_Multiplier;
			fp64 mult_FPS = 1.0;
			if (mult_Value >= 1) {
				mult_FPS = (fp64)(mult_Value + 1);
			} else if (mult_Value <= -1) {
				mult_FPS = 1.0 / (fp64)(-mult_Value + 1);
			}
			initFPS = dispFPS->getRefreshRate() * mult_FPS;
		}
		updateFrameRate(CALC_FRAMERATE_OFFSET(initFPS));
	}

	// { // Doesn't work
	// 	int32_t resX, resY, dimX, dimY;
	// 	SDL_GetWindowSize(window,&resX,&resY);
	// 	SDL_GetWindowSizeInPixels(window,&dimX,&dimY);
	// 	if (resX != dimX || resY != dimY) {
	// 		printWarning(
	// 			"Window is rendering in high dpi mode, and may have scaling artifacts %" PRId32 "x%" PRId32 " != %" PRId32 "x%" PRId32,
	// 			resX, resY, dimX, dimY
	// 		);
	// 	}
	// }

	setDefaultParameters(&current_Fractal, Fractal_ABS_Mandelbrot);
	Bootup_initRenderData();
	if (texture != nullptr) {
		SDL_DestroyTexture(texture);
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
	// printf("Init_Render: %s\n", ((QUIT_FLAG == true) ? "True" : "False"));
	init_KeyBind_PresetList();
	initKeys();
	//cleanKeyBind(&currentKeyBind);

	config_data.Display_Preferences.Display_Config_Hash = getDisplayConfigHash();
	config_data.Rendering_Settings.Hardware_Hash = get_Hardware_Hash();

	bootup_Fractal_Frame_Rendered = false;
	
	write_Render_Ready(true);
	write_Parameters(&current_Fractal, &primaryRenderData, &secondaryRenderData);
	#ifndef BUILD_RELEASE
		printFlush("\n");
	#endif

	while (read_Engine_Ready() == false) {
		if (QUIT_FLAG == true) {
			printWarning("Render thread exiting initialization: QUIT_FLAG == true");
			return -1;
		}
		std::this_thread::yield();
	}
	start_Render(QUIT_FLAG, ABORT_RENDERING);
	return 0;
}

int terminate_Render() {
	terminate_config_data();
	terminateKeyboardGraphics();
	clear_KeyBind_PresetList();
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyTexture(kTexture);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}



void setRenderedBufferBox(BufferBox* box) {
	rendered_buf = box;
}



void renderTestGraphic(BufferBox& buf, fp64 cycleSpeed, fp64 minSpeed, fp64 maxSpeed) {
	if (printValidateBufferBox(&buf) == false) {
		printError("renderTestGraphic() failed");
		return;
	}
	// nano64_t startTimer = getNanoTime();
	static fp64 f = 0.0;
	fp64 halfDiff = (maxSpeed - minSpeed) / 2.0;
	fp64 speed = halfDiff * sin(cycleSpeed * TAU * getDecimalTime()) + minSpeed + halfDiff;
	f += DeltaTime * speed;
	size_t w = (size_t)(f * (256.0));
	size_t z = 0;

	static constexpr size_t patternLength = 256;
	__attribute__((unused)) static constexpr size_t patternSize = patternLength * IMAGE_BUFFER_CHANNELS;

	size_t dimX = ((size_t)buf.resX > patternLength) ? patternLength : (size_t)buf.resX;
	size_t dimY = ((size_t)buf.resY > patternLength) ? patternLength : (size_t)buf.resY;
	size_t pitch = ((size_t)buf.resX * IMAGE_BUFFER_CHANNELS);
	size_t offset = 0;
	for (size_t y = 0; y < dimY; y++) {
		z = offset;
		for (size_t x = 0; x < dimX; x++) {
			#ifdef fullColorTestGraphic
				buf.vram[z] = (uint8_t)((x - w) % 256); buf.vram[z] /= color_square_divider; z++;
				buf.vram[z] = (uint8_t)((w - y) % 256); buf.vram[z] /= color_square_divider; z++;
			#else
				buf.vram[z] = 0; z++;
				buf.vram[z] = 0; z++;
			#endif
			buf.vram[z] = (uint8_t)((w + x + y) % 256); buf.vram[z] /= color_square_divider; z++;
			buf.vram[z] = 0xFF; z++;
		}
		inPlacePatternMemcpy(&buf.vram[offset], pitch, (dimX * IMAGE_BUFFER_CHANNELS));
		offset += pitch;
	}
	if ((size_t)buf.resY > patternLength) {
		inPlacePatternMemcpy(
			buf.vram,
			(size_t)buf.resX * (size_t)buf.resY * buf.channels,
			patternLength * (size_t)buf.resX * (size_t)buf.channels
		);
	}
	// nano64_t finishTimer = getNanoTime();
	// printfInterval(0.2,"\nGRAPHIC: %.3lf",(fp64)(finishTimer - startTimer) / 1.0e6);
}

namespace Status_Graphic {
	enum Status_Graphic_Enum {Graphic_Abort, Graphic_Pause, Graphic_Loading, Graphic_Count};
}

void renderStatusGraphic(BufferBox& buf, Status_Graphic::Status_Graphic_Enum status_graphic, fp64 speed) {
	if (printValidateBufferBox(&buf) == false) {
		printError("renderStatusGraphic() failed");
		return;
	}
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));

	static constexpr size_t patternLength = 256;
	static constexpr size_t patternSize = patternLength * IMAGE_BUFFER_CHANNELS;
	static uint8_t pattern[patternSize];
	size_t z = 0;
	switch (status_graphic) {
		case Status_Graphic::Graphic_Abort:
			for (size_t p = 0; p < patternLength; p++) {
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				pattern[z] = (uint8_t)((w + p) % 256) / 4; pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				if constexpr (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
			}
			break;
		case Status_Graphic::Graphic_Pause:
			for (size_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				if constexpr (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
			}
			break;
		case Status_Graphic::Graphic_Loading:
		default:
			for (size_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = 0; z++;
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				if constexpr (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
			}
			break;
	};
	size_t offset = 0;
	size_t shift = 0;
	size_t pitch = ((size_t)buf.resX * (size_t)buf.channels);
	size_t dimY = ((size_t)buf.resY > patternLength) ? patternLength : (size_t)buf.resY;
	for (size_t y = 0; y < dimY; y++) {
		size_t dimX = (pitch > shift) ? shift : pitch;
		memcpy(&buf.vram[offset], &pattern[patternSize - shift], dimX);
		if (shift <= pitch) {
			patternMemcpy(&buf.vram[offset + shift], pitch - shift, pattern, patternSize);
		}
		offset += pitch;
		shift += IMAGE_BUFFER_CHANNELS;
		shift %= patternSize;
	}
	if ((size_t)buf.resY > patternLength) {
		inPlacePatternMemcpy(
			buf.vram,
			(size_t)buf.resX * (size_t)buf.resY * buf.channels,
			patternLength * (size_t)buf.resX * buf.channels
		);
	}
}

int exportScreenshot() {
	static nano64_t resetTime = 0;
	if (getNanoTime() - resetTime > SECONDS_TO_NANO(0.5) && exportFractalBuffer == false) {
		resetTime = getNanoTime();
		exportFractalBuffer = true;
		printFlush("Taking Screenshot\n");
	}
	return 0;
}

void set_Super_Screenshot_Bounding_Box(
	ABS_Mandelbrot& frac,
	Render_Data& ren,
	dim32_t resX, dim32_t resY,
	Namespace_Image_Render_Bounding_Box::Enum_Image_Render_Bounding_Box bound_type
) {
	// fp64 ORIGINAL_ZOOM = frac.zoom;
	fp64 ratio_Render = (fp64)ren.resX / (fp64)ren.resY;
	fp64 ratio_Window = (fp64)resX / (fp64)resY;
	if (ratio_Render == ratio_Window) {
		return; // Nothing to be done
	}
	using namespace Namespace_Image_Render_Bounding_Box;
	if (bound_type == Expand_from_Center) {
		return; // Nothing to be done
	}
	
	if (ratio_Window >= 1.0) {
		if (ratio_Render >= 1.0) {
			/* Both Wide Ratios */
			switch(bound_type) {
				default:
					break; // Nothing to be done
				case Fill_Area: {
					// Wider than window
					if (ratio_Render >= ratio_Window) {
						
					} else {
						frac.zoom -= log10(ratio_Window / ratio_Render);
					}
				} break;
				case Fit_Area: {
					if (ratio_Render >= ratio_Window) {
						frac.zoom += log10(ratio_Render / ratio_Window);
					} else {
						
					}
				} break;
				case Fit_Width: {
					if (ratio_Render >= ratio_Window) {
						frac.zoom += log10(ratio_Render / ratio_Window);
					} else {
						frac.zoom -= log10(ratio_Window / ratio_Render);
					}
				} break;
				case Fit_Height: {
					if (ratio_Render >= ratio_Window) {
						
					} else {
						
					}
				} break;
			}
		} else {
			/* Window is Wide, and Render is Tall */
			switch(bound_type) {
				default:
					break; // Nothing to be done
				case Fill_Area: {
					frac.zoom -= log10(ratio_Window);
				} break;
				case Fit_Area: {
					frac.zoom += log10(1.0 / ratio_Render);
				} break;
				case Fit_Width: {
					frac.zoom -= log10(ratio_Window);
				} break;
				case Fit_Height: {
					frac.zoom += log10(1.0 / ratio_Render);
				} break;
			}
		}
	} else {
		if (ratio_Render >= 1.0) {
			/* Window is Tall, and Render is Wide*/
			switch(bound_type) {
				default:
					break; // Nothing to be done
				case Fill_Area: {
					frac.zoom -= log10(1.0 / ratio_Window);
				} break;
				case Fit_Area: {
					frac.zoom += log10(ratio_Render);
				} break;
				case Fit_Width: {
					frac.zoom += log10(ratio_Render);
				} break;
				case Fit_Height: {
					frac.zoom -= log10(1.0 / ratio_Window);
				} break;
			}
		} else {
			/* Both Tall Ratios*/
			switch(bound_type) {
				default:
					break; // Nothing to be done
				case Fill_Area: {
					// Wider than window
					if (ratio_Render >= ratio_Window) {
						frac.zoom -= log10(ratio_Window / ratio_Render);
					} else {
						
					}
				} break;
				case Fit_Area: {
					if (ratio_Render >= ratio_Window) {
								
					} else {
						frac.zoom += log10(ratio_Window / ratio_Render);	
					}
				} break;
				case Fit_Width: {
					if (ratio_Render >= ratio_Window) {
										
					} else {
						
					}
				} break;
				case Fit_Height: {
					if (ratio_Render >= ratio_Window) {
						frac.zoom -= log10(ratio_Render / ratio_Window);
					} else {
						frac.zoom += log10(ratio_Window / ratio_Render);
					}
				} break;
			}
		}
	}
	// printFlush("Zoom Change: %.3lf --> %.3lf\n", ORIGINAL_ZOOM, frac.zoom);
}

int exportSuperScreenshot() {
	static nano64_t resetTime = 0;
	if (getNanoTime() - resetTime > SECONDS_TO_NANO(0.5) && exportSuperFractalBuffer == false) {
		resetTime = getNanoTime();
		exportSuperFractalBuffer = true;
		ABS_Mandelbrot superFrac = current_Fractal;
		Render_Data superRenderData = primaryRenderData;
		superFrac.maxItr = super_screenshot_maxItr;
		superRenderData.resX = super_screenshot_resX;
		superRenderData.resY = super_screenshot_resY;
		superRenderData.sample = super_screenshot_super_sample;
		superRenderData.subSample = 1;
		superRenderData.CPU_Threads = (uint32_t)super_screenshot_maxThreads * (uint32_t)super_screenshot_threadMultiplier;
		const User_Screenshot_Settings& screenshot_settings = config_data.Screenshot_Settings;
		set_Super_Screenshot_Bounding_Box(
			superFrac,
			superRenderData,
			primaryRenderData.resX, primaryRenderData.resY,
			(Namespace_Image_Render_Bounding_Box::Enum_Image_Render_Bounding_Box)config_data.Rendering_Settings.Image_Render_Bounding_Box
		);
		switch(screenshot_settings.screenshotFileType) {
			case Image_File_Format::PNG:
			default:
				send_Image_Render(&superFrac, &superRenderData, Image_File_Format::PNG, (uint8_t)screenshot_settings.PNG_Compression_Level);
			break;
			case Image_File_Format::JPG:
				send_Image_Render(&superFrac, &superRenderData, Image_File_Format::JPG, (uint8_t)screenshot_settings.JPG_Quality_Level);
			break;
		}
	}
	return 0;
}

void renderJuliaCordinatePoint(const BufferBox& box) {
	const User_Rendering_Settings& Rendering_Settings = config_data.Rendering_Settings;
	if (Rendering_Settings.JuliaPoint_Enabled == false) { return; }
	if (current_Fractal.zr == 0.0 && current_Fractal.zi == 0.0) { return; }

	if (validateBufferBox(&box, true) == false) {
		printError("Unable to renderJuliaCordinatePoint(), to invalid BufferBox");
		return;
	}
	const int32_t outsideRadiusSquared =
	(int32_t)(Rendering_Settings.JuliaPoint_OuterRadius * Rendering_Settings.JuliaPoint_OuterRadius);
	const int32_t insideRadiusSquared =
	(int32_t)(Rendering_Settings.JuliaPoint_InnerRadius * Rendering_Settings.JuliaPoint_InnerRadius);
	int32_t pointRadius = (int32_t)ceil(Rendering_Settings.JuliaPoint_OuterRadius);
	int32_t posX, posY;
	coordinate_to_pixel(
		current_Fractal.zr, current_Fractal.zi,
		posX, posY, current_Fractal, box.resX, box.resY
	);
	for (int32_t y = posY - pointRadius; y <= posY + pointRadius; y++) {
		if (y < 0 || y >= box.resY) { continue; }
		for (int32_t x = posX - pointRadius; x <= posX + pointRadius; x++) {
			if (x < 0 || x >= box.resX) { continue; }
			int32_t squaredDistance = (posX - x) * (posX - x) + (posY - y) * (posY - y);
			if (
				(squaredDistance >= outsideRadiusSquared) ||
				(squaredDistance < insideRadiusSquared)
			) { continue; }
			size_t offset = ((size_t)x * box.channels) + (getBufferBoxPitch(&box) * (size_t)y);
			for (size_t i = 0; i < box.channels; i++) {
				box.vram[offset + i] = (uint8_t)(((uint32_t)box.vram[offset + i] + (uint32_t)0xFF) / 2);
			}
		}
	}
}

// TODO: Add Minimum_Image_Resolution once this function is mostly complete
/* (Unfinished) Revisied SDL2 Frame Transformation */
// int Transform_Frame_Affine(const ImageBuffer& image, const Render_Data& ren) {
// 	if (image.vram == nullptr) { printError("const ImageBuffer& image.vram is nullptr"); return -1; }
// 	if (image.allocated() == false) { printError("const ImageBuffer& image is not allocated"); return -1; }
// 	const ABS_Mandelbrot& FRAC = current_Fractal;

// 	int32_t fx0 = 0; int32_t fy0 = 0;
// 	int32_t fx1 = 0; int32_t fy1 = 0;
// 	coordinate_to_pixel(image.x00, image.y00, fx0, fy0, FRAC, ren.resX, ren.resY);
// 	coordinate_to_pixel(image.x11, image.y11, fx1, fy1, FRAC, ren.resX, ren.resY);
// 	/* Swap */
// 		if (fx0 > fx1) { int32_t temp = fx0; fx0 = fx1; fx1 = temp; }
// 		if (fy0 > fy1) { int32_t temp = fy0; fy0 = fy1; fy1 = temp; }
// 		fx1 -= fx0;
// 		fy1 -= fy0;

// 	fp64 rotation_Angle = (FRAC.rot - image.rot) * (360.0 / TAU);
		
// 	SDL_Surface* surface_Rot = SDL_CreateRGBSurfaceWithFormatFrom(
// 		image.vram,
// 		image.resX, image.resY,
// 		(int32_t)(image.channels * 8),
// 		(int32_t)(image.channels * (size_t)image.resX),
// 		SDL_PIXELFORMAT_ABGR8888
// 	);
// 	SDL_Rect srcRect_Rot = {0, 0, image.resX, image.resY};
// 	SDL_Rect dstRect_Rot = {0, RESY_UI, ren.resX, ren.resY};
// 	SDL_Texture* texture_Rot = SDL_CreateTextureFromSurface(renderer, surface_Rot);
// 	SDL_Texture* texture_Scale = SDL_CreateTexture(
// 		renderer, SDL_PIXELFORMAT_ABGR8888,
// 		SDL_TEXTUREACCESS_TARGET, ren.resX, ren.resY
// 	);
// 	SDL_SetRenderTarget(renderer, texture_Scale);
// 	if (SDL_RenderCopyEx(renderer, texture_Rot, &srcRect_Rot, &dstRect_Rot, rotation_Angle, nullptr, SDL_FLIP_NONE)) {
// 		printf("renderCopyEx: %s\n", SDL_GetError()); fflush(stdout);
// 	}
// 	SDL_SetRenderTarget(renderer, nullptr);
// 	SDL_Rect srcRect_Scale = {0, 0, ren.resX, ren.resY};
// 	SDL_Rect dstRect_Scale = {fx0, fy0, fx1, fy1};
// 	if (SDL_RenderCopy(renderer, texture_Scale, &srcRect_Scale, &dstRect_Scale)) {
// 		printf("renderCopy: %s\n", SDL_GetError()); fflush(stdout);
// 	}
// 	SDL_DestroyTexture(texture_Scale);
// 	SDL_DestroyTexture(texture_Rot);
// 	SDL_FreeSurface(surface_Rot);
// 	return 0;
// }

static int Transform_Frame_Rotate(
	const ImageBuffer& image,
	const Render_Data& ren,
	const ABS_Mandelbrot& FRAC
) {
	int32_t fx0 = 0; int32_t fy0 = 0;
	int32_t fx1 = 0; int32_t fy1 = 0;
	coordinate_to_pixel(image.x00, image.y00, fx0, fy0, FRAC, ren.resX, ren.resY);
	coordinate_to_pixel(image.x11, image.y11, fx1, fy1, FRAC, ren.resX, ren.resY);
	/* Swap */
		if (fx0 > fx1) { int32_t temp = fx0; fx0 = fx1; fx1 = temp; }
		if (fy0 > fy1) { int32_t temp = fy0; fy0 = fy1; fy1 = temp; }
		fx1 -= fx0;
		fy1 -= fy0;

	fp64 rotation_Angle = (FRAC.rot - image.rot) * (360.0 / TAU);
		
	scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(
		image.vram,
		image.resX, image.resY,
		(int32_t)(image.channels * 8),
		(int32_t)(image.channels * (size_t)image.resX),
		SDL_PIXELFORMAT_ABGR8888
	);
	SDL_Rect srcRect = {0, 0, image.resX, image.resY};
	SDL_Rect dstRect = {0, RESY_UI, ren.resX, ren.resY};
	scale_tex = SDL_CreateTextureFromSurface(renderer, scale_surface);
	if (SDL_RenderCopyEx(renderer, scale_tex, &srcRect, &dstRect, rotation_Angle, nullptr, SDL_FLIP_NONE)) {
		printf("renderCopyEx: %s\n", SDL_GetError()); fflush(stdout);
	}
	SDL_DestroyTexture(scale_tex);
	SDL_FreeSurface(scale_surface);
	return 0;
}

static int Transform_Frame_Scale_Translate(
	const ImageBuffer& image,
	const Render_Data& ren,
	const ABS_Mandelbrot& FRAC
) {

	int32_t fx0 = 0; int32_t fy0 = 0;
	int32_t fx1 = 0; int32_t fy1 = 0;
	coordinate_to_pixel(image.x00, image.y00, fx0, fy0, FRAC, ren.resX, ren.resY);
	coordinate_to_pixel(image.x11, image.y11, fx1, fy1, FRAC, ren.resX, ren.resY);
	/* Swap */
		if (fx0 > fx1) { int32_t temp = fx0; fx0 = fx1; fx1 = temp; }
		if (fy0 > fy1) { int32_t temp = fy0; fy0 = fy1; fy1 = temp; }
		fx1 -= fx0;
		fy1 -= fy0;
	// int32_t fxCenter = (fx0 + fx1) / 2;
	// int32_t fyCenter = (fy0 + fy1) / 2;
	if ((fx1 < 1 || fy1 < 1)) { // Nothing to Render
		// return 1;
		return 0;
	}
	//if ((image.rot != FRAC.rot) || ((fx0 < Master.resX) && (fy0 < (Master.resY - RESY_UI)))) {
		scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(
			image.vram,
			image.resX, image.resY,
			(int32_t)(image.channels * 8),
			(int32_t)(image.channels * (size_t)image.resX),
			SDL_PIXELFORMAT_ABGR8888
		);
		SDL_Rect srcRect = {0, 0, image.resX, image.resY};
		SDL_Rect dstRect = {fx0, fy0 + RESY_UI, fx1, fy1};
		scale_tex = SDL_CreateTextureFromSurface(renderer, scale_surface);
		if (SDL_RenderCopy(renderer, scale_tex, &srcRect, &dstRect)) {
			printf("renderCopy: %s\n", SDL_GetError()); fflush(stdout);
		}
		SDL_DestroyTexture(scale_tex);
		SDL_FreeSurface(scale_surface);
	//}
	return 0;
}

static int Transform_Frame_None(
	const ImageBuffer& image,
	const Render_Data& ren
) {
	scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(
		image.vram,
		image.resX, image.resY,
		(int32_t)(image.channels * 8),
		(int32_t)(image.channels * (size_t)image.resX),
		SDL_PIXELFORMAT_ABGR8888
	);
	SDL_Rect srcRect = {0, 0, image.resX, image.resY};
	SDL_Rect dstRect = {0, RESY_UI, ren.resX, ren.resY};
	scale_tex = SDL_CreateTextureFromSurface(renderer, scale_surface);
	if (SDL_RenderCopy(renderer, scale_tex, &srcRect, &dstRect)) {
		printf("renderCopy: %s\n", SDL_GetError()); fflush(stdout);
	}
	SDL_DestroyTexture(scale_tex);
	SDL_FreeSurface(scale_surface);
	return 0;
}

// Returns what transformations need to be applied to the image buffer
static inline void calculate_Tranformation_Change(
	const ImageBuffer& image, const ABS_Mandelbrot& FRAC,
	bool& Scale_Translate_Transformation, bool& Stretched_Image, fp64& Rotation_Difference
) {
	Stretched_Image = (
		(image.stretchX != 1.0) || (image.stretchY != 1.0) ||
		(FRAC.sX != 1.0) || (FRAC.sY != 1.0)
	) ? true : false;
	Scale_Translate_Transformation = (
		(image.realCord != FRAC.r) || (image.imagCord != FRAC.i) ||
		(image.stretchX != FRAC.sX) || (image.stretchY != FRAC.sY) ||
		(image.zoom != FRAC.zoom)
	) ? true : false;

	Rotation_Difference = fabs(FRAC.rot - image.rot);
}

/* SDL2 Frame Transformation */
__attribute__((unused)) static int Transform_Frame(const ImageBuffer& image, const Render_Data& ren) {
	if (image.vram == nullptr) { printError("const ImageBuffer& image.vram is nullptr"); return -1; }
	if (image.allocated() == false) { printError("const ImageBuffer& image is not allocated"); return -1; }
	if (image.resX <= 0 || image.resY <= 0) {
		printWarning("const ImageBuffer& image has no pixels to render: %" PRIu32 "x%" PRIu32, image.resX, image.resY);
		return 1;
	}
	const ABS_Mandelbrot& FRAC = current_Fractal;
	bool Scale_Translate_Transformation;
	bool Stretched_Image;
	fp64 Rotation_Difference = 0.0;
	calculate_Tranformation_Change(
		image, FRAC,
		Scale_Translate_Transformation, Stretched_Image, Rotation_Difference
	);
	if (Scale_Translate_Transformation == true) {
		// printfInterval(0.6,
		// 	"\nTransform_Frame_Scale_Translate\nRot: %5.1lf - %5.1lf = %5.1lf\n",
		// 	FRAC.rot * (360.0 / TAU), image.rot * (360.0 / TAU), Rotation_Difference * (360.0 / TAU)
		// );
		return Transform_Frame_Scale_Translate(image, ren, FRAC);
	} else if (Rotation_Difference != 0.0) {
		// printfInterval(0.6,
		// 	"\nTransform_Frame_Rotate\nRot: %5.1lf - %5.1lf = %5.1lf\n",
		// 	FRAC.rot * (360.0 / TAU), image.rot * (360.0 / TAU), Rotation_Difference * (360.0 / TAU)
		// );
		return Transform_Frame_Rotate(image, ren, FRAC);
	} else {
		// printfInterval(0.6,
		// 	"\nTransform_Frame_None\nRot: %5.1lf - %5.1lf = %5.1lf\n",
		// 	FRAC.rot * (360.0 / TAU), image.rot * (360.0 / TAU), Rotation_Difference * (360.0 / TAU)
		// );
		return Transform_Frame_None(image, ren);
	}
}

/* Affine Frame Transformation */
__attribute__((unused)) static int Manually_Transform_Frame(const ImageBuffer& image) {
	// nano64_t startTime = getNanoTime();
	if (image.vram == nullptr) { printError("const ImageBuffer& image.vram is nullptr"); return -1; }
	if (image.allocated() == false) { printError("const ImageBuffer& image is not allocated"); return -1; }
	ABS_Mandelbrot& FRAC = current_Fractal;

	BufferBox blit;
	getRenderBufferBoxFromMaster(blit);
	if (validateBufferBox(&blit) == false) {
		printError("Invalid blit BufferBox");
		return -1;
	}

	int ret_val = frame_Transform(image, blit, FRAC, config_data.Rendering_Settings);	

	renderJuliaCordinatePoint(blit);

	// nano64_t endTime = getNanoTime();
	// printfInterval(0.4,
	// 	"\ntime(%d): %.3lfms %.3lfFPS ", MT_Value,
	// 	NANO_TO_SECONDS(endTime - startTime) * 1.0e3,
	// 	NANO_TO_FRAMERATE(endTime - startTime)
	// );
	return ret_val;
}

#ifdef Enable_OpenCV_Scaler
	/* OpenCV Frame Transformation */
	static int transformFracImage(const ImageBuffer& image, const Render_Data& ren) {
		if (image.vram == nullptr) { printError("const ImageBuffer& image.vram is nullptr"); return -1; }
		if (image.allocated() == false) { printError("const ImageBuffer& image is not allocated"); return -1; }
		ABS_Mandelbrot& FRAC = current_Fractal;

		BufferBox blit;
		BufferBox temp_MASTER;
		Master.getBufferBox(&temp_MASTER);
		if (validateBufferBox(&temp_MASTER) == false) {
			printError("Invalid temp_MASTER BufferBox");
			return -1;
		}

		const User_Rendering_Settings& Rendering_Settings = config_data.Rendering_Settings;

		fp32 dx00 = 0.0f; fp32 dy00 = 0.0f; fp32 dx11 = 0.0f; fp32 dy11 = 0.0f;
		fp32 dx01 = 0.0f; fp32 dy01 = 0.0f; fp32 dx10 = 0.0f; fp32 dy10 = 0.0f;
		coordinate_to_pixel(image.x00, image.y00, dx00, dy00, FRAC, ren.resX, ren.resY);
		coordinate_to_pixel(image.x11, image.y11, dx11, dy11, FRAC, ren.resX, ren.resY);
		coordinate_to_pixel(image.x01, image.y01, dx01, dy01, FRAC, ren.resX, ren.resY);
		coordinate_to_pixel(image.x10, image.y10, dx10, dy10, FRAC, ren.resX, ren.resY);
		dim32_t resX = (dim32_t)(image.resX);
		dim32_t resY = (dim32_t)(image.resY);
		// printfInterval(0.5,
		// 	"\nres{%" PRIu32 "x%" PRIu32 "}"
		// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}"
		// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}\n",
		// 	resX,resY,
		// 	dx00,dy00,dx10,dy10,
		// 	dx01,dy01,dx11,dy11
		// );

		fp32 sx00 =       0.0f; fp32 sy00 =       0.0f;
		// fp32 sx11 = (fp32)resX; fp32 sy11 = (fp32)resY;
		fp32 sx01 =       0.0f; fp32 sy01 = (fp32)resY;
		fp32 sx10 = (fp32)resX; fp32 sy10 =       0.0f;
		// image->printTransformationData(0.6);
		// printfInterval(0.6,"\nsrc: 00{%" PRId32 ",%" PRId32 "} 11{%" PRId32 ",%" PRId32 "} 01{%" PRId32 ",%" PRId32 "} 10{%" PRId32 ",%" PRId32 "}",sx00,sy00,sx11,sy11,sx01,sy01,sx10,sy10);
		// printfInterval(0.6,"\ndst: 00{%" PRId32 ",%" PRId32 "} 11{%" PRId32 ",%" PRId32 "} 01{%" PRId32 ",%" PRId32 "} 10{%" PRId32 ",%" PRId32 "}\n",dx00,dy00,dx11,dy11,dx01,dy01,dx10,dy10);
		uint32_t backgroundColor = 0xFF000000;
		backgroundColor |= (uint32_t)(FRAC.exterior_R_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_R_Phase)));
		backgroundColor |= (uint32_t)(FRAC.exterior_G_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_G_Phase))) << 8;
		backgroundColor |= (uint32_t)(FRAC.exterior_B_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_B_Phase))) << 16;

		constexpr fp32 Minimum_Image_Size = 0.1f;
		fp32 Approximate_Image_Size = fabs(dx11 - dx00) * fabs(dy11 - dy00);
		if (Approximate_Image_Size < Minimum_Image_Size) {
			return -2; // Displays a loading graphic if the User jumps from being very zoomed in to very zoomed out as no more than 1 pixel would be rendered. Although I may have to alter this behaviour to account for intentionally zooming out
		}
		int Image_Scaler_Return_Value = Image_Scaler_Parallelogram(
			&blit, &image, &ren,
			backgroundColor,
			nullptr, nullptr,
			Rendering_Settings.Frame_Interpolation_Method,
			sx00, sy00,
			sx01, sy01, sx10, sy10,
			dx00, dy00,
			dx01, dy01, dx10, dy10
		);
		if (Image_Scaler_Return_Value < 0) {
			FREE(blit.vram);
			printError("\nImage_Scaler_Parallelogram failed (%d)", Image_Scaler_Return_Value);
			return -1;
		}
		renderJuliaCordinatePoint(blit);
		copyBuffer_VeritcalOffset(temp_MASTER, blit, (size_t)RESY_UI);
		FREE(blit.vram);
		
		// if (
		// 	Image_Scaler_Quadrilateral(
		// 		&blit, &image, &ren,
		// 		Rendering_Settings.Frame_Interpolation_Method,
		// 		sx00, sy00, sx11, sy11,
		// 		sx01, sy01, sx10, sy10,
		// 		dx00, dy00, dx11, dy11,
		// 		dx01, dy01, dx10, dy10
		// 	) == -1
		// ) {
		// 	FREE(blit.vram);
		// 	printError("\nImage_Scaler_Quadrilateral failed");
		// 	return -1;
		// }

		//printfInterval(0.6,"\n%p: %" PRIu32 "x%" PRIu32 " %" PRIu32 "C %" PRIu32 "P",blit.vram,blit.resX,blit.resY,blit.channels,blit.padding);
		
		// nano64_t startTime0 = getNanoTime();
		
			// uint8_t* dstBuf = temp_MASTER.vram;
			// Buffer_Data srcData; set_Buffer_Data(srcData,
			// 	blit.resX, blit.resY,
			// 	IMAGE_BUFFER_CHANNELS, (size_t)temp_MASTER.resX * IMAGE_BUFFER_CHANNELS
			// );
			// const uint8_t* srcBuf = blit.vram;
			// Buffer_Data dstData; set_Buffer_Data(dstData,
			// 	temp_MASTER.resX, temp_MASTER.resY,
			// 	IMAGE_BUFFER_CHANNELS, (size_t)temp_MASTER.resX * IMAGE_BUFFER_CHANNELS
			// );
			// BufferCopy(dstBuf, srcBuf, dstData, srcData, 0, (int32_t)RESY_UI, true);
		// nano64_t endTime0 = getNanoTime();
		// nano64_t startTime1 = getNanoTime();
		// 	copyBuffer(blit,temp_MASTER,0,(int32_t)RESY_UI,true);
		// nano64_t endTime1 = getNanoTime();

		// printfInterval(0.1,"\ntime0: %.3lf\ntime1: %.3lf\n",
		// 	(fp64)(endTime0 - startTime0) / 1.0e6,
		// 	(fp64)(endTime1 - startTime1) / 1.0e6
		// );

		return 0;
	}
#endif

void fill_Background_Color(const ImageBuffer& image) {
	if (Render_Background_Color == false) {
		Master.clearBuffer();
		return;
	}
	const ABS_Mandelbrot& FRAC = current_Fractal;
	uint8_t frac_R = (uint8_t)(FRAC.exterior_R_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_R_Phase)));
	uint8_t frac_G = (uint8_t)(FRAC.exterior_G_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_G_Phase)));
	uint8_t frac_B = (uint8_t)(FRAC.exterior_B_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_B_Phase)));
	uint32_t frac_Color = (uint32_t)frac_R + ((uint32_t)frac_G << 8) + ((uint32_t)frac_B << 16);
	
	if (image.vram == nullptr || image.resX < 2 || image.resY < 2) {
		Master.clearBuffer(frac_R, frac_G, frac_B);
		return;
	}
	uint32_t aver_R = 0, aver_G = 0, aver_B = 0;
	
	const size_t pitch = (size_t)image.resX * image.channels;
	const size_t point_List[] = {
		/* NW */ 0,
		/* NE */ ((size_t)image.resX - 1) * image.channels,
		/* SW */ ((size_t)image.resY - 1) * pitch,
		/* SE */ (((size_t)image.resX - 1) * image.channels) + (((size_t)image.resY - 1) * pitch)
	};
	const size_t point_Count = ARRAY_LENGTH(point_List);

	for (size_t i = 0; i < point_Count; i++) {
		uint8_t corner_R = image.vram[point_List[i] + 0];
		uint8_t corner_G = image.vram[point_List[i] + 1];
		uint8_t corner_B = image.vram[point_List[i] + 2];
		uint32_t corner_Color = (uint32_t)corner_R + ((uint32_t)corner_G << 8) + ((uint32_t)corner_B << 16);
		if (corner_Color == frac_Color) {
			Master.clearBuffer(frac_R, frac_G, frac_B);
			return;
		}
		aver_R += corner_R * corner_R;
		aver_G += corner_G * corner_G;
		aver_B += corner_B * corner_B;
	}
	aver_R /= point_Count;
	aver_G /= point_Count;
	aver_B /= point_Count;
	aver_R = (uint32_t)sqrt((fp64)aver_R);
	aver_G = (uint32_t)sqrt((fp64)aver_G);
	aver_B = (uint32_t)sqrt((fp64)aver_B);
	Master.clearBuffer((uint8_t)aver_R, (uint8_t)aver_G, (uint8_t)aver_B);
}

void newFrame() {
	// nano64_t startTime = getNanoTime();

	__attribute__((unused)) constexpr fp64 Maximum_Rotation_Difference = 0.0 * (TAU / 360.0);
	if (Master.bufferSafe() == false) {
		printError("Master ImageBuffer is invalid");
		return;
	}

	if (exportSuperFractalBuffer == true) {
		exportSuperFractalBuffer = false;
	}

	int primaryBufferChange = next_Read_Cycle_Pos(&Primary_Image, Primary_Full);
	bool primaryBufferValid = (primaryBufferChange < 0 || Primary_Image == nullptr || Primary_Image->bufferSafe() == false) ? false : true;
	
	/* Temporary code that will force the rendering Engine to Update at bootup */
	if (primaryBufferValid == true) {
		bootup_Fractal_Frame_Rendered = true;
	} else if (bootup_Fractal_Frame_Rendered == false) {
		write_Update_Level(Change_Level::Full_Reset);
	}

	if (primaryBufferValid == true) {
		fill_Background_Color(*Primary_Image);
	} else {
		Master.clearBuffer();
	}

	if (Abort_Rendering_Flag == true) {
		//primaryBufferValid = false;
		Waiting_To_Abort_Rendering = read_Abort_Render_Ongoing();
		BufferBox render_Area; getRenderBufferBoxFromMaster(render_Area);
		if (Waiting_To_Abort_Rendering == true) {
			renderStatusGraphic(render_Area, Status_Graphic::Graphic_Abort, 0.3);
		} else {
			renderStatusGraphic(render_Area, Status_Graphic::Graphic_Pause, 0.4);
			//renderTestGraphic(0.2,0.4,1.0);
		}
		exportFractalBuffer = false;
	} else if (primaryBufferValid == false) {
		BufferBox render_Area; getRenderBufferBoxFromMaster(render_Area);
		renderStatusGraphic(render_Area, Status_Graphic::Graphic_Loading, 1.0); // Renders a loading screen if Fractal buffers are unavailable
		exportFractalBuffer = false;
	}
	#ifdef Enable_OpenCV_Scaler
		if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
			int scaleRet = transformFracImage(*Primary_Image, primaryRenderData);
			// int scaleRet = Manually_Transform_Frame(*Primary_Image);
			if (scaleRet == -2) { // Scaled Image is too small
				BufferBox render_Area; getRenderBufferBoxFromMaster(render_Area);
				renderStatusGraphic(render_Area, Status_Graphic::Graphic_Loading, 1.0);
			}
		}
	#endif
	#ifndef Enable_OpenCV_Scaler
		bool Enable_SDL2_Scaler = true;
		if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
			bool Scale_Translate_Transformation;
			bool Stretched_Image;
			fp64 Rotation_Difference = 0.0;
			calculate_Tranformation_Change(
				*Primary_Image, current_Fractal,
				Scale_Translate_Transformation, Stretched_Image, Rotation_Difference
			);
			if (
				(
					(Scale_Translate_Transformation == true) &&
					(Rotation_Difference > Maximum_Rotation_Difference)
				) || (
					(Stretched_Image == true) &&
					(Rotation_Difference != 0.0)
				) || (
					(config_data.Rendering_Settings.JuliaPoint_Enabled == true) &&
					(current_Fractal.zr != (fpCord)0.0 || current_Fractal.zi != (fpCord)0.0)
				)
			) {
				Enable_SDL2_Scaler = false;
				// BufferBox render_Area; getRenderBufferBoxFromMaster(render_Area);
				// renderStatusGraphic  (render_Area, Status_Graphic::Graphic_Loading, 1.0);
				Manually_Transform_Frame(*Primary_Image);
			}
		}
	#endif
	SDL_UpdateTexture(texture, nullptr, Master.vram, (dim32_t)Master.resX * (dim32_t)Master.channels);
	{
		SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_Rect dstRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	}
	if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
		BufferBox temp_primaryBox;
		Primary_Image->getBufferBox(&temp_primaryBox);

		#ifndef Enable_OpenCV_Scaler
			if (Enable_SDL2_Scaler == true) {
				int dispRet = Transform_Frame(*Primary_Image, primaryRenderData);
				printfChange(int, dispRet, "\ndisplayFracImage: %" PRId32, dispRet);
			}
		#endif
		if (exportFractalBuffer == true) {
			nano64_t curTime = getNanoTime();
			const char* fractal_name = (current_Fractal.polarMandelbrot == true) ? FractalTypeFileText[Fractal_Polar_Mandelbrot] : FractalTypeFileText[Fractal_ABS_Mandelbrot];
			size_t size = (size_t)snprintf(nullptr, 0, "%s_%" PRIu64, fractal_name, curTime);
			char* name = (char*)calloc(size + 1,sizeof(char));
			snprintf(name, size, "%s_%" PRIu64, fractal_name, curTime);
			const char* path = config_data.File_Paths.Path_Screenshot.c_str();
			const User_Screenshot_Settings& screenshot_settings = config_data.Screenshot_Settings;
			switch(screenshot_settings.screenshotFileType) {
				case Image_File_Format::PNG:
					writePNGImage(&temp_primaryBox,path,name,screenshot_settings.PNG_Compression_Level);
				break;
				case Image_File_Format::JPG:
					writeJPGImage(&temp_primaryBox,path,name,screenshot_settings.JPG_Quality_Level);
				break;
				case Image_File_Format::TGA:
					writeTGAImage(&temp_primaryBox,path,name);
				break;
				case Image_File_Format::BMP:
					writeBMPImage(&temp_primaryBox,path,name);
				break;
				default:
				printError("Unknown screenshot file type: %" PRId32,screenshot_settings.screenshotFileType);
			}
			FREE(name);
		}
		exportFractalBuffer = false;
	}
	
	render_IMGUI();
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(kTexture); // From render_IMGUI

	// nano64_t endTime = getNanoTime();
	// printfInterval(0.4,
	// 	"\ntime: %.3lfms %.3lfFPS ",
	// 	NANO_TO_SECONDS(endTime - startTime) * 1.0e3,
	// 	NANO_TO_FRAMERATE(endTime - startTime)
	// );
}
