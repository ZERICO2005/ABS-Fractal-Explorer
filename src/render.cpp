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
#include "BufferCopy.hpp"
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
#include "programData.h"
#include "user_data.h"

#include "menu_Interface/display_GUI.h"
#include "displayInfo.h"

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
					if (func <= Key_Function::SCREEN_SPLIT || func >= Key_Function::FUNCTIONS) {
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
			{SCREEN_SPLIT,30.0,0.3,1.0},{FUNCTIONS,120.0,0.8,0.8},{RENDERING,180.0,0.4,0.6},
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
				printFlush("\nFracExp_KeyBind File: %s",argv[a]);
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

void correctUsernameText(char* buf, size_t len) { /* Strips characters */
	size_t p = 0;
	for (size_t i = 0; i < strnlen(buf,len); i++) {
		#ifdef displayTerribleProgrammingJokes
			if (i == 0) {
				while (i < strnlen(buf,len) && (buf[i] >= '0' && buf[i] <= '9')) {
					i++;
				}
			}
		#endif
		if (!(
			(buf[i] >= 'A' && buf[i] <= 'Z') || 
			(buf[i] >= 'a' && buf[i] <= 'z') ||
			(buf[i] >= '0' && buf[i] <= '9') ||
			(buf[i] == '_')
		)) {
			if ((buf[i] == ' ') || (buf[i] == '-') || (buf[i] == '.') || (buf[i] == '~')) {
				buf[i] = '_';
			} else {
				continue;
			}
		}
		buf[p] = buf[i];
		p++;
	}
	for (;p < len; p++) {
		buf[p] = '\0';
	}
}

int32_t utitledFileNameGenerator(char* buf, size_t maxLen) {
	static const char* UntitledFile_Front[] = { // Easier to read atomic names
		"Aluminum","Argon","Beryllium","Bismuth","Boron","Bromine","Calcium","Carbon","Cesium","Chlorine","Chromium","Cobalt","Copper","Fluorine","Gallium","Gold","Hafnium","Helium","Hydrogen","Iodine","Iridium","Iron","Krypton","Lithium","Magnesium","Neon","Neptunium","Nickel","Nitrogen","Osmium","Oxygen","Phosphorus","Platinum","Plutonium","Potassium","Rhodium","Silicon","Silver","Sodium","Sulfur","Technetium","Thorium","Titanium","Tungsten","Uranium","Vanadium","Xenon"
	};
	static const char* UntitledFile_Middle[] = { // Common color names
		"Amber","Aquamarine","Beige","Black","Blue","Brown","Charcoal","Cyan","Fuchsia","Green","Grey","Indigo","Lime","Magenta","Maroon","Mint","Olive","Orange","Pink","Purple","Red","Teal","Turquoise","Violet","White","Yellow"
	};
	static const char* UntitledFile_End[] = { // Shapes and mathematical terms
		"Cardiod","Catenary","Circle","Cube","Cycloid","Cylinder","Diamond","Dodecagon","Ellispse","Exponential","Hexagon","Hyperbola","Icosahedron","Logarithm","Nephroid","Octogon","Parabola","Parallelogram","Pentagon","Polynomial","Rectangle","Rhombus","Sphere","Square","Star","Tangent","Tesseract","Tetreahedron","Trapozoid","Triangle","Vertex"
	};
	srand((unsigned int)getNanoTime());
	uint32_t choice_front = (uint32_t)rand() % (uint32_t)ARRAY_LENGTH(UntitledFile_Front);
	uint32_t choice_middle = (uint32_t)rand() % (uint32_t)ARRAY_LENGTH(UntitledFile_Middle);
	uint32_t choice_end = (uint32_t)rand() % (uint32_t)ARRAY_LENGTH(UntitledFile_End);
	return snprintf(buf,maxLen,"%s-%s-%s",UntitledFile_Front[choice_front],UntitledFile_Middle[choice_middle],UntitledFile_End[choice_end]);
};
/*
void initFunctionTimers() {
	using namespace Key_Function;
	for (size_t f = 0; f < Parameter_Function_Count; f++) {
		func_stat[f].freqTime = 0;
		func_stat[f].resetTime = getNanoTime();
	}
	func_stat[incFormula].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[decFormula].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[incFamily].freqTime = SECONDS_TO_NANO(1.0/10.0);
	func_stat[decFamily].freqTime = SECONDS_TO_NANO(1.0/10.0);
	func_stat[incPower].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[decPower].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[incSubSample].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[decSubSample].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[incSuperSample].freqTime = SECONDS_TO_NANO(1.0/6.0);
	func_stat[decSuperSample].freqTime = SECONDS_TO_NANO(1.0/6.0);
}
*/

// enum Menu_Enum {GUI_Menu_None, GUI_Menu_Coordinates, GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds, GUI_Menu_Count};

// #define stretchValue(s) pow(2.0,-abs(s))
// #define zoomDefault(p) (-log10(getABSFractalMaxRadius((fp64)(p))) - 0.01)

template<typename fpX>
void moveCord(fpX* x, fpX* y, fp64 angle, fp64 speed) {
	*x += (fpX)speed * cos((fpX)angle);
	*y += (fpX)speed * sin((fpX)angle);
}

bool funcTimeDelay(Key_Function::Key_Function_Enum func) {
	if (getNanoTime() - func_stat[func].resetTime > func_stat[func].freqTime) {
		if (func_stat[func].triggered == true) {
			func_stat[func].resetTime = getNanoTime();
			return true;
		}
	}
	return false;
}

bool funcTimeDelay(Key_Function::Key_Function_Enum func, fp64 freq) {
	if (getNanoTime() - func_stat[func].resetTime > SECONDS_TO_NANO(freq)) {
		if (func_stat[func].triggered == true) {
			func_stat[func].resetTime = getNanoTime();
			return true;
		}
	}
	return false;
}

inline void Update_Level(int_enum& update_level, int_enum level) {
	update_level = (level > update_level) ? level : update_level;
}

int get_ABS_Mandelbrot_Update_Level(ABS_Mandelbrot* frac_data, Render_Data* ren, int_enum update_level = Change_Level::Nothing) {
	if (frac_data == nullptr) { return update_level; }
	if (ren == nullptr) { return update_level; }
	static ABS_Mandelbrot frac0 = *frac_data;
	static Render_Data ren0 = *ren;
	using namespace Change_Level;
	
	/* Render_Data */
	if (ren->resX != ren0.resX || ren->resY != ren0.resY) {
		Update_Level(update_level, Change_Level::Resolution);
	}
	if (
		(ren->render_precision != ren0.render_precision) ||
		(ren->render_method != ren0.render_method) ||
		(ren->render_preset != ren0.render_preset)
	) {
		Update_Level(update_level, Change_Level::Method_of_Rendering);
	}
	if (ren->sample != ren0.sample) {
		Update_Level(update_level, Change_Level::Super_Sample);
	}
	if (ren->subSample != ren0.subSample) {
		Update_Level(update_level, Change_Level::Resolution);
	}
	if (ren->flip != ren0.flip) {
		Update_Level(update_level, Change_Level::Rotation);
	}
	/* ABS_Mandelbrot */
	if (frac_data->r != frac0.r || frac_data->i != frac0.i) {
		Update_Level(update_level, Change_Level::Translation);
	}
	if (frac_data->zoom != frac0.zoom) {
		Update_Level(update_level, Change_Level::Zoom);
	}
	if (frac_data->zr != frac0.zr || frac_data->zi != frac0.zi) {
		Update_Level(update_level, Change_Level::Julia);
	}
	if (frac_data->maxItr != frac0.maxItr || frac_data->maxItr_Log2 != frac0.maxItr_Log2) {
		Update_Level(update_level, Change_Level::Iterations);
	}
	if (frac_data->rot != frac0.rot) {
		Update_Level(update_level, Change_Level::Rotation);
	}
	if (frac_data->stretch != frac0.stretch) {
		Update_Level(update_level, Change_Level::Stretch);
	}
	if (frac_data->breakoutValue != frac0.breakoutValue) {
		Update_Level(update_level, Change_Level::Breakout);
	}
	if (
		(frac_data->exterior_R_Amp != frac0.exterior_R_Amp || frac_data->exterior_R_Freq != frac0.exterior_R_Freq || frac_data->exterior_R_Phase != frac0.exterior_R_Phase) ||
		(frac_data->exterior_G_Amp != frac0.exterior_G_Amp || frac_data->exterior_G_Freq != frac0.exterior_G_Freq || frac_data->exterior_G_Phase != frac0.exterior_G_Phase) ||
		(frac_data->exterior_B_Amp != frac0.exterior_B_Amp || frac_data->exterior_B_Freq != frac0.exterior_B_Freq || frac_data->exterior_B_Phase != frac0.exterior_B_Phase) ||
		(frac_data->interior_R_Amp != frac0.interior_R_Amp || frac_data->interior_R_Freq != frac0.interior_R_Freq || frac_data->interior_R_Phase != frac0.interior_R_Phase) ||
		(frac_data->interior_G_Amp != frac0.interior_G_Amp || frac_data->interior_G_Freq != frac0.interior_G_Freq || frac_data->interior_G_Phase != frac0.interior_G_Phase) ||
		(frac_data->interior_B_Amp != frac0.interior_B_Amp || frac_data->interior_B_Freq != frac0.interior_B_Freq || frac_data->interior_B_Phase != frac0.interior_B_Phase) ||
		(frac_data->exterior_Alpha != frac0.exterior_Alpha || frac_data->interior_Alpha != frac0.interior_Alpha)
	) {
		Update_Level(update_level, Change_Level::Coloring);
	}
	if (frac_data->smoothColoring != frac0.smoothColoring) {
		Update_Level(update_level, Change_Level::Coloring);
	}
	if (frac_data->power != frac0.power) {
		Update_Level(update_level, Change_Level::Power_Change);
	}
	if (frac_data->polarPower != frac0.polarPower) {
		Update_Level(update_level, Change_Level::Polar_Power);
	}
	if (frac_data->formula != frac0.formula) {
		Update_Level(update_level, Change_Level::Formula_Change);
	}
	if (frac_data->polarMandelbrot != frac0.polarMandelbrot) {
		Update_Level(update_level, Change_Level::Fractal_Change);
	}
	if (frac_data->juliaSet != frac0.juliaSet) {
		Update_Level(update_level, Change_Level::Fractal_Change);
	}
	frac0 = *frac_data;
	ren0 = *ren;
	return update_level;
}

// #define BufAndLen(x) x,ARRAY_LENGTH(x)

// bool bootup_Fractal_Frame_Rendered = false;

// TimerBox GUI_FrameTimer;
// nano64_t GUI_FrameTimeNano = SECONDS_TO_NANO(1.0/60.0);
// fp64 GUI_FrameTime = 1.0/60.0;
// fp64 GUI_FrameRate = 60.0;

inline void paramToggle(Key_Function::Key_Function_Enum func, bool& toggle, fp64 freq) {
	if (funcTimeDelay(func,freq)) {
		toggle = !toggle;
	}
}

int_enum updateFractalParameters() {
	using namespace Key_Function;
	using namespace Change_Level;
	ABS_Mandelbrot& FRAC = current_Fractal;
	fp64 temp_breakoutValue = log2(FRAC.breakoutValue);
	fp64 moveDelta = (DeltaTime < 0.2) ? DeltaTime : 0.2;
	
	const User_Parameter_Sensitivity& config_sensitivity = config_data.Parameter_Sensitivity;

	moveDelta *= config_sensitivity.global;

	int_enum update_level = Change_Level::Nothing;

	/* Magic Constants */
		#define ABS_Mandelbrot_Default_Power 2
		#define Polar_Mandelbrot_Default_Power 3.0
	/* Boolean toggles */

		#define paramToggleUpdate(func, toggle, freq, level) if (funcTimeDelay(func, freq)) { toggle = !toggle; Update_Level(update_level, level); }
		
		paramToggle(toggleAdjustZoomToPower, FRAC.adjustZoomToPower, 0.4);
		paramToggleUpdate(toggleJulia, FRAC.juliaSet, 0.4, Major_Reset);
		paramToggleUpdate(toggleABSandPolarMandelbrot, FRAC.polarMandelbrot, 0.4, Major_Reset);
		paramToggle(toggleRelativeZValue, FRAC.relativeZValue, 0.4);
		paramToggle(toggleCursorZValue, FRAC.cursorZValue, 0.4);
		paramToggleUpdate(toggleStartingZ, FRAC.startingZ, 0.4, Minor_Reset);
		paramToggleUpdate(toggleIntegerPower, FRAC.integerPolarPower, 0.4, Minor_Reset);
		
	/* Real and Imaginary Coordinates */
		if (func_stat[incRealPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.0 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.coordinate
			);
		}
		if (func_stat[decRealPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.5 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.coordinate
			);
		}
		if (func_stat[incImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.25 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.coordinate
			);
		}
		if (func_stat[decImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.75 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.coordinate
			);
		}
		if (funcTimeDelay(resetRealPos, 0.2)) {
			FRAC.r = 0.0;
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetImagPos, 0.2)) {
			FRAC.i = 0.0;
			Update_Level(update_level, Jump);
		}
	/* Real and Imaginary Julia Z Coordinates */
		if (func_stat[incZReal].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.0 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.julia
			);
		}
		if (func_stat[decZReal].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.5 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.julia
			);
		}
		if (func_stat[incZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.25 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.julia
			);
		}
		if (func_stat[decZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.75 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.julia
			);
		}
		if (funcTimeDelay(resetZReal, 0.2)) {
			FRAC.zr = 0.0;
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetZImag, 0.2)) {
			FRAC.zi = 0.0;
			Update_Level(update_level, Jump);
		}
		if (FRAC.cursorZValue == true) {
			if (FRAC.relativeZValue == true) {
				fp64 resZ = (fp64)((Master.resX > Master.resY) ? Master.resY : Master.resX);
				FRAC.zr = 4.0 * ((fp64)ImGui::GetMousePos().x - ((fp64)Master.resX / 2.0)) / resZ;
				FRAC.zi = 4.0 * ((fp64)(ImGui::GetMousePos().y - (fp64)RESY_UI) - ((fp64)Master.resY / 2.0)) / resZ;
			} else {
				pixel_to_coordinate(
					(int32_t)(ImGui::GetMousePos().x), (int32_t)ImGui::GetMousePos().y - (int32_t)RESY_UI,
					&FRAC.zr, &FRAC.zi, &FRAC, &primaryRenderData
				);
			}
		}
	/* Zoom */
		if (func_stat[incZoom].triggered == true) {
			FRAC.zoom += 0.25 * moveDelta * config_sensitivity.zoom * (config_sensitivity.invert_zoom ? -1.0 : 1.0);
		}
		if (func_stat[decZoom].triggered == true) {
			FRAC.zoom -= 0.25 * moveDelta * config_sensitivity.zoom * (config_sensitivity.invert_zoom ? -1.0 : 1.0);
		}
		if (funcTimeDelay(resetZoom, 0.2)) {
			FRAC.zoom = zoomDefault(FRAC.power);
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetCoordinates, 0.2)) {
			FRAC.r = 0.0; FRAC.i = 0.0;
			FRAC.stretch = 0.0; FRAC.rot = 0.0;
			if (FRAC.polarMandelbrot == true) {
				FRAC.zoom = zoomDefault(FRAC.polarPower);
			} else {
				FRAC.zoom = zoomDefault((fp64)FRAC.power);
			}
			valueClamp(FRAC.zoom, -0.4, 0.4);
			Update_Level(update_level, Jump);
		}
	/* maxItr */
		if (func_stat[incMaxItr].triggered) {
			FRAC.maxItr_Log2 += 1.8 * moveDelta * config_sensitivity.maxIter;
			setMaxItr(&FRAC, FRAC.maxItr_Log2);
		}
		if (func_stat[decMaxItr].triggered) {
			FRAC.maxItr_Log2 -= 1.8 * moveDelta * config_sensitivity.maxIter;
			setMaxItr(&FRAC, FRAC.maxItr_Log2);
		}
		if (funcTimeDelay(resetMaxItr, 0.2)) {
			FRAC.maxItr_Log2 = log2(192.0);
			FRAC.maxItr = 192;
			setMaxItr(&FRAC, FRAC.maxItr_Log2);
		}
	/* Formula*/
		if (funcTimeDelay(incFormula ,1.0/10.0)) {
			FRAC.formula++;
		}
		if (funcTimeDelay(decFormula, 1.0/10.0)) {
			FRAC.formula--;
		}
		if (funcTimeDelay(incFamily, 1.0/10.0)) {
			FRAC.formula += getABSValue(FRAC.power);
		}
		if (funcTimeDelay(decFamily, 1.0/10.0)) {
			FRAC.formula -= getABSValue(FRAC.power);
		}
		if (funcTimeDelay(resetFormula, 0.2)) {
			FRAC.formula = 0;
		}
	/* Power */
		if (FRAC.polarMandelbrot == true) {
			if (FRAC.integerPolarPower == true) {
				if (funcTimeDelay(incPower, 1.0/6.0)) {
					FRAC.polarPower++;
				}
				if (funcTimeDelay(decPower, 1.0/6.0)) {
					FRAC.polarPower--;
				}
			} else {
				if (func_stat[incPower].triggered) {
					FRAC.polarPower += moveDelta * 1.0 * config_sensitivity.polar_power;
				}
				if (func_stat[decPower].triggered) {
					FRAC.polarPower -= moveDelta * 1.0 * config_sensitivity.polar_power;
				}
			}
			
			if (funcTimeDelay(resetPower, 0.2)) {
				FRAC.polarPower = Polar_Mandelbrot_Default_Power;
			}
			if (funcTimeDelay(roundPower, 0.2)) {
				FRAC.polarPower = round(FRAC.polarPower);
			}
			if (funcTimeDelay(floorPower, 0.2)) {
				FRAC.polarPower = floor(FRAC.polarPower);
			}
			if (funcTimeDelay(ceilingPower, 0.2)) {
				FRAC.polarPower = ceil(FRAC.polarPower);
			}
		} else {
			if (funcTimeDelay(incPower, 1.0/6.0)) {
				FRAC.power++;
			}
			if (funcTimeDelay(decPower, 1.0/6.0)) {
				FRAC.power--;
			}
			if (funcTimeDelay(resetPower, 0.2)) {
				FRAC.power = ABS_Mandelbrot_Default_Power;
			}
			FRAC.formula = limitFormulaID(FRAC.power,FRAC.formula);
		}
	/* Rotations */
		if (func_stat[counterclockwiseRot].triggered) {
			FRAC.rot -= (TAU / 3.0) * moveDelta * getStretchValue(FRAC.stretch) * config_sensitivity.rotation;
		}
		if (func_stat[clockwiseRot].triggered) {
			FRAC.rot += (TAU / 3.0) * moveDelta * getStretchValue(FRAC.stretch) * config_sensitivity.rotation;
		}
		if (funcTimeDelay(clockwiseRot90, 0.3)) {
			FRAC.rot += (TAU * (90.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRot90, 0.3)) {
			FRAC.rot -= (TAU * (90.0/360.0));
		}
		if (funcTimeDelay(rotate180,0.3)) {
			FRAC.rot += (TAU * (180.0/360.0));
		}
		if (funcTimeDelay(clockwiseRotStep, 1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRotStep, 1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(clockwiseRotPower, 1.0/6.0)) {
			FRAC.rot += (TAU * (1.0 / (fp64)((FRAC.power - 1) * 2)));
		}
		if (funcTimeDelay(counterclockwiseRotPower, 1.0/6.0)) {
			FRAC.rot -= (TAU * (1.0 / (fp64)((FRAC.power - 1) * 2)));
		}
		if (funcTimeDelay(resetRotation, 0.2)) {
			FRAC.rot = 0.0;
		}
		FRAC.rot = (FRAC.rot >= 0.0) ? fmod(FRAC.rot,TAU) : fmod(FRAC.rot + TAU,TAU);
	/* Transformations */
		if (func_stat[incStretch].triggered) {
			FRAC.stretch += 1.0 * moveDelta * config_sensitivity.stretch;
		}
		if (func_stat[decStretch].triggered) {
			FRAC.stretch -= 1.0 * moveDelta * config_sensitivity.stretch;
		}
		if (funcTimeDelay(resetStretch, 0.2)) {
			FRAC.stretch = 0.0;
		}
		if (funcTimeDelay(resetTransformations, 0.2)) {
			FRAC.rot = 0.0;
			FRAC.stretch = 0.0;
		}
	/* Breakout Value */
		if (func_stat[incBreakout].triggered) {
			temp_breakoutValue += 2.0 * moveDelta * config_sensitivity.breakout_value;
		}
		if (func_stat[decBreakout].triggered) {
			temp_breakoutValue -= 2.0 * moveDelta * config_sensitivity.breakout_value;
		}
		if (funcTimeDelay(resetBreakout, 0.2)) {
			temp_breakoutValue = log2(16777216.0);
		}
	/* Rendering */
		if (funcTimeDelay(incSubSample, 1.0/6.0)) {
			primaryRenderData.subSample++;
		}
		if (funcTimeDelay(decSubSample, 1.0/6.0)) {
			primaryRenderData.subSample--;
		}
		if (funcTimeDelay(resetSubSample, 0.2)) {
			primaryRenderData.subSample = 1;
		}
		valueClamp(primaryRenderData.subSample, 1, 24);
		if (funcTimeDelay(incSuperSample, 1.0/6.0)) {
			primaryRenderData.sample++;
		}
		if (funcTimeDelay(decSuperSample, 1.0/6.0)) {
			primaryRenderData.sample--;
		}
		if (funcTimeDelay(resetSuperSample, 0.2)) {
			primaryRenderData.sample = 1;
		}
		valueClamp(primaryRenderData.sample, 1, 24);
	/* Rendering Method */
	{
		using namespace Legacy_Rendering_Method;
		using namespace Rendering_Configuration;
		if (funcTimeDelay(fp32CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision(Render_Precision_Float32)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp64CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision(Render_Precision_Float64)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp80CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision(Render_Precision_Float80)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp128CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision(Render_Precision_Float128)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp16GpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float16)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp32GpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float32)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp64GpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float64)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		
		primaryRenderData.render_precision = Render_Config.get_Render_Precision();
		primaryRenderData.render_method = Render_Config.get_Render_Method();
		primaryRenderData.render_preset = Render_Config.get_Render_Preset();

		secondaryRenderData.render_precision = Render_Config.get_Render_Precision();
		secondaryRenderData.render_method = Render_Config.get_Render_Method();
		secondaryRenderData.render_preset = Render_Config.get_Render_Preset();
		// printfInterval(0.3,"\nRender: %s | %s %s",
		// 	Rendering_Preset_Name[Render_Config.get_Render_Preset()],
		// 	Rendering_Preset_Name[primaryRenderData.render_preset],
		// 	Rendering_Preset_Name[secondaryRenderData.render_preset]
		// );
	}
	/* Other */
	FRAC.breakoutValue = pow(2.0,temp_breakoutValue);
	correctFracParameters(&FRAC);
	update_level = get_ABS_Mandelbrot_Update_Level(&FRAC,&primaryRenderData,update_level);
	/* ABS Mandelbrot */
	/* Polar Mandelbrot */
	/* Global Application Functions */
	#define GUI_MENU_TOGGLE(m) buttonSelection = (buttonSelection == (m)) ? -1 : (m);
		if (funcTimeDelay(inputFormula,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Coordinates);
		}
		if (funcTimeDelay(inputPower,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
		}
		if (funcTimeDelay(openFractalMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
		}
		if (funcTimeDelay(openKeyBindsMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_KeyBinds);
		}
		if (funcTimeDelay(openRenderingMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Rendering);
		}
		if (funcTimeDelay(openSettingsMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Settings);
		}
		if (funcTimeDelay(toggleFullscreen,0.4)) {
			toggle_Window_Fullscreen_Mode();
		}
		if (funcTimeDelay(refreshFractal,0.4)) {
			//write_Update_Level(Change_Level::Refresh);
			write_Update_Level(Change_Level::Moderate_Reset);
		}
		if (funcTimeDelay(takeScreenshot,0.4)) {
			exportScreenshot();
		}
		if (funcTimeDelay(takeSuperScreenshot,0.4)) {
			exportSuperScreenshot();
		}
	write_Update_Level(update_level);
	return update_level;
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
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				ABORT_RENDERING = true;
				QUIT_FLAG = true;
			}
		}
		updateKeys();
		if (GUI_FrameTimer.timerReset()) {
			DeltaTime = GUI_FrameTimer.getDeltaTime();
			fp64 RenderTime = getRenderDelta();
			{
				static fp64 maxFrameTime = 0.0;
				static fp64 maxRenderTime = 0.0;
				if (maxFrameReset.timerReset()) {
					Frame_Time_Display = maxFrameTime;
					Render_Time_Display = maxRenderTime;
					maxFrameTime = 0.0;
					maxRenderTime = 0.0;			
				}
				if (DeltaTime > maxFrameTime) {
					maxFrameTime = DeltaTime;
				}
				if (RenderTime > maxRenderTime) {
					maxRenderTime = RenderTime;
				}
			}
			windowResizingCode();
			updateFractalParameters();
			write_Parameters(&current_Fractal, &primaryRenderData, &secondaryRenderData);
			newFrame();
		}
	}
	return 0;
}

