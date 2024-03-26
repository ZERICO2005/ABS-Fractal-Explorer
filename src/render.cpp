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
#include "fracExpKB.h"
#include "fileManager.h"
#include "imageBuffer.h"
#include "imageTransform.h"

#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "programData.h"
#include "user_data.h"

#include "display_GUI.h"
#include "displayInfo.h"





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
	rDat->rendering_method = Rendering_Method::GPU_Rendering;
	rDat->CPU_Precision = 64;
	if ((uint32_t)std::thread::hardware_concurrency() <= 1) {
		rDat->CPU_Threads = 1;
	} else {
		rDat->CPU_Threads = (uint32_t)std::thread::hardware_concurrency() - 1;
	}
	rDat->GPU_Precision = 32;
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
	int get_currentKBPreset_Pos() {
		std::size_t index = 0;
		for (const auto& element : KeyBind_PresetList) {
			if (&element == currentKBPreset) {
				return index;
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

bool keyPressed(uint32_t key) {
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

// // Amount of displays detected
// uint32_t DISPLAY_COUNT = 0;
// /* Display Bootup */
// 	namespace Display_Bootup_Legacy {
// 		enum Display_Bootup_Enum_Legacy {
// 			Automatic,First,Last,Specific,Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,HighResolution,HighFrameRate,LowResolution,LowFrameRate,Length
// 		};
// 	};
// 	uint32_t SPECIFIC_BOOTUP_DISPLAY = 1; // Supposed to be save data
// 	uint32_t Display_Match[Display_Bootup_Legacy::Length];
// 	Display_Bootup_Legacy::Display_Bootup_Enum_Legacy Display_Bootup_Type = Display_Bootup_Legacy::Automatic;
// 	bool useDefaultWindowSize = false;

// struct _DisplayInfo_Legacy {
// 	uint32_t resX;
// 	uint32_t resY;
// 	int32_t posX;
// 	int32_t posY;
// 	uint32_t refreshRate;
// 	uint8_t bbp;
// 	const char* name;
// }; typedef struct _DisplayInfo_Legacy DisplayInfo_Legacy;
// DisplayInfo_Legacy* DisplayList;

// // Counts from ONE
// uint32_t CURRENT_DISPLAY = 1;
// Counts from ONE
// DisplayInfo_Legacy* getDisplayInfo(size_t i) { // size_t i = 1
// 	if (i == 0 || i > (size_t)getDisplayCount() || DisplayList == NULL) {
// 		return NULL;
// 	}
// 	return &DisplayList[i - 1];
// }
// Counts from ONE
// DisplayInfo_Legacy* getCurrentDisplayInfo() {
// 	return getDisplayInfo((size_t)CURRENT_DISPLAY);
// }

// static const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

// #ifndef BUILD_RELEASE
// 	const char* buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
// #else
// 	const char* buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds"};
// #endif

bool windowResizingCode(uint32_t* resX = NULL, uint32_t* resY = NULL) {
	bool reVal = false;
	int32_t x = 0, y = 0;
	static int32_t rX = 0, rY = 0;
	SDL_GetWindowSize(window,&x,&y);
	if ((rX != x || rY != y) && (rX != 0 && rY != 0)) {
		if (x < RESX_Minimum) { x = RESX_Minimum; }
		if (y < RESY_Minimum) { y = RESY_Minimum; }
		if (x > RESX_Maximum) { x = RESX_Maximum; }
		if (y > RESY_Maximum) { y = RESY_Maximum; }
		//if (x & 0x3) { x &= 0xFFFFFFFC; } // Sets resX to a multiple of 4 so I don't have to deal with padded and unpadded image buffers
		
		SDL_SetWindowSize(window,x,y);
		SDL_RenderSetLogicalSize(renderer, x, y);
		Master.resX = x;
		Master.resY = y;
		TestGraphic.resX = x;
		TestGraphic.resY = y - RESY_UI;
		// printFlush("\n%d %d | %llu",x,y,getBufferBoxSize(&TestGraphic));
		TestGraphic.vram = (uint8_t*)realloc((void*)(TestGraphic.vram),getBufferBoxSize(&TestGraphic));
		if (resX != NULL) { *resX = x; }
		if (resY != NULL) { *resY = y; }
		updateRenderData(&primaryRenderData);
		updateRenderData(&secondaryRenderData);
		write_Update_Level(Change_Level::Full_Reset);
		Master.resizeBuffer(x,y,IMAGE_BUFFER_CHANNELS);
		if (texture != NULL) {
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);

		write_Buffer_Size({nullptr,Master.resX,Master.resY - RESY_UI,IMAGE_BUFFER_CHANNELS,0});
		reVal = true;
	}
	rX = x;
	rY = y;
	return reVal;
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

int utitledFileNameGenerator(char* buf, size_t maxLen) {
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
	uint32_t choice_front = rand() % ARRAY_LENGTH(UntitledFile_Front);
	uint32_t choice_middle = rand() % ARRAY_LENGTH(UntitledFile_Middle);
	uint32_t choice_end = rand() % ARRAY_LENGTH(UntitledFile_End);
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

void moveCord(fp128* x, fp128* y, fp64 angle, fp64 speed) {
	*x += speed * cos(angle);
	*y += speed * sin(angle);
}

void moveCord(fp64* x, fp64* y, fp64 angle, fp64 speed) {
	*x += speed * cos(angle);
	*y += speed * sin(angle);
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
	if (ren->rendering_method != ren0.rendering_method) {
		Update_Level(update_level, Change_Level::Method_of_Rendering);
	} else if (
		(
			(ren->rendering_method == Rendering_Method::CPU_Rendering) &&
			((ren->CPU_Precision != ren0.CPU_Precision) || (ren->CPU_Threads != ren0.CPU_Threads))
		) && (
			(ren->rendering_method == Rendering_Method::GPU_Rendering) &&
			(ren->GPU_Precision != ren0.GPU_Precision)
		)
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
		(frac_data->rA != frac0.rA || frac_data->rF != frac0.rF || frac_data->rP != frac0.rP) ||
		(frac_data->gA != frac0.gA || frac_data->gF != frac0.gF || frac_data->gP != frac0.gP) ||
		(frac_data->bA != frac0.bA || frac_data->bF != frac0.bF || frac_data->bP != frac0.bP) ||
		(frac_data->iA != frac0.iA || frac_data->iF != frac0.iF || frac_data->iP != frac0.iP)
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

int updateFractalParameters() {
	using namespace Key_Function;
	using namespace Change_Level;
	#define FRAC frac.type.abs_mandelbrot
	fp64 temp_breakoutValue = log2(FRAC.breakoutValue);
	fp64 moveDelta = (DeltaTime < 0.2) ? DeltaTime : 0.2;
	
	const User_Parameter_Sensitivity& config_sensitivity = config_data.Parameter_Sensitivity;

	moveDelta *= config_sensitivity.global;

	int update_level = Change_Level::Nothing;

	/* Magic Constants */
		#define ABS_Mandelbrot_Default_Power 2
		#define Polar_Mandelbrot_Default_Power 3.0
	/* Boolean toggles */
		#define paramToggle(func,toggle,freq) if (funcTimeDelay(func,freq)) { toggle = !toggle; }
		#define paramToggleUpdate(func,toggle,freq,level) if (funcTimeDelay(func,freq)) { toggle = !toggle; Update_Level(update_level, level); }
		paramToggle(toggleAdjustZoomToPower,FRAC.adjustZoomToPower,0.4);
		paramToggleUpdate(toggleJulia,FRAC.juliaSet,0.4,Major_Reset);
		paramToggleUpdate(toggleABSandPolarMandelbrot,FRAC.polarMandelbrot,0.4,Major_Reset);
		paramToggle(toggleRelativeZValue,FRAC.relativeZValue,0.4);
		paramToggle(toggleCursorZValue,FRAC.cursorZValue,0.4);
		paramToggleUpdate(toggleStartingZ,FRAC.startingZ,0.4,Minor_Reset);
		paramToggleUpdate(toggleIntegerPower,FRAC.integerPolarPower,0.4,Minor_Reset);
		#undef paramToggle
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
		if (funcTimeDelay(resetRealPos,0.2)) {
			FRAC.r = 0.0;
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetImagPos,0.2)) {
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
		if (funcTimeDelay(resetZReal,0.2)) {
			FRAC.zr = 0.0;
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetZImag,0.2)) {
			FRAC.zi = 0.0;
			Update_Level(update_level, Jump);
		}
		if (FRAC.cursorZValue == true) {
			if (FRAC.relativeZValue == true) {
				fp64 resZ = (fp64)((Master.resX > Master.resY) ? Master.resY : Master.resX);
				FRAC.zr = 4.0 * ((fp64)ImGui::GetMousePos().x - ((fp64)Master.resX / 2.0)) / resZ;
				FRAC.zi = 4.0 * ((fp64)(ImGui::GetMousePos().y - RESY_UI) - ((fp64)Master.resY / 2.0)) / resZ;
			} else {
				pixel_to_coordinate(
					(int32_t)(ImGui::GetMousePos().x),(int32_t)(ImGui::GetMousePos().y - RESY_UI),
					&FRAC.zr,&FRAC.zi,&FRAC,&primaryRenderData
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
		if (funcTimeDelay(resetZoom,0.2)) {
			FRAC.zoom = zoomDefault(FRAC.power);
			Update_Level(update_level, Jump);
		}
		if (funcTimeDelay(resetCoordinates,0.2)) {
			FRAC.r = 0.0; FRAC.i = 0.0;
			FRAC.stretch = 0.0; FRAC.rot = 0.0;
			if (FRAC.polarMandelbrot == true) {
				FRAC.zoom = zoomDefault(FRAC.polarPower);
			} else {
				FRAC.zoom = zoomDefault((fp64)FRAC.power);
			}
			valueLimit(FRAC.zoom,-0.4,0.4);
			Update_Level(update_level, Jump);
		}
	/* maxItr */
		if (func_stat[incMaxItr].triggered) {
			FRAC.maxItr_Log2 += 2.0 * moveDelta * config_sensitivity.maxIter;
			setMaxItr(&FRAC,FRAC.maxItr_Log2);
		}
		if (func_stat[decMaxItr].triggered) {
			FRAC.maxItr_Log2 -= 2.0 * moveDelta * config_sensitivity.maxIter;
			setMaxItr(&FRAC,FRAC.maxItr_Log2);
		}
		if (funcTimeDelay(resetMaxItr,0.2)) {
			FRAC.maxItr_Log2 = log2(192.0);
			FRAC.maxItr = 192;
			setMaxItr(&FRAC,FRAC.maxItr_Log2);
		}
	/* Formula*/
		if (funcTimeDelay(incFormula,1.0/10.0)) {
			FRAC.formula++;
		}
		if (funcTimeDelay(decFormula,1.0/10.0)) {
			FRAC.formula--;
		}
		if (funcTimeDelay(incFamily,1.0/10.0)) {
			FRAC.formula += getABSValue(FRAC.power);
		}
		if (funcTimeDelay(decFamily,1.0/10.0)) {
			FRAC.formula -= getABSValue(FRAC.power);
		}
		if (funcTimeDelay(resetFormula,0.2)) {
			FRAC.formula = 0;
		}
	/* Power */
		if (FRAC.polarMandelbrot == true) {
			if (FRAC.integerPolarPower == true) {
				if (funcTimeDelay(incPower,1.0/6.0)) {
					FRAC.polarPower++;
				}
				if (funcTimeDelay(decPower,1.0/6.0)) {
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
			
			if (funcTimeDelay(resetPower,0.2)) {
				FRAC.polarPower = Polar_Mandelbrot_Default_Power;
			}
			if (funcTimeDelay(roundPower,0.2)) {
				FRAC.polarPower = round(FRAC.polarPower);
			}
			if (funcTimeDelay(floorPower,0.2)) {
				FRAC.polarPower = floor(FRAC.polarPower);
			}
			if (funcTimeDelay(ceilingPower,0.2)) {
				FRAC.polarPower = ceil(FRAC.polarPower);
			}
		} else {
			if (funcTimeDelay(incPower,1.0/6.0)) {
				FRAC.power++;
			}
			if (funcTimeDelay(decPower,1.0/6.0)) {
				FRAC.power--;
			}
			if (funcTimeDelay(resetPower,0.2)) {
				FRAC.power = ABS_Mandelbrot_Default_Power;
			}
			FRAC.formula = limitFormulaID(FRAC.power,FRAC.formula);
		}
	/* Rotations */
		if (func_stat[counterclockwiseRot].triggered) {
			FRAC.rot -= (TAU/3.0) * moveDelta * getStretchValue(FRAC.stretch) * config_sensitivity.rotation;
		}
		if (func_stat[clockwiseRot].triggered) {
			FRAC.rot += (TAU/3.0) * moveDelta * getStretchValue(FRAC.stretch) * config_sensitivity.rotation;
		}
		if (funcTimeDelay(clockwiseRot90,0.3)) {
			FRAC.rot += (TAU * (90.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRot90,0.3)) {
			FRAC.rot -= (TAU * (90.0/360.0));
		}
		if (funcTimeDelay(rotate180,0.3)) {
			FRAC.rot += (TAU * (180.0/360.0));
		}
		if (funcTimeDelay(clockwiseRotStep,1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRotStep,1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(clockwiseRotPower,1.0/6.0)) {
			FRAC.rot += (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
		}
		if (funcTimeDelay(counterclockwiseRotPower,1.0/6.0)) {
			FRAC.rot -= (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
		}
		if (funcTimeDelay(resetRotation,0.2)) {
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
		if (funcTimeDelay(resetStretch,0.2)) {
			FRAC.stretch = 0.0;
		}
		if (funcTimeDelay(resetTransformations,0.2)) {
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
		if (funcTimeDelay(resetBreakout,0.2)) {
			temp_breakoutValue = log2(16777216.0);
		}
	/* Rendering */
		if (funcTimeDelay(incSubSample,1.0/6.0)) {
			primaryRenderData.subSample++;
		}
		if (funcTimeDelay(decSubSample,1.0/6.0)) {
			primaryRenderData.subSample--;
		}
		if (funcTimeDelay(resetSubSample,0.2)) {
			primaryRenderData.subSample = 1;
		}
		valueLimit(primaryRenderData.subSample,1,24);
		if (funcTimeDelay(incSuperSample,1.0/6.0)) {
			primaryRenderData.sample++;
		}
		if (funcTimeDelay(decSuperSample,1.0/6.0)) {
			primaryRenderData.sample--;
		}
		if (funcTimeDelay(resetSuperSample,0.2)) {
			primaryRenderData.sample = 1;
		}
		valueLimit(primaryRenderData.sample,1,24);
	/* Rendering Method */
	{
		using namespace Rendering_Method;
		if (funcTimeDelay(fp32CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 32;
		}
		if (funcTimeDelay(fp64CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 64;
		}
		if (funcTimeDelay(fp80CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 80;
		}
		if (funcTimeDelay(fp128CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 128;
		}
		if (funcTimeDelay(fp16GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 16;
		}
		if (funcTimeDelay(fp32GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 32;
		}
		if (funcTimeDelay(fp64GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 64;
		}
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
		if (funcTimeDelay(takeScreenshot,0.4)) {
			exportScreenshot();
		}
		if (funcTimeDelay(takeSuperScreenshot,0.4)) {
			exportSuperScreenshot();
		}
	#undef FRAC
	write_Update_Level(update_level);
	return update_level;
}

// #define BufAndLen(x) x,ARRAY_LENGTH(x)
// /* Sets defualt window size and position along with size constraints */
// #define ImGui_DefaultWindowSize(valX,bufX,minX,maxX,ratioX,valY,bufY,minY,maxY,ratioY); \
// 	uint32_t WINDOW_RESX = calcMinMaxRatio(valX-bufX,minX,maxX,ratioX); \
// 	uint32_t WINDOW_RESY = calcMinMaxRatio(valY-bufY,minY,maxY,ratioY); \
// 	ImGui::SetNextWindowPos({(fp32)((valX - WINDOW_RESX) / 2),(fp32)((valY - WINDOW_RESY) / 2)}, ImGuiCond_Once); \
// 	ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY}, ImGuiCond_Once); \
// 	if (AutoResizeWindows == true) { \
// 		ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY}); \
// 	} \
// 	ImGui::SetNextWindowSizeConstraints({(fp32)minX,(fp32)minY},{(fp32)valX - bufX,(fp32)valY - bufY}); \
// 	WINDOW_RESX = (WINDOW_RESX > valX - bufX) ? (valX - bufX) : WINDOW_RESX; \
// 	WINDOW_RESY = (WINDOW_RESY > valY - bufY) ? (valY - bufY) : WINDOW_RESY; \
// 	ImGui::SetNextWindowBgAlpha(WindowOpacity);

// #define ImGui_BoundWindowPosition(); \
// 	if (PreventOutOfBoundsWindows == true) { \
// 		int32_t WINDOW_POSX = ImGui::GetWindowPos().x; \
// 		int32_t WINDOW_POSY = ImGui::GetWindowPos().y; \
// 		valueLimit(WINDOW_POSX,ImGui_WINDOW_MARGIN,(int32_t)Master.resX - (int32_t)ImGui::GetWindowSize().x - ImGui_WINDOW_MARGIN); \
// 		valueLimit(WINDOW_POSY,ImGui_WINDOW_MARGIN,(int32_t)Master.resY - (int32_t)ImGui::GetWindowSize().y - ImGui_WINDOW_MARGIN); \
// 		ImGui::SetWindowPos({(fp32)(WINDOW_POSX),(fp32)(WINDOW_POSY)}); \
// 	}


// void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
//     ImGui::Begin("Horizontal Button Page", NULL, window_flags);

// 	static ImVec4 GUI_FrameRateColor;
// 	static ImVec4 Render_FrameRateColor;
// 	fp64 Frame_FPS_Display = 1.0 / Frame_Time_Display;
// 	fp64 Render_FPS_Display = 1.0 / Render_Time_Display;
// 	GUI_FrameRateColor = {
// 		(fp32)linearInterpolationClamp(Frame_FPS_Display,59.0,119.0,1.0,0.0),
// 		(fp32)linearInterpolationClamp(Frame_FPS_Display,0.0,29.0,0.0,1.0),
// 		(fp32)linearInterpolationClamp(Frame_FPS_Display,29.0,59.0,0.0,1.0),
// 		1.0
// 	};
// 	Render_FrameRateColor = {
// 		(fp32)linearInterpolationClamp(Render_FPS_Display,59.0,119.0,1.0,0.0),
// 		(fp32)linearInterpolationClamp(Render_FPS_Display,0.0,29.0,0.0,1.0),
// 		(fp32)linearInterpolationClamp(Render_FPS_Display,29.0,59.0,0.0,1.0),
// 		1.0
// 	};
	
// 	ImGui::Text("GUI:"); ImGui::SameLine();
// 	ImGui::TextColored(GUI_FrameRateColor,"%.2lf",Frame_FPS_Display); ImGui::SameLine(0.0,1.0);
// 	ImGui::Text("FPS"); ImGui::SameLine();
// 	ImGui::TextColored(GUI_FrameRateColor,"%.2lf",Frame_Time_Display * 1000.0); ImGui::SameLine(0.0,1.0);
// 	ImGui::Text("ms");
// 	ImGui::SameLine();
// 	ImGui::Text("Render:"); ImGui::SameLine();
// 	ImGui::TextColored(Render_FrameRateColor,"%.2lf",Render_FPS_Display); ImGui::SameLine(0.0,1.0);
// 	ImGui::Text("FPS"); ImGui::SameLine();
// 	ImGui::TextColored(Render_FrameRateColor,"%.2lf",Render_Time_Display * 1000.0); ImGui::SameLine(0.0,1.0);
// 	ImGui::Text("ms");

//     size_t buttonCount = sizeof(buttonLabels) / sizeof(buttonLabels[0]);
// 	if (ImGui::Button("Coordinates")) {
// 		buttonSelection = (buttonSelection == GUI_Menu_Coordinates) ? -1 : GUI_Menu_Coordinates;
// 	} ImGui::SameLine();
// 	if (ImGui::Button("Fractal")) {
// 		buttonSelection = (buttonSelection == GUI_Menu_Fractal) ? -1 : GUI_Menu_Fractal;
// 	} ImGui::SameLine();
// 	#ifndef BUILD_RELEASE
// 		if (ImGui::Button("Export")) {

// 		} ImGui::SameLine();
// 		if (ImGui::Button("Import")) {
// 			buttonSelection = (buttonSelection == GUI_Menu_Import) ? -1 : GUI_Menu_Import;
// 		} ImGui::SameLine();
// 	#endif
// 	if (ImGui::Button("Screenshot")) {
// 		exportScreenshot();
// 	} ImGui::SameLine();
// 	if (ImGui::Button("Rendering")) {
// 		buttonSelection = (buttonSelection == GUI_Menu_Rendering) ? -1 : GUI_Menu_Rendering;
// 	} ImGui::SameLine();
// 	if (ImGui::Button("Settings")) {
// 		buttonSelection = (buttonSelection == GUI_Menu_Settings) ? -1 : GUI_Menu_Settings;
// 	} ImGui::SameLine();
// 	if (ImGui::Button("Key-binds")) {
// 		buttonSelection = (buttonSelection == GUI_Menu_KeyBinds) ? -1 : GUI_Menu_KeyBinds;
// 	} ImGui::SameLine();
// 	if (Waiting_To_Abort_Rendering == true) {
// 		ImGui::Text("Aborting...(%.1lfs)",NANO_TO_SECONDS(getNanoTime() - abortTimer));
// 	} else {
// 		if (Abort_Rendering_Flag == true) {
// 			if (ImGui::Button("Resume Rendering")) {
// 				Abort_Rendering_Flag = false;
// 			}
// 		} else {
// 			if (ImGui::Button("Abort Rendering")) {
// 				Abort_Rendering_Flag = true;
// 				Waiting_To_Abort_Rendering = true;
// 				write_Abort_Render_Ongoing(true);
// 				abortTimer = getNanoTime();
// 			}
// 		}
// 	}
// 	if (Lock_Key_Inputs == true) {
// 		ImGui::SameLine();
// 		ImGui::Text("Key inputs are locked inside of menus");
// 	}

// 	ImGui::Separator();
// 	uint32_t boxSpace = 8;
// 	uint32_t boxCount = 8;
// 	fp32 boxWidth = (fp32)(Master.resX - (boxSpace * (boxCount + 1))) / (fp32)boxCount;

// 	#define Param_Input_Box(text,id,buf) \
// 	ImGui::Text(text); \
// 	ImGui::SetNextItemWidth(boxWidth); \
// 	ImGui::SameLine(); \
// 	ImGui::InputText(id,buf,sizeof(buf)); \
// 	correctTextFloat(buf,sizeof(buf),0);
	
// 	#define Param_Input_Double(text,id,ptr,fm) \
// 	ImGui::Text(text); \
// 	ImGui::SetNextItemWidth(boxWidth); \
// 	ImGui::SameLine(); \
// 	ImGui::InputDouble(id,ptr,0.0,0.0,fm);

// 	#define Param_Input_Int(text,id,ptr) \
// 	ImGui::Text(text); \
// 	ImGui::SetNextItemWidth(boxWidth); \
// 	ImGui::SameLine(); \
// 	ImGui::InputInt(id,ptr,16,256);

// 	#define FRAC frac.type.abs_mandelbrot
// 	uint32_t renderFP = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? primaryRenderData.CPU_Precision : primaryRenderData.GPU_Precision;
// 	const char* const renderMethod = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? "CPU" : "GPU";
// 	ImGui::Text(
// 		"Formula: %llu Power: %6.4lf Super-Sample: %u Rendering: %s fp%u",
// 		FRAC.formula,(FRAC.polarMandelbrot ? FRAC.polarPower : (fp64)FRAC.power),primaryRenderData.sample * primaryRenderData.sample,renderMethod,renderFP
// 	);
// 	#define temp_quad_len 64
// 	static char temp_quad_r[temp_quad_len]; static char temp_quad_i[temp_quad_len];
// 	static char temp_quad_zr[temp_quad_len]; static char temp_quad_zi[temp_quad_len];
// 	quadmath_snprintf(temp_quad_r,temp_quad_len,"%15.12Qf",FRAC.r);
// 	quadmath_snprintf(temp_quad_i,temp_quad_len,"%15.12Qf",FRAC.i);
// 	quadmath_snprintf(temp_quad_zr,temp_quad_len,"%15.12Qf",FRAC.zr);
// 	quadmath_snprintf(temp_quad_zi,temp_quad_len,"%15.12Qf",FRAC.zi);
// 	ImGui::Text(
// 		"Zreal: %s Zimag: %s Rotation: %5.1lf Stetch: 2^%6.4lf",
// 		temp_quad_zr,temp_quad_zi,FRAC.rot * 360.0 / TAU,FRAC.stretch
// 	);
// 	ImGui::Text(" ");
// 	fp64 adjustedZoomValue = FRAC.zoom;
// 	if (FRAC.adjustZoomToPower == true) {
// 		if (FRAC.polarMandelbrot == true) {
// 			adjustedZoomValue += log10(getABSFractalMaxRadius(FRAC.polarPower));
// 		} else {
// 			adjustedZoomValue += log10(getABSFractalMaxRadius((fp64)FRAC.power));
// 		}
// 		valueLimit(adjustedZoomValue,FRAC.zoom - 0.4, FRAC.zoom + 0.4);
// 	}
	
// 	ImGui::Text(
// 		"Real:  %s Imag:  %s Zoom: 10^%6.4lf Itr: %u",
// 		temp_quad_r,temp_quad_i,adjustedZoomValue,FRAC.maxItr
// 	);
// 	#undef FRAC
//     // End the ImGui window
//     ImGui::End();
// }

// int render_IMGUI() {
// 	if (window == nullptr) {
// 		printError("render_IMGUI(): window == nullptr");
// 	}
// 	ImGui_ImplSDLRenderer2_NewFrame();
// 	ImGui_ImplSDL2_NewFrame(window);
// 	ImGui::NewFrame();
// 	ImGui::SetNextWindowPos({0,0});
// 	ImGui::SetNextWindowSize({(fp32)Master.resX,(fp32)RESY_UI});
// 	ImGuiWindowFlags window_flags = 0;
// 	window_flags |= ImGuiWindowFlags_NoTitleBar;
// 	window_flags |= ImGuiWindowFlags_NoResize;
// 	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
// 	horizontal_buttons_IMGUI(window_flags);
	
// 	ImGui_WINDOW_FLAGS = 0;
// 	ImGui_WINDOW_FLAGS |= ImGuiWindowFlags_NoCollapse; 
// 	ImGui_WINDOW_FLAGS |= (AutoResizeWindows == true) ? ImGuiWindowFlags_NoResize : 0;

// 	if (buttonSelection != -1) {
// 		Lock_Key_Inputs = (LockKeyInputsInMenus == true) ? true : false;
// 		switch(buttonSelection) {
// 			case GUI_Menu_Coordinates:
// 				Menu_Coordinates();
// 			break;
// 			case GUI_Menu_Fractal:
// 				Menu_Fractal();
// 			break;
// 			case GUI_Menu_Import:
// 				//Menu_Import();
// 			break;
// 			case GUI_Menu_Rendering:
// 				Menu_Rendering();
// 			break;
// 			case GUI_Menu_Settings:
// 				Menu_Settings();
// 			break;
// 			case GUI_Menu_KeyBinds:
// 				Menu_Keybinds();
// 			break;
// 			default:
// 			Lock_Key_Inputs = false;
// 		}
// 	} else {
// 		Lock_Key_Inputs = false;
// 	}
// 	if (ShowTheXButton == false) {
// 		buttonSelection = -1;
// 		ShowTheXButton = true;
// 	}

// 	ImGui::Render();
// 	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	
// 	//SDL_RenderPresent(renderer); // Redundant
// 	return 0;
// }


void correctFrameTime() {
	valueLimit(GUI_FrameTimeNano,SECONDS_TO_NANO(1.0/1200.0),SECONDS_TO_NANO(1.0/6.0));
	valueLimit(GUI_FrameTime,1.0/1200.0,1.0/6.0);
	valueLimit(GUI_FrameRate,6.0,1200.0);
	GUI_FrameTimer.setFreq(GUI_FrameTime);
}

void updateFrameTimeNano(nano64_t frameTime) {
	GUI_FrameTimeNano = frameTime;
	GUI_FrameTime = NANO_TO_SECONDS(frameTime);
	GUI_FrameRate = 1.0 / NANO_TO_SECONDS(frameTime);
	correctFrameTime();
}

void updateFrameTime(fp64 frameTime) {
	GUI_FrameTimeNano = SECONDS_TO_NANO(frameTime);
	GUI_FrameTime = frameTime;
	GUI_FrameRate = 1.0 / frameTime;
	correctFrameTime();
}

void updateFrameRate(fp64 frameRate) {
	GUI_FrameTimeNano = SECONDS_TO_NANO(1.0 / frameRate);
	GUI_FrameTime = 1.0 / frameRate;
	GUI_FrameRate = frameRate;
	correctFrameTime();
}


int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	updateFrameRate(FRAME_RATE + FRAME_RATE_OFFSET);
	GUI_FrameTimer = TimerBox(GUI_FrameTime);

	TimerBox maxFrameReset = TimerBox(1.0/5.0); /* Keeps track of longest frame times */
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
			write_Parameters(&frac,&primaryRenderData,&secondaryRenderData);
			newFrame();
		}
	}
	return 0;
}

// int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
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
// 						printFlush("\n\nYeild Count: %llu Errors: %llu",yeildCount, yeildError);
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

// Automatic,First,Last,Specific,Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,HighResolution,HighFrameRate,LowResolution,LowFrameRate,Length

// int setupDisplayInfo(const User_Display_Preferences& display_config,int32_t* initResX, int32_t* initResY, int32_t* initPosX, int32_t* initPosY) {
// 	if (initResX == nullptr || initResY == nullptr || initPosX == nullptr || initPosY == nullptr) {
// 		printError("Unable to get Display Info, NULL parameters");
// 		return -1;
// 	}
// 	using namespace Display_Bootup_Legacy;
// 	DISPLAY_COUNT = SDL_GetNumVideoDisplays();
// 	printf("\n\tDisplay Count: %d",DISPLAY_COUNT);
// 	if (DISPLAY_COUNT == 0) {
// 		printError("No Displays Detected");
// 		return -1;
// 	}
// 	DisplayList = (DisplayInfo_Legacy*)calloc(DISPLAY_COUNT, sizeof(DisplayInfo_Legacy));
// 	if (DisplayList == nullptr) {
// 		FREE(DisplayList);
// 		printError("Unable to allocate memory for DisplayList");
// 		return -1;
// 	}
// 	for (size_t d = 0; d < Display_Bootup_Legacy::Length; d++) {
// 		Display_Match[d] = 1; // Set to first monitor
// 	}
// 	Display_Match[Display_Bootup_Legacy::Automatic] = 1;
// 	Display_Match[Display_Bootup_Legacy::First] = 1;
// 	Display_Match[Display_Bootup_Legacy::Last] = DISPLAY_COUNT;
// 	int Specific_Bootup_Display = display_config.Specific_Bootup_Display;
// 	Display_Match[Display_Bootup_Legacy::Specific] = valueLimit(Specific_Bootup_Display,1,(int32_t)DISPLAY_COUNT);
// 	#define Display(match) DisplayList[Display_Match[(match)] - 1]
// 	for (size_t i = 0; i < DISPLAY_COUNT; i++) {
// 		SDL_DisplayMode mode;
// 		SDL_Rect rect;
// 		SDL_GetDesktopDisplayMode(i,&mode);
// 		SDL_GetDisplayBounds(i, &rect);
// 		DisplayList[i].resX = mode.w;
// 		DisplayList[i].resY = mode.h;
// 		DisplayList[i].posX = rect.x;
// 		DisplayList[i].posY = rect.y;
// 		DisplayList[i].refreshRate = mode.refresh_rate;
// 		DisplayList[i].bbp = SDL_BITSPERPIXEL(mode.format);
// 		DisplayList[i].name = SDL_GetDisplayName(i);
// 		/* Orthagonal */
// 		if (DisplayList[i].posX < Display(Display_Bootup_Legacy::Left).posX) { Display_Match[Display_Bootup_Legacy::Left] = i + 1; }
// 		if (DisplayList[i].posX + DisplayList[i].resX > Display(Display_Bootup_Legacy::Right).posX + Display(Display_Bootup_Legacy::Right).resX) { Display_Match[Display_Bootup_Legacy::Right] = i + 1; }
// 		if (DisplayList[i].posY < Display(Display_Bootup_Legacy::Top).posY) { Display_Match[Display_Bootup_Legacy::Top] = i + 1; }
// 		if (DisplayList[i].posY + DisplayList[i].resY > Display(Display_Bootup_Legacy::Bottom).posY + Display(Display_Bootup_Legacy::Bottom).resY) { Display_Match[Display_Bootup_Legacy::Bottom] = i + 1; }
// 		/* Diagonal */
// 		/* Resolution and Refresh-Rate */
// 		if (DisplayList[i].resX * DisplayList[i].resY > Display(Display_Bootup_Legacy::HighResolution).resX * Display(Display_Bootup_Legacy::HighResolution).resY) { Display_Match[Display_Bootup_Legacy::HighResolution] = i + 1; }
// 		if (DisplayList[i].refreshRate > Display(Display_Bootup_Legacy::HighFrameRate).refreshRate) { Display_Match[Display_Bootup_Legacy::HighFrameRate] = i + 1; }
// 		if (DisplayList[i].resX * DisplayList[i].resY < Display(Display_Bootup_Legacy::LowResolution).resX * Display(Display_Bootup_Legacy::LowResolution).resY) { Display_Match[Display_Bootup_Legacy::LowResolution] = i + 1; }
// 		if (DisplayList[i].refreshRate < Display(Display_Bootup_Legacy::LowFrameRate).refreshRate) { Display_Match[Display_Bootup_Legacy::LowFrameRate] = i + 1; }
// 	}
// 	*initResX = Display(display_config.Display_Bootup_Type).resX;
// 	*initResY = Display(display_config.Display_Bootup_Type).resY;
// 	*initPosX = Display(display_config.Display_Bootup_Type).posX;
// 	*initPosY = Display(display_config.Display_Bootup_Type).posY;
// 	if (Display(display_config.Display_Bootup_Type).resX < RESX_Minimum || Display(display_config.Display_Bootup_Type).resY < RESY_Minimum) {
// 		if (DISPLAY_COUNT == 1) {
// 			printWarning("This display does not meet the minimum screen resolution");
// 			return 0;
// 		}
// 		for (size_t i = 0; i < DISPLAY_COUNT; i++) {
// 			if (Display(display_config.Display_Bootup_Type).resX >= RESX_Minimum && Display(display_config.Display_Bootup_Type).resY >= RESY_Minimum) {
// 				*initResX = DisplayList[i].resX;
// 				*initResY = DisplayList[i].resX;
// 				*initPosX = DisplayList[i].resX;
// 				*initPosY = DisplayList[display_config.Display_Bootup_Type].resX;
// 				printWarning("Display %u does not meet the minimum screen resolution, switching to display %zu",Display_Match[display_config.Display_Bootup_Type],i + 1);
// 				return 0;
// 			}
// 		}
// 		printWarning("None of the available displays support the minimum screen resolution");
// 		return 0;
// 	}
// 	fflush(stdout);
// 	return 0;
// }

// Returns the index of the display to be used. Returns 0 on failure
int32_t loadDisplayInformation(
	const User_Display_Preferences& display_config,
	int32_t& initResX, int32_t& initResY,
	int32_t& initPosX, int32_t& initPosY
) {
	int32_t displayCount = reloadDisplays();
	if (displayCount <= 0) {
		printError("Failed to detect displays");
		return 0;
	}
	printf("\n\tDisplay Count: %d",displayCount);
	int32_t cursorPosX, cursorPosY;
	SDL_GetMouseState(&cursorPosX, &cursorPosY);
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
	export_config_data(config_data,"./config.fracExpConfig");
	// char filePath[324]; memset(filePath,'\0',sizeof(filePath));
	// saveFileInterface(filePath,ARRAY_LENGTH(filePath));
	// export_config_data(config_data,"./config.fracExpConfig");
}

int init_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	init_config_data();
	//SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printFatalError("SDL_Init(SDL_INIT_EVERYTHING) failed to initialize");
		return -1;
	}
	printf("\nSystem Information:");
	int32_t dispResX, dispResY;
	int32_t initResX, initResY, initPosX, initPosY;
	// if (setupDisplayInfo(config_data.Display_Preferences,&initResX,&initResY,&initPosX,&initPosY) < 0) {
	// 	printCriticalError("init_Render failed to setupDisplayInfo");
	// }
	int32_t initDisplayIndex = loadDisplayInformation(config_data.Display_Preferences,initResX,initResY,initPosX,initPosY);
	if (initDisplayIndex == 0) {
		printCriticalError("init_Render failed to loadDisplayInformation");
	}
	//printFlush("\nOld: %dx%d %d,%d",initResX,initResY,initPosX,initPosY);
	dispResX = initResX;
	dispResY = initResY;
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
	//printFlush("\nNew: %dx%d %d,%d",initResX,initResY,initPosX,initPosY);
	{
		#ifndef MANUAL_FRAME_RATE_OVERRIDE
			const DisplayInfo* disp = getDisplayFromIndex(initDisplayIndex);
			if (disp != nullptr) {
				FRAME_RATE = disp->getRefreshRate() * Default_Frame_Rate_Multiplier;
			}
		#endif
		FRAME_RATE += FRAME_RATE_OFFSET;
		valueLimit(FRAME_RATE,6.0,1200.0);
	}
	printf("\n\tOperating System: %s",SDL_GetPlatform());
	printf("\n\tSystem RAM: %dMB",SDL_GetSystemRAM());
	// Allocate Buffers
	//initBufferBox(&Master,NULL,initResX,initResY,IMAGE_BUFFER_CHANNELS);
	Master = ImageBuffer(initResX,initResY,IMAGE_BUFFER_CHANNELS);
	initBufferBox(&TestGraphic,nullptr,Master.resX,Master.resY - RESY_UI,IMAGE_BUFFER_CHANNELS);
	TestGraphic.vram = (uint8_t*)malloc(getBufferBoxSize(&TestGraphic));
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
	write_Buffer_Size({nullptr,Master.resX,Master.resY - RESY_UI,IMAGE_BUFFER_CHANNELS,0});

	super_screenshot_maxThreads = std::thread::hardware_concurrency();
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

	setDefaultParameters(&frac,Fractal_ABS_Mandelbrot);
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
	printFlush("\n");
	write_Render_Ready(true);
	while (read_Engine_Ready() == false) {
		if (QUIT_FLAG == true) {
			printWarning("Render thread exiting initialization: QUIT_FLAG == true");
			return -1;
		}
		std::this_thread::yield();
	}
	start_Render(QUIT_FLAG,ABORT_RENDERING,Key_Function_Mutex);
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
	get_GPU_Hardware_Hash(hardwareHash);
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
	FREE(TestGraphic.vram);
	return 0;
}


void setRenderedBufferBox(BufferBox* box) {
	rendered_buf = box;
}



void renderTestGraphic(fp64 cycleSpeed, fp64 minSpeed, fp64 maxSpeed) {
	// nano64_t startTimer = getNanoTime();
	static fp64 f = 0.0;
	fp64 halfDiff = (maxSpeed - minSpeed) / 2.0;
	fp64 speed = halfDiff * sin(cycleSpeed * TAU * getDecimalTime()) + minSpeed + halfDiff;
	f += DeltaTime * speed;
	size_t w = (size_t)(f * (256.0));
	size_t z = 0;

	static constexpr size_t patternLength = 256;
	static constexpr size_t patternSize = patternLength * IMAGE_BUFFER_CHANNELS;

	size_t dimX = ((size_t)TestGraphic.resX > patternLength) ? patternLength : TestGraphic.resX;
	size_t dimY = ((size_t)TestGraphic.resY > patternLength) ? patternLength : TestGraphic.resY;
	size_t pitch = (TestGraphic.resX * IMAGE_BUFFER_CHANNELS);
	size_t offset = 0;
	for (size_t y = 0; y < dimY; y++) {
		z = offset;
		for (size_t x = 0; x < dimX; x++) {
			#ifdef fullColorTestGraphic
				TestGraphic.vram[z] = (x - w) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
				TestGraphic.vram[z] = (w - y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			#else
				TestGraphic.vram[z] = 0; z++;
				TestGraphic.vram[z] = 0; z++;
			#endif
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0xFF; z++;
		}
		inPlacePatternMemcpy(&TestGraphic.vram[offset], pitch, (dimX * IMAGE_BUFFER_CHANNELS));
		offset += pitch;
	}
	if ((size_t)TestGraphic.resY > patternLength) {
		inPlacePatternMemcpy(
			TestGraphic.vram,
			TestGraphic.resX * TestGraphic.resY * TestGraphic.channels,
			patternLength * TestGraphic.resX * TestGraphic.channels
		);
	}
	// nano64_t finishTimer = getNanoTime();
	// printfInterval(0.2,"\nGRAPHIC: %.3lf",(fp64)(finishTimer - startTimer) / 1.0e6);
}

namespace Status_Graphic {
	enum Status_Graphic_Enum {Graphic_Abort, Graphic_Pause, Graphic_Loading, Graphic_Count};
}

void renderStatusGraphic(Status_Graphic::Status_Graphic_Enum status_graphic, fp64 speed) {
	if (printValidateBufferBox(&TestGraphic) == false) {
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
				pattern[z] = (w + p) % 256; pattern[z] /= color_square_divider; z++;
				pattern[z] = ((w + p) % 256) / 4; pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				pattern[z] = 0xFF; z++;
			}
			break;
		case Status_Graphic::Graphic_Pause:
			for (uint32_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = (w + p) % 256; pattern[z] /= color_square_divider; z++;
				pattern[z] = 0; z++;
				pattern[z] = 0xFF; z++;
			}
			break;
		case Status_Graphic::Graphic_Loading:
		default:
			for (uint32_t p = 0; p < patternLength; p++) {
				pattern[z] = 0; z++;
				pattern[z] = 0; z++;
				pattern[z] = (w + p) % 256; pattern[z] /= color_square_divider; z++;
				pattern[z] = 0xFF; z++;
			}
			break;
	};
	size_t offset = 0;
	size_t shift = 0;
	size_t pitch = ((size_t)TestGraphic.resX * (size_t)TestGraphic.channels);
	size_t dimY = ((size_t)TestGraphic.resY > patternLength) ? patternLength : (size_t)TestGraphic.resY;
	for (size_t y = 0; y < dimY; y++) {
		size_t dimX = (pitch > shift) ? shift : pitch;
		memcpy(&TestGraphic.vram[offset], &pattern[patternSize - shift], dimX);
		if (shift <= pitch) {
			patternMemcpy(&TestGraphic.vram[offset + shift], pitch - shift, pattern, patternSize);
		}
		offset += pitch;
		shift += IMAGE_BUFFER_CHANNELS;
		shift %= patternSize;
	}
	if ((size_t)TestGraphic.resY > patternLength) {
		inPlacePatternMemcpy(
			TestGraphic.vram,
			TestGraphic.resX * TestGraphic.resY * TestGraphic.channels,
			patternLength * TestGraphic.resX * TestGraphic.channels
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
		Fractal_Data superFrac = frac;
		Render_Data superRenderData = primaryRenderData;
		if (superFrac.type_value == Fractal_ABS_Mandelbrot) {
			superFrac.type.abs_mandelbrot.maxItr = super_screenshot_maxItr;
		} else if (superFrac.type_value == Fractal_Polar_Mandelbrot) {
			superFrac.type.polar_mandelbrot.maxItr = super_screenshot_maxItr;
		}
		superRenderData.resX = super_screenshot_resX;
		superRenderData.resY = super_screenshot_resY;
		superRenderData.sample = super_screenshot_super_sample;
		superRenderData.subSample = 1;
		superRenderData.CPU_Threads = super_screenshot_maxThreads * super_screenshot_threadMultiplier;
		const User_Screenshot_Settings& screenshot_settings = config_data.Screenshot_Settings;
		switch(screenshot_settings.screenshotFileType) {
			case Image_File_Format::PNG:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::PNG,screenshot_settings.PNG_Compression_Level);
			break;
			case Image_File_Format::JPG:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::JPG,screenshot_settings.JPG_Quality_Level);
			break;
			default:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::PNG,screenshot_settings.PNG_Compression_Level);
		}
	}
	return 0;
}



int displayFracImage(ImageBuffer* image, Render_Data* ren) {
	if (image == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	if (image->vram == nullptr) { printError("ImageBuffer* image->vram is NULL"); return -1; }
	if (image->allocated() == false) { printError("ImageBuffer* image is not allocated"); return -1; }
	if (ren == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	#define FRAC frac.type.abs_mandelbrot
	static const uint32_t minimumImageResolution = 2;
	if (image->resX < minimumImageResolution || image->resY < minimumImageResolution) {
		printWarning("ImageBuffer* image is below minimum resolution: %ux%u",image->resX,image->resY);
		return 1;
	}
	i32 fx0 = 0; i32 fy0 = 0;
	i32 fx1 = 0; i32 fy1 = 0;
	coordinate_to_pixel(image->x00 - FRAC.r,image->y00 - FRAC.i,&fx0,&fy0,&FRAC,ren);
	coordinate_to_pixel(image->x11 - FRAC.r,image->y11 - FRAC.i,&fx1,&fy1,&FRAC,ren);
	if (fx0 > fx1) { i32 temp = fx0; fx0 = fx1; fx1 = temp; }
	if (fy0 > fy1) { i32 temp = fy0; fy0 = fy1; fy1 = temp; }
	i32 fxA = (fx0 + fx1) / 2;
	i32 fyA = (fy0 + fy1) / 2;
	if ((fx1 < (i32)minimumImageResolution || fy1 < (i32)minimumImageResolution)) {
		return 1;
	}
	if ((image->rot != FRAC.rot) || ((fx0 < (i32)Master.resX) && (fy0 < (i32)(Master.resY - RESY_UI)))) {
		scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(image->vram, image->resX, image->resY, image->channels * 8, image->channels * image->resX, SDL_PIXELFORMAT_ABGR8888);
		fx1 -= fx0;
		fy1 -= fy0;
		SDL_Rect srcRect = {0,0,(i32)image->resX,(i32)image->resY};
		SDL_Rect dstRect = {fx0,fy0 + (i32)RESY_UI,fx1,fy1};
		scale_tex = SDL_CreateTextureFromSurface(renderer, scale_surface);
		if (SDL_RenderCopy(renderer, scale_tex, &srcRect, &dstRect)) {
			printf("\nrenderCopy: %s",SDL_GetError()); fflush(stdout);
		}
		SDL_DestroyTexture(scale_tex);
		SDL_FreeSurface(scale_surface);
	}
	#undef FRAC
	return 0;
}

int transformFracImage(ImageBuffer* image, Render_Data* ren) {
	if (image == nullptr) { printError("ImageBuffer* image is NULL"); return -1; }
	if (image->vram == nullptr) { printError("ImageBuffer* image->vram is NULL"); return -1; }
	if (image->allocated() == false) { printError("ImageBuffer* image is not allocated"); return -1; }
	if (ren == NULL) { printError("ImageBuffer* image is NULL"); return -1; }
	#define FRAC frac.type.abs_mandelbrot
	static const uint32_t minimumImageResolution = 2;
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
	int32_t resX = (int32_t)(image->resX);
	int32_t resY = (int32_t)(image->resY);
	// printfInterval(0.5,
	// 	"\nres{%ux%u}"
	// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}"
	// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}\n",
	// 	resX,resY,
	// 	dx00,dy00,dx10,dy10,
	// 	dx01,dy01,dx11,dy11
	// );
	fp32 dimX = ((fp32)ren->resX / (fp32)ren->subSample);
	fp32 dimY = ((fp32)ren->resY / (fp32)ren->subSample);
	fp32 sx00 = 0.0f; fp32 sy00 = 0.0f;
	fp32 sx11 = (fp32)resX; fp32 sy11 = (fp32)resY;
	fp32 sx01 = 0.0f; fp32 sy01 = (fp32)resY;
	fp32 sx10 = (fp32)resX; fp32 sy10 = 0.0f;
	// image->printTransformationData(0.6);
	// printfInterval(0.6,"\nsrc: 00{%d,%d} 11{%d,%d} 01{%d,%d} 10{%d,%d}",sx00,sy00,sx11,sy11,sx01,sy01,sx10,sy10);
	// printfInterval(0.6,"\ndst: 00{%d,%d} 11{%d,%d} 01{%d,%d} 10{%d,%d}\n",dx00,dy00,dx11,dy11,dx01,dy01,dx10,dy10);
	uint32_t backgroundColor = 0xFF000000;
	backgroundColor |= (uint32_t)(FRAC.rA * (127.5 - 127.5 * cos(TAU * FRAC.rP)));
	backgroundColor |= (uint32_t)(FRAC.gA * (127.5 - 127.5 * cos(TAU * FRAC.gP))) << 8;
	backgroundColor |= (uint32_t)(FRAC.bA * (127.5 - 127.5 * cos(TAU * FRAC.bP))) << 16;

	if (
		Image_Scaler_Parallelogram(
			&blit, image, ren,
			backgroundColor,
			nullptr, nullptr,
			Rendering_Settings.Frame_Interpolation_Method,
			sx00, sy00,
			sx01, sy01, sx10, sy10,
			dx00, dy00,
			dx01, dy01, dx10, dy10
		) == -1
	) {
		printError("\nImage_Scaler_Parallelogram failed");
		return -1;
	}
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
	// 	printError("\nImage_Scaler_Quadrilateral failed");
	// 	return -1;
	// }
	//printfInterval(0.6,"\n%p: %ux%u %uC %uP",blit.vram,blit.resX,blit.resY,blit.channels,blit.padding);
	//uint64_t stopWatch = getNanoTime();
	copyBuffer(blit,temp_MASTER,0,RESY_UI,true);
	//printfInterval(0.6,"\nFunc: %.3lfms\n",NANO_TO_SECONDS(getNanoTime() - stopWatch) * 1000.0);
	FREE(blit.vram);
	#undef FRAC
	return 0;
}

void newFrame() {
	if (Master.bufferSafe() == false) {
		printError("Master ImageBuffer is invalid");
		return;
	}
	if (frac.type_value == Fractal_ABS_Mandelbrot || frac.type_value == Fractal_Polar_Mandelbrot) {
		#define FRAC frac.type.abs_mandelbrot
		Master.clearBuffer(
			(uint8_t)(FRAC.rA * (127.5 - 127.5 * cos(TAU * FRAC.rP))),
			(uint8_t)(FRAC.gA * (127.5 - 127.5 * cos(TAU * FRAC.gP))),
			(uint8_t)(FRAC.bA * (127.5 - 127.5 * cos(TAU * FRAC.bP)))
		);
		#undef FRAC
	} else {
		Master.clearBuffer();
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
		if (Waiting_To_Abort_Rendering == true) {
			renderStatusGraphic(Status_Graphic::Graphic_Abort, 0.3);
		} else {
			renderStatusGraphic(Status_Graphic::Graphic_Pause, 0.4);
			//renderTestGraphic(0.2,0.4,1.0);
		}
		BufferBox temp_MASTER;
		Master.getBufferBox(&temp_MASTER);
		copyBuffer(TestGraphic,temp_MASTER,0,RESY_UI,false);
		exportFractalBuffer = false;
	} else if (primaryBufferValid == false) {
		renderStatusGraphic(Status_Graphic::Graphic_Loading,1.0); // Renders a loading screen if Fractal buffers are unavailable
		BufferBox temp_MASTER;
		Master.getBufferBox(&temp_MASTER);
		copyBuffer(TestGraphic,temp_MASTER,0,RESY_UI,false);
		exportFractalBuffer = false;
	}
	#ifdef Use_OpenCV_Scaler
		if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
			int scaleRet = transformFracImage(Primary_Image,&primaryRenderData);
			printfChange(int,scaleRet,"\ntransformFracImage: %d",scaleRet);
		}
	#endif
	SDL_UpdateTexture(texture, nullptr, Master.vram, Master.resX * Master.channels);
	{
		SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_Rect dstRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	}
	if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
		BufferBox temp_primaryBox;
		Primary_Image->getBufferBox(&temp_primaryBox);

		#ifndef Use_OpenCV_Scaler
			int dispRet = displayFracImage(Primary_Image,&primaryRenderData);
			printfChange(int,dispRet,"\ndisplayFracImage: %d",dispRet);
		#endif
		if (exportFractalBuffer == true) {
			uint64_t curTime = getNanoTime();
			size_t size = snprintf(nullptr,0,"%s_%llu",frac.type_name,curTime);
			char* name = (char*)calloc(size + 1,sizeof(char));
			snprintf(name,size,"%s_%llu",FractalTypeFileText[frac.type_value],curTime);
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
				printError("Unknown screenshot file type: %d",screenshot_settings.screenshotFileType);
			}
			FREE(name);
		}
		exportFractalBuffer = false;
	}
	
	render_IMGUI();
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(kTexture); // From render_IMGUI
}