// int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING) {
// 	uint64_t yeildTimeNano = 80000; /* 80 micro seconds */
// 	uint64_t FRAME_RATE_NANO = SECONDS_TO_NANO(1.0 / FRAME_RATE);
// 	//printFlush("\nyeildTimeNano: %llu | %lf",yeildTimeNano,NANO_TO_SECONDS(yeildTimeNano));
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
// 					printFlush("\n%.3lfus",(frameTimer.timeElapsed() - (1.0/FRAME_RATE)) * 1.0e6);
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
// 			printFlush("\n\nElapsed: %6lluus TimeLeft:  %6lluus",timeElapsed/TIME_SCALE, timeLeft/TIME_SCALE);
// 			if (timeLeft > END_SLEEP_HEADROOM) {
// 				uint64_t sleepTime = timeLeft - END_SLEEP_HEADROOM;
// 				std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
// 				uint64_t remainingTime = frameTimer.timeToTimerReadyNano();
// 				printFlush("\nSleep:   %6lluus Remaining: %6lluus",sleepTime/TIME_SCALE,remainingTime/TIME_SCALE);
// 			}
// 		}
// 		*/
// 		if (frameTimer.timerReset()) {
// 			DeltaTime = frameTimer.getDeltaTime();
// 			fp64 RenderTime = getRenderDelta();
// 			{
// 				static fp64 maxFrameTime = 0.0;
// 				static fp64 maxRenderTime = 0.0;
// 				if (maxFrameReset.timerReset()) {
// 					Frame_Time_Display = maxFrameTime;
// 					Render_Time_Display = maxRenderTime;
// 					maxFrameTime = 0.0;
// 					maxRenderTime = 0.0;
// 					/*
// 					if (yeildPrint == true || yeildSwitch == true) {
// 						yeildSwitch = false;
// 						yeildPrint = false;
// 						printFlush("\n\nYeild Count: %llu Errors: %" PRIu64,yeildCount, yeildError);
// 						printFlush("\nTime Saved: %.3lfms",NANO_TO_SECONDS(yeildSave) * 1.0e3);
// 						yeildSum += yeildCount;
// 						printFlush("\nYeild Error: %.3lfms per error\n",((fp64)(getNanoTime() - yeildTimer) / (fp64)yeildError) / 1.0e6);
// 						yeildCount = 0;
// 					}
// 					*/				
// 				}
// 				if (DeltaTime > maxFrameTime) {
// 					maxFrameTime = DeltaTime;
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

void init_config_data() {
	if (import_config_data(config_data,"./config.fracExpConfig") == 0) {
		if (config_data.Automatic_Behaviour.AutoLoad_Config_File == false) {
			default_User_Configuration_Data(config_data, true);
			return;
		}
		clean_User_Configuration_Data(config_data);
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
	
	export_config_data(config_data,"./config.fracExpConfig");
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
	printf("\n\tDisplay Count: %" PRId32,displayCount);
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
	if (config_data.Display_Preferences.ScaleWindowToScreenSize == true) {
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
		initResX = calcMinMaxRatio(initResX,RESX_Minimum,RESX_Default,0.6);
		if (initResX > RESX_Maximum) {
			initResX = RESX_Maximum;
		}
		initResY = calcMinMaxRatio(initResY,RESY_Minimum,RESY_Default,0.6);
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

	Render_Config.reset_Render_Configurator(
		/* Float16 */ false,
		/* Float32 */ true,
		/* Float64 */ false
	);
	Render_Config.suggest_Render_Preset(Rendering_Configuration::Render_Preset_GPU_Float32);

	//SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printFatalError("SDL_Init(SDL_INIT_EVERYTHING) failed to initialize");
		return -1;
	}
	printf("\nSystem Information:");
	
	dim32_t dispResX, dispResY;
	dim32_t initResX, initResY, initPosX, initPosY;
	int32_t initDisplayIndex = loadDisplayInformation(Display_Preferences, initResX, initResY, initPosX, initPosY);
	if (initDisplayIndex == 0) {
		printCriticalError("init_Render failed to loadDisplayInformation");
	}
	dispResX = initResX;
	dispResY = initResY;
	//printFlush("\nOld: %" PRId32 "x%" PRId32 " %" PRId32 ",%" PRId32,initResX,initResY,initPosX,initPosY);
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
	//printFlush("\nNew: %" PRId32 "x%" PRId32 " %" PRId32 ",%" PRId32,initResX,initResY,initPosX,initPosY);
	printf("\n\tOperating System: %s", SDL_GetPlatform());
	printf("\n\tSystem RAM: %" PRId32 "MB", SDL_GetSystemRAM());
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
	// printf("\nInit_Render: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	init_KeyBind_PresetList();
	initKeys();
	//cleanKeyBind(&currentKeyBind);

	config_data.Display_Preferences.Display_Config_Hash = getDisplayConfigHash();
	config_data.Rendering_Settings.Hardware_Hash = get_Hardware_Hash();

	bootup_Fractal_Frame_Rendered = false;
	
	write_Render_Ready(true);
	write_Parameters(&current_Fractal, &primaryRenderData, &secondaryRenderData);
	printFlush("\n");

	while (read_Engine_Ready() == false) {
		if (QUIT_FLAG == true) {
			printWarning("Render thread exiting initialization: QUIT_FLAG == true");
			return -1;
		}
		std::this_thread::yield();
	}
	start_Render(QUIT_FLAG,ABORT_RENDERING);
	return 0;
}

uint64_t get_Hardware_Hash() {
	uint64_t hardwareHash = 0x0;
	int8_t value8 = 0x0; int32_t value32 = 0x0;
	value8 = PROGRAM_V_MAJOR;
	fnv1a_hash_continous(hardwareHash,(uint8_t*)(void*)&value8,sizeof(int32_t));
	value32 = (int32_t)std::thread::hardware_concurrency();
	fnv1a_hash_continous(hardwareHash,(uint8_t*)(void*)&value32,sizeof(int32_t));
	value32 = SDL_GetCPUCacheLineSize();
	fnv1a_hash_continous(hardwareHash,(uint8_t*)(void*)&value32,sizeof(int32_t));
	value32 = SDL_GetSystemRAM();
	fnv1a_hash_continous(hardwareHash,(uint8_t*)(void*)&value32,sizeof(int32_t));
	// #ifdef Enable_OpenCL
	// 	get_GPU_Hardware_Hash(hardwareHash);
	// #endif
	return hardwareHash;
}

int terminate_Render() {
	terminate_config_data();
	terminateKeyboardGraphics();
	clear_KeyBind_PresetList();
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
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
			for (uint32_t p = 0; p < patternLength; p++) {
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				pattern[z] = (uint8_t)((w + p) % 256) / 4; pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				if (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
			}
			break;
		case Status_Graphic::Graphic_Pause:
			for (uint32_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				if (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
			}
			break;
		case Status_Graphic::Graphic_Loading:
		default:
			for (uint32_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = 0; z++;
				pattern[z] = (uint8_t)((w + p) % 256); pattern[z] /= color_square_divider; z++;
				if (IMAGE_BUFFER_CHANNELS == 4) { pattern[z] = 0xFF; z++; }
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
		printFlush("\nTaking Screenshot");
	}
	return 0;
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

void renderJuliaCordinatePoint(const BufferBox& box, const Render_Data* ren) {
	const User_Rendering_Settings& Rendering_Settings = config_data.Rendering_Settings;
	if (Rendering_Settings.JuliaPoint_Enabled == false) { return; }
	if (current_Fractal.zr == 0.0 && current_Fractal.zi == 0.0) { return; }
	if (ren == nullptr) {
		printError("Unable to renderJuliaCordinatePoint(), Render_Data is nullptr");
		return;
	}
	if (validateBufferBox(&box,true) == false) {
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
		&posX, &posY, &current_Fractal, ren
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

/* Legacy Frame Transformation */
int displayFracImage(ImageBuffer* image, Render_Data* ren) {
	if (image == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	if (image->vram == nullptr) { printError("ImageBuffer* image->vram is NULL"); return -1; }
	if (image->allocated() == false) { printError("ImageBuffer* image is not allocated"); return -1; }
	if (ren == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	ABS_Mandelbrot& FRAC = current_Fractal;
	constexpr dim32_t MinimumImageResolution = 2;
	if (image->resX < MinimumImageResolution || image->resY < MinimumImageResolution) {
		printWarning("ImageBuffer* image is below minimum resolution: %" PRIu32 "x%" PRIu32,image->resX,image->resY);
		return 1;
	}
	int32_t fx0 = 0; int32_t fy0 = 0;
	int32_t fx1 = 0; int32_t fy1 = 0;
	coordinate_to_pixel(image->x00 - FRAC.r,image->y00 - FRAC.i,&fx0,&fy0,&FRAC,ren);
	coordinate_to_pixel(image->x11 - FRAC.r,image->y11 - FRAC.i,&fx1,&fy1,&FRAC,ren);
	if (fx0 > fx1) { int32_t temp = fx0; fx0 = fx1; fx1 = temp; }
	if (fy0 > fy1) { int32_t temp = fy0; fy0 = fy1; fy1 = temp; }
	// int32_t fxCenter = (fx0 + fx1) / 2;
	// int32_t fyCenter = (fy0 + fy1) / 2;
	if ((fx1 < MinimumImageResolution || fy1 < MinimumImageResolution)) {
		return 1;
	}
	if ((image->rot != FRAC.rot) || ((fx0 < Master.resX) && (fy0 < (Master.resY - RESY_UI)))) {
		scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(
			image->vram,
			image->resX, image->resY,
			(int32_t)(image->channels * 8),
			(int32_t)(image->channels * (size_t)image->resX),
			SDL_PIXELFORMAT_ABGR8888
		);
		fx1 -= fx0;
		fy1 -= fy0;
		SDL_Rect srcRect = {0, 0, image->resX, image->resY};
		SDL_Rect dstRect = {fx0, fy0 + RESY_UI, fx1, fy1};
		scale_tex = SDL_CreateTextureFromSurface(renderer, scale_surface);
		if (SDL_RenderCopy(renderer, scale_tex, &srcRect, &dstRect)) {
			printf("\nrenderCopy: %s",SDL_GetError()); fflush(stdout);
		}
		SDL_DestroyTexture(scale_tex);
		SDL_FreeSurface(scale_surface);
	}
	return 0;
}

/* OpenCV Frame Transformation */
int transformFracImage(ImageBuffer* image, const Render_Data* ren) {
	if (image == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	if (image->vram == nullptr) { printError("ImageBuffer* image->vram is NULL"); return -1; }
	if (image->allocated() == false) { printError("ImageBuffer* image is not allocated"); return -1; }
	if (ren == NULL) { printError("ImageBuffer* image is NULL"); return -1; }
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
	coordinate_to_image_cordinate(image->x00 - FRAC.r,image->y00 - FRAC.i,&dx00,&dy00,&FRAC,ren);
	coordinate_to_image_cordinate(image->x11 - FRAC.r,image->y11 - FRAC.i,&dx11,&dy11,&FRAC,ren);
	coordinate_to_image_cordinate(image->x01 - FRAC.r,image->y01 - FRAC.i,&dx01,&dy01,&FRAC,ren);
	coordinate_to_image_cordinate(image->x10 - FRAC.r,image->y10 - FRAC.i,&dx10,&dy10,&FRAC,ren);
	dim32_t resX = (dim32_t)(image->resX);
	dim32_t resY = (dim32_t)(image->resY);
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
		&blit, image, ren,
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
	renderJuliaCordinatePoint(blit, ren);
	copyBuffer_VeritcalOffset(temp_MASTER, blit, (size_t)RESY_UI);
	FREE(blit.vram);
	
	// if (
	// 	Image_Scaler_Quadrilateral(
	// 		&blit, image, ren,
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

void newFrame() {
	if (Master.bufferSafe() == false) {
		printError("Master ImageBuffer is invalid");
		return;
	}
	{
		const ABS_Mandelbrot& FRAC = current_Fractal;
		Master.clearBuffer(
			(uint8_t)(FRAC.exterior_R_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_R_Phase))),
			(uint8_t)(FRAC.exterior_G_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_G_Phase))),
			(uint8_t)(FRAC.exterior_B_Amp * (127.5 - 127.5 * cos(TAU * FRAC.exterior_B_Phase)))
		);
		//Master.clearBuffer();
	}

	if (exportSuperFractalBuffer == true) {
		exportSuperFractalBuffer = false;
	}

	int primaryBufferChange = next_Read_Cycle_Pos(&Primary_Image,Primary_Full);
	bool primaryBufferValid = (primaryBufferChange < 0 || Primary_Image == nullptr || Primary_Image->bufferSafe() == false) ? false : true;
	
	/* Temporary code that will force the rendering Engine to Update at bootup */
	if (primaryBufferValid == true) {
		bootup_Fractal_Frame_Rendered = true;
	} else if (bootup_Fractal_Frame_Rendered == false) {
		write_Update_Level(Change_Level::Full_Reset);
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
			int scaleRet = transformFracImage(Primary_Image,&primaryRenderData);
			if (scaleRet == -2) { // Scaled Image is too small
				BufferBox render_Area; getRenderBufferBoxFromMaster(render_Area);
				renderStatusGraphic(render_Area, Status_Graphic::Graphic_Loading, 1.0);
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
			int dispRet = displayFracImage(Primary_Image,&primaryRenderData);
			printfChange(int,dispRet,"\ndisplayFracImage: %" PRId32,dispRet);
		#endif
		if (exportFractalBuffer == true) {
			nano64_t curTime = getNanoTime();
			const char* fractal_name = (current_Fractal.polarMandelbrot == true) ? FractalTypeFileText[Fractal_Polar_Mandelbrot] : FractalTypeFileText[Fractal_ABS_Mandelbrot];
			size_t size = (size_t)snprintf(nullptr, 0, "%s_%" PRIu64, fractal_name, curTime);
			char* name = (char*)calloc(size + 1,sizeof(char));
			snprintf(name, size, "%s_%" PRIu64, fractal_name, curTime);
			char path[] = "./";
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
}