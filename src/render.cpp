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

SDL_Renderer* renderer;
SDL_Window* window;
ImGuiIO* io_IMGUI;

bool Abort_Rendering_Flag = false;
bool Waiting_To_Abort_Rendering = false;

bool Lock_Key_Inputs = false;
bool LockKeyInputsInMenus = true;

bool AutoResizeWindows = false;
bool PreventOutOfBoundsWindows = false;
fp32 WindowOpacity = 0.95f;
fp64 WindowAutoScale = 0.7;
const int32_t ImGui_WINDOW_MARGIN = 8;
ImGuiWindowFlags ImGui_WINDOW_FLAGS = 0;

bool SaveUsernameInFiles = false; /* This MUST be False by Default */
#define FileUsernameLength 32
char FileUsername[FileUsernameLength];
bool SaveHardwareInfoInFiles = false; /* This MUST be False by Default */

int IMGUI_Theme = 1; // 0 Classic, 1 Dark, 2 Light

SDL_Texture* texture;
SDL_Texture* kTexture; // Keyboard graphic
ImageBuffer Master;
BufferBox TestGraphic;
fp64 TestGraphicSpeed = 0.4;

ImageBuffer* Primary_Image = nullptr;
ImageBuffer* Primary_Image_Preview = nullptr;
ImageBuffer* Secondary_Image = nullptr;
ImageBuffer* Secondary_Image_Preview = nullptr;

int Frame_Interpolation_Method;

//#define MANUAL_FRAME_RATE_OVERRIDE
fp64 FRAME_RATE = 60.0; // Double the max screen refresh rate
const fp64 FRAME_RATE_OFFSET = 0.01;
uint64_t FRAME_RATE_NANO;
#define  Default_Frame_Rate_Multiplier 1.0
const uint8_t color_square_divider = 2; //5 dark, 4 dim, 3 ambient, 2 bright, 1 the sun
fp64 DeltaTime = 0.0;
uint64_t END_SLEEP_HEADROOM = SECONDS_TO_NANO(0.02);

fp64 Frame_Time_Display = 0.0;
fp64 Render_Time_Display = 0.0;

uint32_t RESY_UI = 128;

uint64_t abortTimer = 0; // How long it is taking to abort the rendering jobs

#define RESX_Default 800
#define RESY_Default 600
#define RESX_Minimum 400
#define RESY_Minimum 320
#define RESX_Maximum 8192
#define RESY_Maximum 4608
#define RESX_Margin 16
#define RESY_Margin 16

/* Fractal Data */

Fractal_Data frac;
Render_Data primaryRenderData;
Render_Data secondaryRenderData;

#define default_Super_Screenshot_ResX 1920
#define default_Super_Screenshot_ResY 1080
#define default_Super_Screenshot_Sample 3
#define default_Super_Screenshot_MaxItr 16384
#define default_Super_Screenshot_ThreadMultiplier 6

int32_t super_screenshot_resX = default_Super_Screenshot_ResX;
int32_t super_screenshot_resY = default_Super_Screenshot_ResY;
int32_t super_screenshot_super_sample = default_Super_Screenshot_Sample;
int32_t super_screenshot_maxItr = default_Super_Screenshot_MaxItr;
int32_t super_screenshot_threadMultiplier = default_Super_Screenshot_ThreadMultiplier;
int32_t super_screenshot_maxThreads = 1;

Render_Data primarySuperRenderData;
Render_Data secondarySuperRenderData;

// ImageBuffer primaryFracImage;
// ImageBuffer secondaryFracImage;

int exportScreenshot();
int exportSuperScreenshot();

Image_File_Format::Image_File_Format_Enum screenshotFileType = Image_File_Format::PNG;
uint32_t User_PNG_Compression_Level = 8;
uint32_t User_JPG_Quality_Level = 95;

User_Parameter_Sensitivity user_sensitivity;

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

const uint8_t* KEYS;

struct _Key_Status {
	SDL_Scancode key;
	bool pressed;
	uint64_t timePressed;
	uint64_t timeReleased;
}; typedef struct _Key_Status Key_Status;

size_t KeyBind_PresetCount;
//KeyBind_Preset* KeyBind_List;
// KeyBind_Preset defaultKeyBind = {"Default Key-bind",ARRAY_LENGTH(defaultKeyBind),defaultKeyBind};
// KeyBind_Preset importedKeyBind = {"Blank",0,NULL};
// KeyBind_Preset* currentKeyBind = &defaultKeyBind;

//std::list<KeyBind> currentKeyBind = defaultKeyBind;
//std::list<KeyBind> importedKeyBind; // Deprecate this

/* KeyBind_Preset */
	std::list<KeyBind_Preset> KeyBind_PresetList;
	KeyBind_Preset* currentKBPreset;

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

Key_Status Key_List[SDL_NUM_SCANCODES];

Function_Status func_stat[Key_Function::Parameter_Function_Count];

void updateKeys() {
	for (size_t t = 0; t < ARRAY_LENGTH(func_stat); t++) {
		func_stat[t].triggered = false;
	}
	KEYS = SDL_GetKeyboardState(NULL);
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (KEYS[i] != 0) { // Key Pressed
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
		}; typedef init_key_HSV init_key_HSV;
		init_key_HSV InitKeyHSV[] = {
			{NONE,0.0,0.0,0.5},
			{COORDINATES,0.0,0.7,1.0},{TRANSFORMATIONS,180.0,0.7,1.0},{JULIA,300.0,0.6,1.0},
			{PARAMETERS,59.9,1.0,1.0},{POLAR,240.0,0.7,0.7},{FORMULA,210.0,0.3,0.8},
			{SCREEN_SPLIT,30.0,0.3,1.0},{FUNCTIONS,120.0,0.8,0.8},{RENDERING,180.0,0.4,0.6},
		};
		struct init_key_RGB {
			enum Key_Function_Enum type;
			uint8_t r; uint8_t g; uint8_t b;
		}; typedef init_key_RGB init_key_RGB;
		init_key_RGB InitKeyRGB[ARRAY_LENGTH(InitKeyHSV)];
		for (size_t i = 0; i < ARRAY_LENGTH(InitKeyHSV); i++) {
			getRGBfromHSV(&(InitKeyRGB[i].r),&(InitKeyRGB[i].g),&(InitKeyRGB[i].b),InitKeyHSV[i].h,InitKeyHSV[i].s,InitKeyHSV[i].v);
		}
		// if (currentKBPreset->kList.size() <= 0) {
		// 	currentKBPreset->kList = defaultKeyBind;
		// }
		for (size_t s = 0; s < SDL_NUM_SCANCODES; s++) {
			size_t keyColorSet = 0;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == (SDL_Scancode)s) {
					if (keyColorSet != 0) { // Detects if different function types are binded to a key
						setRGB_Scancode(0xFF,0xFF,0xFF,(SDL_Scancode)s);
						break; // Idempotent
					} 
					for (size_t r = SDL_SCANCODE_UNKNOWN + 1; r < ARRAY_LENGTH(Key_Function_Map); r++) {
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

// Amount of displays detected
uint32_t DISPLAY_COUNT = 0;
/* Display Bootup */
	namespace Display_Bootup {
		enum Display_Bootup_Enum {
			Automatic,First,Last,Specific,Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,HighResolution,HighFrameRate,LowResolution,LowFrameRate,Length
		};
	};
	uint32_t SPECIFIC_BOOTUP_DISPLAY = 1; // Supposed to be save data
	uint32_t Display_Match[Display_Bootup::Length];
	Display_Bootup::Display_Bootup_Enum Display_Bootup_Type = Display_Bootup::Automatic;
	bool useDefaultWindowSize = false;

struct _DisplayInfo {
	uint32_t resX;
	uint32_t resY;
	int32_t posX;
	int32_t posY;
	uint32_t refreshRate;
	uint8_t bbp;
	const char* name;
}; typedef struct _DisplayInfo DisplayInfo;
DisplayInfo* DisplayList;

// Counts from ONE
uint32_t CURRENT_DISPLAY = 1;
// Counts from ONE
DisplayInfo* getDisplayInfo(size_t i = 1) {
	if (i == 0 || i > DISPLAY_COUNT || DisplayList == NULL) {
		return NULL;
	}
	return &DisplayList[i - 1];
}
// Counts from ONE
DisplayInfo* getCurrentDisplayInfo() {
	return getDisplayInfo((size_t)CURRENT_DISPLAY);
}

static const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","Top-Left Corner","Top-Right Corner","Bottom-Left Corner","Bottom-Right Corner","Floating"};

#ifndef BUILD_RELEASE
	const char* buttonLabels[] = {"Fractal", "Export", "Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
#else
	const char* buttonLabels[] = {"Fractal", "Screenshot", "Rendering", "Settings", "KeyBinds"};
#endif

int buttonSelection = -1;
bool ShowTheXButton = true;
//bool yeildSwitch = true;

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

enum Menu_Enum {GUI_Menu_None, GUI_Menu_Coordinates, GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds, GUI_Menu_Count};

#define stretchValue(s) pow(2.0,-abs(s))
#define zoomDefault(p) (-log10(getABSFractalMaxRadius((fp64)(p))) - 0.01)

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

#define Update_Level(level); update_level = ((level) > update_level) ? (level) : update_level;
int get_ABS_Mandelbrot_Update_Level(ABS_Mandelbrot* frac, Render_Data* ren, int update_level = Change_Level::Nothing) {
	if (frac == nullptr) { return update_level; }
	if (ren == nullptr) { return update_level; }
	static ABS_Mandelbrot frac0 = *frac;
	static Render_Data ren0 = *ren;
	using namespace Change_Level;
	
	/* Render_Data */
	if (ren->resX != ren0.resX || ren->resY != ren0.resY) {
		Update_Level(Change_Level::Resolution);
	}
	if (ren->rendering_method != ren0.rendering_method) {
		Update_Level(Change_Level::Method_of_Rendering);
	} else if (
		(
			(ren->rendering_method == Rendering_Method::CPU_Rendering) &&
			((ren->CPU_Precision != ren0.CPU_Precision) || (ren->CPU_Threads != ren0.CPU_Threads))
		) && (
			(ren->rendering_method == Rendering_Method::GPU_Rendering) &&
			(ren->GPU_Precision != ren0.GPU_Precision)
		)
	) {
		Update_Level(Change_Level::Method_of_Rendering);
	}
	if (ren->sample != ren0.sample) {
		Update_Level(Change_Level::Super_Sample);
	}
	if (ren->subSample != ren0.subSample) {
		Update_Level(Change_Level::Resolution);
	}
	if (ren->flip != ren0.flip) {
		Update_Level(Change_Level::Rotation);
	}
	/* ABS_Mandelbrot */
	if (frac->r != frac0.r || frac->i != frac0.i) {
		Update_Level(Change_Level::Translation);
	}
	if (frac->zoom != frac0.zoom) {
		Update_Level(Change_Level::Zoom);
	}
	if (frac->zr != frac0.zr || frac->zi != frac0.zi) {
		Update_Level(Change_Level::Julia);
	}
	if (frac->maxItr != frac0.maxItr || frac->maxItr_Log2 != frac0.maxItr_Log2) {
		Update_Level(Change_Level::Iterations);
	}
	if (frac->rot != frac0.rot) {
		Update_Level(Change_Level::Rotation);
	}
	if (frac->stretch != frac0.stretch) {
		Update_Level(Change_Level::Stretch);
	}
	if (frac->breakoutValue != frac0.breakoutValue) {
		Update_Level(Change_Level::Breakout);
	}
	if (
		(frac->rA != frac0.rA || frac->rF != frac0.rF || frac->rP != frac0.rP) ||
		(frac->gA != frac0.gA || frac->gF != frac0.gF || frac->gP != frac0.gP) ||
		(frac->bA != frac0.bA || frac->bF != frac0.bF || frac->bP != frac0.bP) ||
		(frac->iA != frac0.iA || frac->iF != frac0.iF || frac->iP != frac0.iP)
	) {
		Update_Level(Change_Level::Coloring);
	}
	if (frac->smoothColoring != frac0.smoothColoring) {
		Update_Level(Change_Level::Coloring);
	}
	if (frac->power != frac0.power) {
		Update_Level(Change_Level::Power_Change);
	}
	if (frac->polarPower != frac0.polarPower) {
		Update_Level(Change_Level::Polar_Power);
	}
	if (frac->formula != frac0.formula) {
		Update_Level(Change_Level::Formula_Change);
	}
	if (frac->polarMandelbrot != frac0.polarMandelbrot) {
		Update_Level(Change_Level::Fractal_Change);
	}
	if (frac->juliaSet != frac0.juliaSet) {
		Update_Level(Change_Level::Fractal_Change);
	}
	frac0 = *frac;
	ren0 = *ren;
	return update_level;
}

int updateFractalParameters() {
	using namespace Key_Function;
	using namespace Change_Level;
	#define FRAC frac.type.abs_mandelbrot
	fp64 temp_breakoutValue = log2(FRAC.breakoutValue);
	fp64 moveDelta = (DeltaTime < 0.2) ? DeltaTime : 0.2;
	
	moveDelta *= user_sensitivity.global;

	int update_level = Change_Level::Nothing;

	/* Magic Constants */
		#define ABS_Mandelbrot_Default_Power 2
		#define Polar_Mandelbrot_Default_Power 3.0
	/* Boolean toggles */
		#define paramToggle(func,toggle,freq) if (funcTimeDelay(func,freq)) { toggle = !toggle; }
		#define paramToggleUpdate(func,toggle,freq,level) if (funcTimeDelay(func,freq)) { toggle = !toggle; Update_Level(level); }
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
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.coordinate
			);
		}
		if (func_stat[decRealPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.5 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.coordinate
			);
		}
		if (func_stat[incImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.25 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.coordinate
			);
		}
		if (func_stat[decImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.75 * TAU + FRAC.rot,
				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.coordinate
			);
		}
		if (funcTimeDelay(resetRealPos,0.2)) {
			FRAC.r = 0.0;
			Update_Level(Jump);
		}
		if (funcTimeDelay(resetImagPos,0.2)) {
			FRAC.i = 0.0;
			Update_Level(Jump);
		}
	/* Real and Imaginary Julia Z Coordinates */
		if (func_stat[incZReal].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.0 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.julia
			);
		}
		if (func_stat[decZReal].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.5 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.julia
			);
		}
		if (func_stat[incZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.25 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.julia
			);
		}
		if (func_stat[decZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.75 * TAU + FRAC.rot,
				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.julia
			);
		}
		if (funcTimeDelay(resetZReal,0.2)) {
			FRAC.zr = 0.0;
			Update_Level(Jump);
		}
		if (funcTimeDelay(resetZImag,0.2)) {
			FRAC.zi = 0.0;
			Update_Level(Jump);
		}
		if (FRAC.cursorZValue == true) {
			if (FRAC.relativeZValue == true) {
				fp64 resZ = (fp64)((Master.resX > Master.resY) ? Master.resY : Master.resX);
				FRAC.zr = 4.0 * ((fp64)ImGui::GetMousePos().x - ((fp64)Master.resX / 2.0)) / resZ;
				FRAC.zi = 4.0 * ((fp64)(ImGui::GetMousePos().y - RESY_UI) - ((fp64)Master.resY / 2.0)) / resZ;
			} else {
				pixel_to_coordinate(ImGui::GetMousePos().x,ImGui::GetMousePos().y - RESY_UI,&FRAC.zr,&FRAC.zi,&FRAC,&primaryRenderData);
			}
		}
	/* Zoom */
		if (func_stat[incZoom].triggered == true) {
			FRAC.zoom += 0.25 * moveDelta * user_sensitivity.zoom * (user_sensitivity.invert_zoom ? -1.0 : 1.0);
		}
		if (func_stat[decZoom].triggered == true) {
			FRAC.zoom -= 0.25 * moveDelta * user_sensitivity.zoom * (user_sensitivity.invert_zoom ? -1.0 : 1.0);
		}
		if (funcTimeDelay(resetZoom,0.2)) {
			FRAC.zoom = zoomDefault(FRAC.power);
			Update_Level(Jump);
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
			Update_Level(Jump);
		}
	/* maxItr */
		if (func_stat[incMaxItr].triggered) {
			FRAC.maxItr_Log2 += 2.0 * moveDelta * user_sensitivity.maxIter;
			setMaxItr(&FRAC,FRAC.maxItr_Log2);
		}
		if (func_stat[decMaxItr].triggered) {
			FRAC.maxItr_Log2 -= 2.0 * moveDelta * user_sensitivity.maxIter;
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
					FRAC.polarPower += moveDelta * 1.0 * user_sensitivity.polar_power;
				}
				if (func_stat[decPower].triggered) {
					FRAC.polarPower -= moveDelta * 1.0 * user_sensitivity.polar_power;
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
			FRAC.rot -= (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch) * user_sensitivity.rotation;
		}
		if (func_stat[clockwiseRot].triggered) {
			FRAC.rot += (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch) * user_sensitivity.rotation;
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
			FRAC.stretch += 1.0 * moveDelta * user_sensitivity.stretch;
		}
		if (func_stat[decStretch].triggered) {
			FRAC.stretch -= 1.0 * moveDelta * user_sensitivity.stretch;
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
			temp_breakoutValue += 2.0 * moveDelta * user_sensitivity.breakout_value;
		}
		if (func_stat[decBreakout].triggered) {
			temp_breakoutValue -= 2.0 * moveDelta * user_sensitivity.breakout_value;
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

#define BufAndLen(x) x,ARRAY_LENGTH(x)
/* Sets defualt window size and position along with size constraints */
#define ImGui_DefaultWindowSize(valX,bufX,minX,maxX,ratioX,valY,bufY,minY,maxY,ratioY); \
	uint32_t WINDOW_RESX = calcMinMaxRatio(valX-bufX,minX,maxX,ratioX); \
	uint32_t WINDOW_RESY = calcMinMaxRatio(valY-bufY,minY,maxY,ratioY); \
	ImGui::SetNextWindowPos({(fp32)((valX - WINDOW_RESX) / 2),(fp32)((valY - WINDOW_RESY) / 2)}, ImGuiCond_Once); \
	ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY}, ImGuiCond_Once); \
	if (AutoResizeWindows == true) { \
		ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY}); \
	} \
	ImGui::SetNextWindowSizeConstraints({(fp32)minX,(fp32)minY},{(fp32)valX - bufX,(fp32)valY - bufY}); \
	WINDOW_RESX = (WINDOW_RESX > valX - bufX) ? (valX - bufX) : WINDOW_RESX; \
	WINDOW_RESY = (WINDOW_RESY > valY - bufY) ? (valY - bufY) : WINDOW_RESY; \
	ImGui::SetNextWindowBgAlpha(WindowOpacity);

#define ImGui_BoundWindowPosition(); \
	if (PreventOutOfBoundsWindows == true) { \
		int32_t WINDOW_POSX = ImGui::GetWindowPos().x; \
		int32_t WINDOW_POSY = ImGui::GetWindowPos().y; \
		valueLimit(WINDOW_POSX,ImGui_WINDOW_MARGIN,(int32_t)Master.resX - (int32_t)ImGui::GetWindowSize().x - ImGui_WINDOW_MARGIN); \
		valueLimit(WINDOW_POSY,ImGui_WINDOW_MARGIN,(int32_t)Master.resY - (int32_t)ImGui::GetWindowSize().y - ImGui_WINDOW_MARGIN); \
		ImGui::SetWindowPos({(fp32)(WINDOW_POSX),(fp32)(WINDOW_POSY)}); \
	}



void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
    ImGui::Begin("Horizontal Button Page", NULL, window_flags);

	static ImVec4 GUI_FrameRateColor;
	static ImVec4 Render_FrameRateColor;
	fp64 Frame_FPS_Display = 1.0 / Frame_Time_Display;
	fp64 Render_FPS_Display = 1.0 / Render_Time_Display;
	GUI_FrameRateColor = {
		(fp32)linearInterpolationLimit(Frame_FPS_Display,59.0,119.0,1.0,0.0),
		(fp32)linearInterpolationLimit(Frame_FPS_Display,0.0,29.0,0.0,1.0),
		(fp32)linearInterpolationLimit(Frame_FPS_Display,29.0,59.0,0.0,1.0),
		1.0
	};
	Render_FrameRateColor = {
		(fp32)linearInterpolationLimit(Render_FPS_Display,59.0,119.0,1.0,0.0),
		(fp32)linearInterpolationLimit(Render_FPS_Display,0.0,29.0,0.0,1.0),
		(fp32)linearInterpolationLimit(Render_FPS_Display,29.0,59.0,0.0,1.0),
		1.0
	};
	
	ImGui::Text("GUI:"); ImGui::SameLine();
	ImGui::TextColored(GUI_FrameRateColor,"%.2lf",Frame_FPS_Display); ImGui::SameLine(0.0,1.0);
	ImGui::Text("FPS"); ImGui::SameLine();
	ImGui::TextColored(GUI_FrameRateColor,"%.2lf",Frame_Time_Display * 1000.0); ImGui::SameLine(0.0,1.0);
	ImGui::Text("ms");
	ImGui::SameLine();
	ImGui::Text("Render:"); ImGui::SameLine();
	ImGui::TextColored(Render_FrameRateColor,"%.2lf",Render_FPS_Display); ImGui::SameLine(0.0,1.0);
	ImGui::Text("FPS"); ImGui::SameLine();
	ImGui::TextColored(Render_FrameRateColor,"%.2lf",Render_Time_Display * 1000.0); ImGui::SameLine(0.0,1.0);
	ImGui::Text("ms");

    size_t buttonCount = sizeof(buttonLabels) / sizeof(buttonLabels[0]);
	if (ImGui::Button("Coordinates")) {
		buttonSelection = (buttonSelection == GUI_Menu_Coordinates) ? -1 : GUI_Menu_Coordinates;
	} ImGui::SameLine();
	if (ImGui::Button("Fractal")) {
		buttonSelection = (buttonSelection == GUI_Menu_Fractal) ? -1 : GUI_Menu_Fractal;
	} ImGui::SameLine();
	#ifndef BUILD_RELEASE
		if (ImGui::Button("Export")) {

		} ImGui::SameLine();
		if (ImGui::Button("Import")) {
			buttonSelection = (buttonSelection == GUI_Menu_Import) ? -1 : GUI_Menu_Import;
		} ImGui::SameLine();
	#endif
	if (ImGui::Button("Screenshot")) {
		exportScreenshot();
	} ImGui::SameLine();
	if (ImGui::Button("Rendering")) {
		buttonSelection = (buttonSelection == GUI_Menu_Rendering) ? -1 : GUI_Menu_Rendering;
	} ImGui::SameLine();
	if (ImGui::Button("Settings")) {
		buttonSelection = (buttonSelection == GUI_Menu_Settings) ? -1 : GUI_Menu_Settings;
	} ImGui::SameLine();
	if (ImGui::Button("Key-binds")) {
		buttonSelection = (buttonSelection == GUI_Menu_KeyBinds) ? -1 : GUI_Menu_KeyBinds;
	} ImGui::SameLine();
	if (Waiting_To_Abort_Rendering == true) {
		ImGui::Text("Aborting...(%.1lfs)",NANO_TO_SECONDS(getNanoTime() - abortTimer));
	} else {
		if (Abort_Rendering_Flag == true) {
			if (ImGui::Button("Resume Rendering")) {
				Abort_Rendering_Flag = false;
			}
		} else {
			if (ImGui::Button("Abort Rendering")) {
				Abort_Rendering_Flag = true;
				Waiting_To_Abort_Rendering = true;
				write_Abort_Render_Ongoing(true);
				abortTimer = getNanoTime();
			}
		}
	}
	if (Lock_Key_Inputs == true) {
		ImGui::SameLine();
		ImGui::Text("Key inputs are locked inside of menus");
	}

	ImGui::Separator();
	uint32_t boxSpace = 8;
	uint32_t boxCount = 8;
	fp32 boxWidth = (fp32)(Master.resX - (boxSpace * (boxCount + 1))) / (fp32)boxCount;

	#define Param_Input_Box(text,id,buf) \
	ImGui::Text(text); \
	ImGui::SetNextItemWidth(boxWidth); \
	ImGui::SameLine(); \
	ImGui::InputText(id,buf,sizeof(buf)); \
	correctTextFloat(buf,sizeof(buf),0);
	
	#define Param_Input_Double(text,id,ptr,fm) \
	ImGui::Text(text); \
	ImGui::SetNextItemWidth(boxWidth); \
	ImGui::SameLine(); \
	ImGui::InputDouble(id,ptr,0.0,0.0,fm);

	#define Param_Input_Int(text,id,ptr) \
	ImGui::Text(text); \
	ImGui::SetNextItemWidth(boxWidth); \
	ImGui::SameLine(); \
	ImGui::InputInt(id,ptr,16,256);

	#define FRAC frac.type.abs_mandelbrot
	uint32_t renderFP = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? primaryRenderData.CPU_Precision : primaryRenderData.GPU_Precision;
	const char* const renderMethod = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? "CPU" : "GPU";
	ImGui::Text(
		"Formula: %llu Power: %6.4lf Super-Sample: %u Rendering: %s fp%u",
		FRAC.formula,(FRAC.polarMandelbrot ? FRAC.polarPower : (fp64)FRAC.power),primaryRenderData.sample * primaryRenderData.sample,renderMethod,renderFP
	);
	#define temp_quad_len 64
	static char temp_quad_r[temp_quad_len]; static char temp_quad_i[temp_quad_len];
	static char temp_quad_zr[temp_quad_len]; static char temp_quad_zi[temp_quad_len];
	quadmath_snprintf(temp_quad_r,temp_quad_len,"%15.12Qf",FRAC.r);
	quadmath_snprintf(temp_quad_i,temp_quad_len,"%15.12Qf",FRAC.i);
	quadmath_snprintf(temp_quad_zr,temp_quad_len,"%15.12Qf",FRAC.zr);
	quadmath_snprintf(temp_quad_zi,temp_quad_len,"%15.12Qf",FRAC.zi);
	ImGui::Text(
		"Zreal: %s Zimag: %s Rotation: %5.1lf Stetch: 2^%6.4lf",
		temp_quad_zr,temp_quad_zi,FRAC.rot * 360.0 / TAU,FRAC.stretch
	);
	ImGui::Text(" ");
	fp64 adjustedZoomValue = FRAC.zoom;
	if (FRAC.adjustZoomToPower == true) {
		if (FRAC.polarMandelbrot == true) {
			adjustedZoomValue += log10(getABSFractalMaxRadius(FRAC.polarPower));
		} else {
			adjustedZoomValue += log10(getABSFractalMaxRadius((fp64)FRAC.power));
		}
		valueLimit(adjustedZoomValue,FRAC.zoom - 0.4, FRAC.zoom + 0.4);
	}
	
	ImGui::Text(
		"Real:  %s Imag:  %s Zoom: 10^%6.4lf Itr: %u",
		temp_quad_r,temp_quad_i,adjustedZoomValue,FRAC.maxItr
	);
	#undef FRAC
    // End the ImGui window
    ImGui::End();
}

void Menu_Coordinates() {
	ImGui_DefaultWindowSize(Master.resX,ImGui_WINDOW_MARGIN * 2,240,400,WindowAutoScale,Master.resY,ImGui_WINDOW_MARGIN * 2,160,320,WindowAutoScale);
	ImGui::Begin("Coordinates Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition();

	if (frac.type_value == Fractal_ABS_Mandelbrot || frac.type_value == Fractal_Polar_Mandelbrot) { /* ABS and Polar */
		#define FRAC frac.type.abs_mandelbrot
		#define NumberTextLen 64
		
		#define Quad_InputText(lbl, num, fmt); \
			{ \
				static char Temp_Text_Input_Buf[NumberTextLen]; \
				quadmath_snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
				if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
					num = strtoflt128(Temp_Text_Input_Buf, nullptr); \
				} \
			}
		#define Float_InputText(lbl, num, fmt, func); \
			{ \
				static char Temp_Text_Input_Buf[NumberTextLen]; \
				snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
				if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
					num = func(Temp_Text_Input_Buf, nullptr); \
				} \
			}
		#define Int_InputText(lbl, num, fmt, func, base); \
			{ \
				static char Temp_Text_Input_Buf[NumberTextLen]; \
				snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
				if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
					num = func(Temp_Text_Input_Buf, nullptr, base); \
				} \
			}
		ImGui::Text("Real and Imaginary Coordinate:");
			Quad_InputText("r",FRAC.r,"%35.32Qf");
			Quad_InputText("i",FRAC.i,"%35.32Qf");
		ImGui::Text("Zoom:");
			Float_InputText("##zoom_input",FRAC.zoom,"%.5lf",strtod);
		ImGui::Text("Julia Coordinate:");
			static bool useJuliaSliders = true;
			if (useJuliaSliders == true) {
				float input_Zreal = (fp32)FRAC.zr; float input_Zimag = (fp32)FRAC.zi;
				if (ImGui::SliderFloat("zr",&input_Zreal,-2.0,2.0,"%.9f")) { FRAC.zr = (fp64)input_Zreal; }
				if (ImGui::SliderFloat("zi",&input_Zimag,-2.0,2.0,"%.9f")) { FRAC.zi = (fp64)input_Zimag; }
			} else {
				Quad_InputText("##input_Zreal",FRAC.zr,"%35.32Qf");
				Quad_InputText("##input_zimag",FRAC.zi,"%35.32Qf");
			}
			ImGui::Checkbox("Use Sliders", &useJuliaSliders);
		ImGui::Text("Maximum Iterations:");
		Int_InputText("##input_maxIter",FRAC.maxItr,"%u",strtoul,10);
		ImGui::Text("Fractal Formula:");
			static bool inputHexadecimal = false;
			if (inputHexadecimal == true) {
				Int_InputText("##input_formula",FRAC.formula,"%llX",strtoull,16);
			} else {
				Int_InputText("##input_formula",FRAC.formula,"%llu",strtoull,10);
			}
			ImGui::Checkbox("Hexadecimal", &inputHexadecimal);
		/* Power */
			if (frac.type_value == Fractal_ABS_Mandelbrot) {
				ImGui::Text("Power: %s",getPowerText((int32_t)FRAC.power));
				Int_InputText("##input_power",FRAC.power,"%u",strtoul,10);
			} else if (frac.type_value == Fractal_Polar_Mandelbrot) {
				ImGui::Text("Power: %s",getPowerText(round(FRAC.polarPower)));
				Float_InputText("##input_polar_power",FRAC.polarPower,"%.5lf",strtod);
			}
		#undef FRAC
	} else if (frac.type_value == Fractal_Sierpinski_Carpet) { /* Sierpinski Carpet */
		#define FRAC frac.type.sierpinski_carpet
		#undef FRAC
	}
	ImGui::End();
}

void Menu_Fractal() {
	ImGui_DefaultWindowSize(Master.resX,ImGui_WINDOW_MARGIN * 2,240,400,WindowAutoScale,Master.resY,ImGui_WINDOW_MARGIN * 2,160,320,WindowAutoScale);
	static const char* juliaBehaviour[] = {"Independant Movement","Copy Movement","Cordinates follow Z Value","Z Value follows Coordinates"};
	static bool juliaSet = false;
	static bool startingZ = false;
	static bool swapJuliaSplit = false;
	static bool cursorZValue = false;
	static bool relativeZValue = false;
	static bool showFloatingJulia = true;
	static bool adjustZoomToPower = false;
	static bool lockToCardioid = false;
	static bool flipCardioidSide = false;
	ImGui::Begin("Fractal Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition();
	static int Combo_FractalType = 0;
	ImGui::Text("Fractal Type:");
    if (ImGui::Combo("##fractalType", &Combo_FractalType, BufAndLen(FractalTypeText))) {
		switch(Combo_FractalType) {
			case Fractal_ABS_Mandelbrot:
				setDefaultParameters(&frac,Fractal_ABS_Mandelbrot);
			break;
			case Fractal_Polar_Mandelbrot:
				setDefaultParameters(&frac,Fractal_Polar_Mandelbrot);
			break;
			case Fractal_Sierpinski_Carpet:
				setDefaultParameters(&frac,Fractal_Sierpinski_Carpet);
			break;
			default:
			printError("Unknown Fractal Type: %d",Combo_FractalType);
		}
    }
	ImGui::Separator();
	if (Combo_FractalType == Fractal_ABS_Mandelbrot || Combo_FractalType == Fractal_Polar_Mandelbrot) { /* ABS and Polar */
		#define FRAC frac.type.abs_mandelbrot
		fp64 maxRadius = getABSFractalMaxRadius((Combo_FractalType == Fractal_ABS_Mandelbrot) ? (fp64)FRAC.power : FRAC.polarPower);
		fp64 minRadius = getABSFractalMinRadius((Combo_FractalType == Fractal_ABS_Mandelbrot) ? (fp64)FRAC.power : FRAC.polarPower);
		ImGui::Text("Fractal Radius: %.6lg",maxRadius);
		ImGui::Text("Cardioid Location: %.6lg",minRadius);
		if (Combo_FractalType == Fractal_ABS_Mandelbrot) {
			ImGui::Text("Fractal Power: %s",getPowerText((int32_t)FRAC.power));
			int temp_input_power = (int)FRAC.power;
			ImGui::InputInt("##temp_input_power",&temp_input_power,1,1); FRAC.power = (uint32_t)temp_input_power;
			valueLimit(FRAC.power,2,6); // Support up to Sextic
		} else {
			fp32 temp_input_polar_power = (fp32)FRAC.polarPower;
			ImGui::Text("Fractal Power: %s",getPowerText(round(FRAC.polarPower)));
			ImGui::SliderFloat("##input_polar_power",&temp_input_polar_power,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM,"%.4f"); FRAC.polarPower = (fp64)temp_input_polar_power;
			ImGui::Checkbox("Lock position to Cardioid",&FRAC.lockToCardioid);
			if (FRAC.lockToCardioid) {
				ImGui::Checkbox("Flip Cardioid position",&FRAC.flipCardioidSide);
			}
			ImGui::Checkbox("Integer Powers",&FRAC.integerPolarPower);
		}
		ImGui::Checkbox("Adjust zoom value to power",&FRAC.adjustZoomToPower);
		// fp32 temp_input_maxItr = (fp32)log2(FRAC.maxItr);
		// ImGui::Text("Maximum Iterations: %u",FRAC.maxItr);
		// ImGui::SliderFloat("##temp_super_screenshot_maxItr",&temp_input_maxItr,log2(16.0f),log2(16777216.0f),"");
		// FRAC.maxItr = (uint32_t)(pow(2.0f,temp_input_maxItr));
		// valueLimit(FRAC.maxItr,16,16777216); valueLimit(temp_input_maxItr,log2(16.0f),log2(16777216.0f));
		
		ImGui::Text(" ");
		
		fp32 temp_input_breakoutValue = (fp32)log2(FRAC.breakoutValue);
		if (FRAC.breakoutValue < 100.0) {
			ImGui::Text("Breakout Value: %.3lf",FRAC.breakoutValue);
		} else {
			ImGui::Text("Breakout Value: %.1lf",FRAC.breakoutValue);
		}
		ImGui::SliderFloat("##input_breakoutValue",&temp_input_breakoutValue,-2.0,32.0,"");
		FRAC.breakoutValue = pow(2.0,(fp64)temp_input_breakoutValue);
		ImGui::Separator();
		ImGui::Text("Julia Set Options:");
		ImGui::Checkbox("Render Julia Set",&FRAC.juliaSet);
		ImGui::Checkbox("Toggle starting Z values",&FRAC.startingZ);
		ImGui::Checkbox("Use Cursor for Z values",&FRAC.cursorZValue);
		if (FRAC.cursorZValue) { ImGui::Checkbox("Use relative Z values",&FRAC.relativeZValue); }
		ImGui::Separator();
		#ifndef BUILD_RELEASE
			static int Combo_JuliaSplit = 1;
			ImGui::Text("Split Screen:");
			if (ImGui::Combo("##juliaScreen", &Combo_JuliaSplit, BufAndLen(WindowDivider))) {
				if (Combo_JuliaSplit == 7) { /* Floating */
					FRAC.showFloatingJulia = true;
				}
			}
			if (Combo_JuliaSplit == 7) { /* Floating */
				ImGui::Checkbox("Show floating Julia Set window",&FRAC.showFloatingJulia);
			}
			ImGui::Checkbox("Swicth Mandelbrot and Julia Set",&FRAC.swapJuliaSplit);
			static int Combo_JuliaBehaviour = 0;
			ImGui::Text("Julia Set behaviour:");
			if (ImGui::Combo("##juliaBehaviour", &Combo_JuliaBehaviour, BufAndLen(juliaBehaviour))) {

			}
			ImGui::Separator();
		#endif
			/*
			fp32 temp_col = 0.0f;
			temp_col = (fp32)FRAC.rA;
			ImGui::SliderFloat("R",&temp_col,0.0,1.0); FRAC.rA = (fp64)temp_col;
			temp_col = (fp32)FRAC.gA;
			ImGui::SliderFloat("G",&temp_col,0.0,1.0); FRAC.gA = (fp64)temp_col;
			temp_col = (fp32)FRAC.bA;
			ImGui::SliderFloat("B",&temp_col,0.0,1.0); FRAC.bA = (fp64)temp_col;
			temp_col = (fp32)FRAC.iA;
			ImGui::SliderFloat("I",&temp_col,0.0,1.0); FRAC.iA = (fp64)temp_col;
			*/
			ImGui::Text("Note: Color only applies to CPU rendering.");
			ImGui::Text("Color: Red, Green, Blue, Interior");
			ImGui::Text("Phase:");
			float colP[4] = {(fp32)FRAC.rP,(fp32)FRAC.gP,(fp32)FRAC.bP,(fp32)FRAC.iP};
			ImGui::ColorEdit4("##colP",colP);
			FRAC.rP = (fp64)colP[0]; FRAC.gP = (fp64)colP[1]; FRAC.bP = (fp64)colP[2]; FRAC.iP = (fp64)colP[3];
			ImGui::Text("Amplitude:");
			float colA[4] = {(fp32)FRAC.rA,(fp32)FRAC.gA,(fp32)FRAC.bA,(fp32)FRAC.iA};
			ImGui::ColorEdit4("##colA",colA);
			FRAC.rA = (fp64)colA[0]; FRAC.gA = (fp64)colA[1]; FRAC.bA = (fp64)colA[2]; FRAC.iA = (fp64)colA[3];
			ImGui::Text("Frequency:");
			float colF[4] = {(fp32)FRAC.rF,(fp32)FRAC.gF,(fp32)FRAC.bF,(fp32)FRAC.iF};
			ImGui::ColorEdit4("##colF",colF);
			FRAC.rF = (fp64)colF[0]; FRAC.gF = (fp64)colF[1]; FRAC.bF = (fp64)colF[2]; FRAC.iF = (fp64)colF[3];
		#undef FRAC
	} else if (Combo_FractalType == Fractal_Sierpinski_Carpet) { /* Sierpinski Carpet */
		#define FRAC frac.type.sierpinski_carpet
		static bool wallisSieve = false;
		static bool renderOutOfBounds = false;
		static bool fixateOnCorner = false;
		static fp64 squareSize = 1.0; fp32 temp_squareSize = (fp32)FRAC.squareSize;
		ImGui::Checkbox("Wallis Sieve",&FRAC.wallisSieve);
		ImGui::Checkbox("Render out of bounds",&FRAC.renderOutOfBounds);
		ImGui::Checkbox("Fixate on top-left corner",&FRAC.fixateOnCorner);
		ImGui::Text("Square Size Multiplier:");
		ImGui::SliderFloat("##input_squareSize",&temp_squareSize,1.0e-4,1.0,"%.4f"); FRAC.squareSize = (fp64)temp_squareSize;
		#undef FRAC
	}

	ImGui::End();
}

void Menu_Rendering() {
	ImGui_DefaultWindowSize(Master.resX,ImGui_WINDOW_MARGIN * 2,240,400,WindowAutoScale,Master.resY,ImGui_WINDOW_MARGIN * 2,160,320,WindowAutoScale);
	static const char* CPU_RenderingModes[] = {"fp32 | 10^5.7","fp64 | 10^14.4 (Default)","fp80 | 10^17.7","fp128 | 10^32.5"};
	#ifndef BUILD_RELEASE
		static const char* GPU_RenderingModes[] = {"fp16 | 10^1.8","fp32 | 10^5.7 (Default)","fp64 | 10^14.4"};
		static int Combo_GPU_RenderingMode = 1;
	#else
		static const char* GPU_RenderingModes[] = {"fp32 | 10^5.7 (Default)"};
		static int Combo_GPU_RenderingMode = 0;
	#endif
	static int input_subSample = primaryRenderData.subSample;
	static int input_superSample = primaryRenderData.sample;
	int CPU_ThreadCount = (int)std::thread::hardware_concurrency();
	static int input_CPU_MaxThreads = ((CPU_ThreadCount <= 1) ? 1 : (CPU_ThreadCount - 1));
	static int input_CPU_ThreadMultiplier = 1;
	static int Combo_CPU_RenderingMode = 1;

	static int input_super_CPU_MaxThreads = super_screenshot_maxThreads;
	static int input_super_CPU_ThreadMultiplier = super_screenshot_threadMultiplier;
	

	ImGui::Begin("Rendering Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition();

	ImGui::Text("CPU Rendering Mode:");
	if (ImGui::Combo("##CPU_RenderingMode", &Combo_CPU_RenderingMode, BufAndLen(CPU_RenderingModes))) {
		switch (Combo_CPU_RenderingMode) {
			case 0:
				primaryRenderData.CPU_Precision = 32;
			break;
			case 1:
				primaryRenderData.CPU_Precision = 64;
			break;
			case 2:
				primaryRenderData.CPU_Precision = 80;
			break;
			case 3:
				primaryRenderData.CPU_Precision = 128;
			break;
		};
	}
	if (ImGui::CollapsingHeader("CPU MULTI-THREADING SETTINGS")) {
		ImGui::Text("Note: Only modify these settings if you know what you are doing.");
		ImGui::Text("Maximum Threads:");
		ImGui::SliderInt("##input_CPU_MaxThreads",&input_CPU_MaxThreads,1,CPU_ThreadCount);
		ImGui::Text("Thread Multiplier:");
		ImGui::SliderInt("##input_CPU_ThreadMultiplier",&input_CPU_ThreadMultiplier,1,16);
		ImGui::Text(" ");
		ImGui::Text("Super Screenshot:");
		ImGui::Text("Maximum Threads:");
		ImGui::SliderInt("##input_CPU_MaxThreads",&input_super_CPU_MaxThreads,1,CPU_ThreadCount);
		ImGui::Text("Thread Multiplier:");
		ImGui::SliderInt("##input_CPU_ThreadMultiplier",&input_super_CPU_ThreadMultiplier,1,16);
		ImGui::Text(" ");
	}
	primaryRenderData.CPU_Threads = input_CPU_MaxThreads * input_CPU_ThreadMultiplier;
	super_screenshot_threadMultiplier = input_CPU_ThreadMultiplier;
	super_screenshot_maxThreads = input_CPU_MaxThreads;
	ImGui::Separator();

	ImGui::Text("GPU Rendering Mode:");
	if (ImGui::Combo("##GPU_RenderingMode", &Combo_GPU_RenderingMode, BufAndLen(GPU_RenderingModes))) {
		switch (Combo_GPU_RenderingMode) {
			#ifndef BUILD_RELEASE
				case 0:
					primaryRenderData.GPU_Precision = 16;
				break;
			#endif
			case 1:
				primaryRenderData.GPU_Precision = 32;
			break;
			#ifndef BUILD_RELEASE
				case 2:
					primaryRenderData.GPU_Precision = 64;
				break;
			#endif
		};
	}

	#ifndef BUILD_RELEASE
		if (ImGui::CollapsingHeader("GPU ADVANCED SETTINGS")) {
			ImGui::Text("Note: Only modify these settings if you know what you are doing.");
			ImGui::Text("GPU Render Partitions: (Default = 1)");
			ImGui::InputInt("##input_CPU_MaxThreads",(int32_t*)(&primaryRenderData.GPU_Partitions),1,16);
			valueLimit(primaryRenderData.GPU_Partitions,1,1024);
			ImGui::TextWrapped(
				"Increasing the amount of partitions can reduce the time it takes for the GPU to quit rendering when the Abort Rendering button is pressed. "\
				"However, increasing the rendering paritions can cause some performance loss due to the overhead of rendering smaller chunks of the fractal at a time. "\
				"For the best performance, set render paritions to 1."
			);
			ImGui::Text(" ");
		}
	#endif

	ImGui::Separator();

	ImGui::Text("Sub Sample: %d",input_subSample * input_subSample);
	ImGui::SliderInt("##input_subSample",&input_subSample,1,24,"");
	primaryRenderData.subSample = input_subSample;
	ImGui::Text("Samples per pixel: %d",input_superSample * input_superSample);
	ImGui::SliderInt("##input_superSample",&input_superSample,1,24,"");
	uint32_t totalResX = primaryRenderData.resX * primaryRenderData.sample / primaryRenderData.subSample;
	uint32_t totalResY = primaryRenderData.resY * primaryRenderData.sample / primaryRenderData.subSample;
	ImGui::Text("Total Pixels Rendered: %ux%u %.3lfMP",totalResX,totalResY,(fp64)(totalResX * totalResY) / 1000000.0);
	primaryRenderData.sample = input_superSample;
	ImGui::Separator();
	static const char* OpenCV_interpolation_mode_list[] = {"Nearest Neighbor (Default)","Linear","Cubic","Lanczos"};
	static int OpenCV_interpolation_mode = 0;
	ImGui::Text("Frame Interpolation Method:");
	if (ImGui::Combo("##Frame_Interpolation_Method", &OpenCV_interpolation_mode, BufAndLen(OpenCV_interpolation_mode_list))) {
		switch (OpenCV_interpolation_mode) {
			case 0:
				Frame_Interpolation_Method = cv::INTER_NEAREST;
			break;
			case 1:
				Frame_Interpolation_Method = cv::INTER_LINEAR;
			break;
			case 2:
				Frame_Interpolation_Method = cv::INTER_CUBIC;
			break;
			case 3:
				Frame_Interpolation_Method = cv::INTER_LANCZOS4;
			break;
			default:
				Frame_Interpolation_Method = cv::INTER_NEAREST;
		};
	}
	ImGui::Text("Nearest Neighbor is the fastest method. Other methods might not be able to hit 60.0fps at higher resolutions.");

	ImGui::End();
}

void Menu_Settings() {
	static const char* initFrameRate[] = {
		"Current Monitor","Highest Refresh-Rate","Lowest Refresh-Rate","Constant Value"
	};
	static int Combo_initFrameRate = 0;
	static const char* initMonitorLocations[] = {
		"Automatic","First Monitor","Last Monitor","Specific Monitor",
		"Left","Right","Center","Top","Bottom","Top-Left","Top-Right","Bottom-Left","Bottom-Right",
		"Highest Resolution","Highest Framerate","Lowest Resolution","Lowest Framerate"
	};
	static int Combo_initMonitorLocation = 0;
	static int specificMonitor = SPECIFIC_BOOTUP_DISPLAY;

	#define printDisplayInfo(displayNumber); \
	{ \
		DisplayInfo* DispI = getDisplayInfo(displayNumber); \
		if (DispI == NULL) { \
			ImGui::Text("Display %d is not detected",displayNumber); \
		} else { \
			ImGui::Text("Display[%d]: %ux%u at %uHz (%d,%d) | %s",displayNumber,DispI->resX,DispI->resY,DispI->refreshRate,DispI->posX,DispI->posY,DispI->name); \
		} \
	}
	ImGui_DefaultWindowSize(Master.resX,ImGui_WINDOW_MARGIN * 2,240,400,WindowAutoScale,Master.resY,ImGui_WINDOW_MARGIN * 2,160,320,WindowAutoScale);
	ImGui::Begin("Settings Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition();
	ImGui::Text("ABS-Fractal-Explorer v%s", PROGRAM_VERSION);
	ImGui::Separator();
	ImGui::Checkbox("Lock key inputs in menus",&LockKeyInputsInMenus);
	ImGui::Separator();
	if (ImGui::CollapsingHeader("MENU WINDOW SETTINGS")) {
		ImGui::Checkbox("Prevent out of bounds menu windows",&PreventOutOfBoundsWindows);
		ImGui::Checkbox("Auto-resize menu windows",&AutoResizeWindows);
		ImGui::Text("Window Auto-Scale: (0.7 default)");
		fp32 temp_WindowAutoScale = (fp32)WindowAutoScale;
		ImGui::SliderFloat("##WindowAutoScale",&temp_WindowAutoScale,0.3f,1.0f,"%.3f");
		WindowAutoScale = (fp64)temp_WindowAutoScale;
		ImGui::Text("Window Opacity:");
		ImGui::SliderFloat("##WindowOpacity",&WindowOpacity,0.3f,1.0f,"%.3f");
		ImGui::Text(" ");
		

		static const char* GUI_Theme_Options[] = {
			"Classic","Dark-mode (Default)","Light-mode (Unsupported)"
		};
		ImGui::Text("GUI Theme:");
		if (ImGui::Combo("##IMGUI_Theme",&IMGUI_Theme,BufAndLen(GUI_Theme_Options))) {
			switch(IMGUI_Theme) {
				case 0:
					ImGui::StyleColorsClassic();
				break;
				case 1:
					ImGui::StyleColorsDark();
				break;
				case 2:
					ImGui::StyleColorsLight();
				break;
				default:
					ImGui::StyleColorsDark();
			};
		}
		ImGui::Text(" ");
	}
	if (ImGui::CollapsingHeader("DISPLAY AND FRAME-RATE")) {
		ImGui::Text("Base maximum frame-rate off of:");
		if (ImGui::Combo("##initFrameRate", &Combo_initFrameRate, BufAndLen(initFrameRate))) {
		
		}
		uint32_t SELECT_DISPLAY = CURRENT_DISPLAY;
		switch(Combo_initFrameRate) {
			case 0:
				SELECT_DISPLAY = CURRENT_DISPLAY;
			break;
			case 1:
				SELECT_DISPLAY = Display_Match[Display_Bootup::HighFrameRate];
			break;
			case 2:
				SELECT_DISPLAY = Display_Match[Display_Bootup::LowFrameRate];
			break;
		}
		printDisplayInfo(SELECT_DISPLAY);

		//DisplayInfo* disp = getCurrentDisplayInfo();
		DisplayInfo* disp = getDisplayInfo(SELECT_DISPLAY);
		static fp64 FPS_Constant_Value = (disp == NULL) ? 60.0 : (fp64)(disp->refreshRate);
		fp64 TEMP_FPS = (disp == NULL) ? 60.0 : (fp32)(disp->refreshRate); // Would normally be either the current, highest, or lowest refresh rate
		if (Combo_initFrameRate == 3) { // Constant
			static fp32 temp_FPS_Constant_Value = (disp == NULL) ? 60.0 : (fp32)(disp->refreshRate);
			ImGui::Text("%.3lfms",(1.0 / FPS_Constant_Value) * 1000.0);
			ImGui::Text(" ");
			ImGui::InputFloat("##temp_FPS_Constant_Value",&temp_FPS_Constant_Value,6.0,30.0,"%.3f"); valueLimit(temp_FPS_Constant_Value,6.0,1200.0);
			FPS_Constant_Value = (fp64)temp_FPS_Constant_Value;
			if (ImGui::Button("Apply FPS")) {
				updateFrameRate(FPS_Constant_Value + FRAME_RATE_OFFSET);
			}
		} else { // Relative
			ImGui::Text(" "); // Blank Line
			static int temp_frameMultiplier = (Default_Frame_Rate_Multiplier >= 0.0) ? (int)(Default_Frame_Rate_Multiplier - 1.0) : (int)(1.0 - 1.0);
			static fp64 frameMultiplier = Default_Frame_Rate_Multiplier;
			if (temp_frameMultiplier >= 0) {
				frameMultiplier = (fp64)(temp_frameMultiplier + 1);
				ImGui::Text("Maximum FPS Multiplier: %dx",(temp_frameMultiplier + 1));
			} else {
				frameMultiplier = 1.0 / (fp64)(1 - temp_frameMultiplier);
				ImGui::Text("Maximum FPS Multiplier: 1/%dx", (1 - temp_frameMultiplier));
			}
			fp64 calculatedFPS = frameMultiplier * TEMP_FPS;
			valueLimit(calculatedFPS,6.0,1200.0);
			ImGui::Text("%.2lffps %.2lfms", calculatedFPS, (1.0 / (calculatedFPS)) * 1000.0);
			ImGui::SliderInt("##temp_frameMultiplier",&temp_frameMultiplier,-6 + 1,6 - 1,"");
			if (ImGui::Button("Apply FPS")) {
				updateFrameRate(calculatedFPS + FRAME_RATE_OFFSET);
			}
		}

		#ifndef BUILD_RELEASE
			ImGui::Text(" "); ImGui::Separator(); ImGui::Text(" ");
			ImGui::Text("Which monitor should the application open to:");
			if (ImGui::Combo("##initMonitorLocation", &Combo_initMonitorLocation, BufAndLen(initMonitorLocations))) {
				
			}
			if (Combo_initMonitorLocation == 3) { // Specific Monitor
				static bool overrideDisplayCount = false;
				int limitDisplayCount = (overrideDisplayCount == true) ? (DISPLAY_COUNT + 8) : DISPLAY_COUNT;
				if (overrideDisplayCount == false && specificMonitor > (int)DISPLAY_COUNT) {
					specificMonitor = DISPLAY_COUNT;
				}
				if (DISPLAY_COUNT != 1 || overrideDisplayCount == true) {
					ImGui::InputInt("##specificMonitor",&specificMonitor,1,1); valueLimit(specificMonitor,1,limitDisplayCount);
				} else {
					ImGui::Text("Only 1 display detected");
				}
				printDisplayInfo(specificMonitor); 
				ImGui::Checkbox("Override Display Count",&overrideDisplayCount);
				if (specificMonitor > (int)DISPLAY_COUNT) {
					ImGui::Text("Note: Display %d will be used if Display %d is not detected",DISPLAY_COUNT,specificMonitor);
				}
			} else {
				printDisplayInfo(Display_Match[Combo_initMonitorLocation]);
			}
		#endif
		ImGui::Text(" ");
	}
	if (ImGui::CollapsingHeader("FRACEXP FILES")) {
		ImGui::Checkbox("Save username in files",&SaveUsernameInFiles);
		ImGui::Checkbox("Save hardware information in files",&SaveHardwareInfoInFiles);
		if (SaveUsernameInFiles == true) {
			correctUsernameText(FileUsername,FileUsernameLength);
			#ifdef displayTerribleProgrammingJokes
				//idk why I wrote this
				ImGui::TextWrapped("Input Username: The Username MUST be a valid C variable name exclusively using the limited subset of 63 characters of the first 128 8-bit ANSI characters in addition to demonstrating an unwavering adherence and strict compliance to the standards outlined in ISO/IEC 9899:1999 for C99 with a NULL terminated char array that MUST NOT exceed 32 characters long including the NULL terminator and MUST be an absolute minimum of 4 characters long");
			#else
				ImGui::Text("Input Username: 4-31 characters long, A-Z, a-z, 0-9, and _");
			#endif
			ImGui::InputText("##FileUserName_Input",FileUsername,FileUsernameLength);
		}
		ImGui::Text(" ");
	}
	if (ImGui::CollapsingHeader("SCREEN-SHOTS")) {
		static int Combo_ScreenshotFileType = screenshotFileType;
		static const char* Text_ScreenshotFileType[] = {"PNG","JPG/JPEG","TGA","BMP"};
		ImGui::Text("Screenshot File Type:");
		if (ImGui::Combo("##Combo_ScreenshotFileType",&Combo_ScreenshotFileType,BufAndLen(Text_ScreenshotFileType))) {
			screenshotFileType = (Image_File_Format::Image_File_Format_Enum)Combo_ScreenshotFileType;
		}
		if (screenshotFileType == Image_File_Format::PNG) {
			int temp_User_PNG_Compression_Level = User_PNG_Compression_Level;
			ImGui::Text("PNG Compression Level (Default = 8)");
			ImGui::SliderInt("##temp_User_PNG_Compression_Level",&temp_User_PNG_Compression_Level,1,9);
			User_PNG_Compression_Level = (uint32_t)temp_User_PNG_Compression_Level;
			if (User_PNG_Compression_Level < 3) { ImGui::Text("Fastest Saving (Large File Size)"); } else
			if (User_PNG_Compression_Level < 5) { ImGui::Text("Faster Saving"); } else
			if (User_PNG_Compression_Level < 7) { ImGui::Text("Balanced"); } else
			if (User_PNG_Compression_Level < 9) { ImGui::Text("Smaller File Size (Recommended)"); } else
			{ ImGui::Text("Smallest File Size"); }
		} else if (screenshotFileType == Image_File_Format::JPG) {
			int temp_User_JPG_Quality_Level = User_JPG_Quality_Level;
			ImGui::Text("JPG/JPEG Quality Level (Default = 95)");
			ImGui::SliderInt("##temp_User_JPG_Quality_Level",&temp_User_JPG_Quality_Level,25,100);
			User_JPG_Quality_Level = (uint32_t)temp_User_JPG_Quality_Level;
			if (User_JPG_Quality_Level < 50) { ImGui::Text("Low Quality"); } else
			if (User_JPG_Quality_Level < 80) { ImGui::Text("Medium Quality"); } else
			if (User_JPG_Quality_Level < 90) { ImGui::Text("High Quality"); } else
			{ ImGui::Text("Very High Quality (Recommended)"); }
		} else if (screenshotFileType == Image_File_Format::TGA || screenshotFileType == Image_File_Format::BMP) {
			ImGui::Text("Note: Super Screenshots only support PNG and JPG.");
		}

		ImGui::Text(" "); ImGui::Separator(); ImGui::Text(" ");
		ImGui::Text("Super Screenshot Settings:");
		ImGui::Text(" ");
		
		static fp32 temp_super_screenshot_maxItr = log2((fp32)default_Super_Screenshot_MaxItr);
		ImGui::Text("Maximum Iterations: %d",super_screenshot_maxItr);
		ImGui::SliderFloat("##temp_super_screenshot_maxItr",&temp_super_screenshot_maxItr,log2(16.0f),log2(16777216.0f),"");
		super_screenshot_maxItr = (int32_t)(pow(2.0f,temp_super_screenshot_maxItr));
		valueLimit(super_screenshot_maxItr,16,16777216); valueLimit(temp_super_screenshot_maxItr,log2(16.0f),log2(16777216.0f));

		const uint64_t MaximumImageSize = (uint64_t)2147000000; // INT32_MAX minus some arbritrary overhead amount

		ImGui::Text("Samples per pixel: %d",super_screenshot_super_sample * super_screenshot_super_sample);
		ImGui::SliderInt("##super_screenshot_super_sample",&super_screenshot_super_sample,1,32,"");
		size_t totalResX = (size_t)super_screenshot_resX * (size_t)super_screenshot_super_sample;
		size_t totalResY = (size_t)super_screenshot_resY * (size_t)super_screenshot_super_sample;

		ImGui::Text(" ");
		static int Combo_Common_ResolutionPreset = 3;
		const uint32_t Combo_Common_ResolutionPreset_RESX[] = {640,1280,1366,1920,2560,3840,5120,7680};
		const uint32_t Combo_Common_ResolutionPreset_RESY[] = {480, 720, 768,1080,1440,2160,2880,4320};
		static const char* Common_ResolutionPreset[] = {
			"640x480 SD","1280x720 HD","1366x768 WXGA","1920x1080 FHD","2560x1440 QHD","3840x2160 4K","5120x2880 5K","7680x4320 8K"
		};
		ImGui::Text("Resolution Presets:");
		if (ImGui::Combo("##Common_Resolutions",&Combo_Common_ResolutionPreset,BufAndLen(Common_ResolutionPreset))) {
			super_screenshot_resX = Combo_Common_ResolutionPreset_RESX[Combo_Common_ResolutionPreset];
			super_screenshot_resY = Combo_Common_ResolutionPreset_RESY[Combo_Common_ResolutionPreset];
		}

		ImGui::Text("Resolution X:");
		ImGui::InputInt("##super_screenshot_resX",&super_screenshot_resX,16,64);
		super_screenshot_resX &= 0x7FFFFFFC; // Multiple of 4
		valueLimit(super_screenshot_resX,64,65536); valueMaximum(super_screenshot_resX,(int32_t)MaximumImageSize / super_screenshot_resY / 3);
		ImGui::Text("Resolution Y:");
		ImGui::InputInt("##super_screenshot_resY",&super_screenshot_resY,16,64);
		valueLimit(super_screenshot_resY,64,65536); valueMaximum(super_screenshot_resY,(int32_t)MaximumImageSize / super_screenshot_resX / 3);
		
		ImGui::Text(" ");
		ImGui::Text("Total Pixels Rendered: %zux%zu %.3lfMP",totalResX,totalResY,(fp64)(totalResX * totalResY) / 1000000.0);
		if ((uint64_t)super_screenshot_resX * (uint64_t)super_screenshot_resY * IMAGE_BUFFER_CHANNELS >= 1000000000) {
			ImGui::Text("Current Image Size: %.1lf megabytes",
				(fp64)((uint64_t)super_screenshot_resX * (uint64_t)super_screenshot_resY * IMAGE_BUFFER_CHANNELS) / 1000000.0
			);
			ImGui::Text("Maximum Image Size: %.1lf megabytes",
				(fp64)(MaximumImageSize) / 1000000.0
			);
		}
		ImGui::Text(" ");
		if (ImGui::Button("Take Super Screenshot")) {
			exportSuperScreenshot();
			//exportScreenshot();
		}
		ImGui::Text(" ");
	}
	ImGui::End();
}

void Menu_Keybinds() {
	static int Combo_keyboardSize = 0;
	static const char* keyboardSizeText[] = {
		"ANSI (Default)","Extended (Contains some FN keys)","Complete (All 242 SDL Scancodes)"
	};
	static bool displayNumpad = true;
	ImGui_DefaultWindowSize(Master.resX,ImGui_WINDOW_MARGIN * 2,320,480,WindowAutoScale,Master.resY,ImGui_WINDOW_MARGIN * 2,240,360,WindowAutoScale);
	ImGui::Begin("Keybinds Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition();
	ImGui::Checkbox("Lock key inputs in menus",&LockKeyInputsInMenus);
	ImGui::Text("Keyboard Type:");
	if (ImGui::Combo("##keyboardSize", &Combo_keyboardSize, BufAndLen(keyboardSizeText))) {
		
	}
	ImGui::Checkbox("Display Numberpad",&displayNumpad);
	if (Combo_keyboardSize != 0) {
		ImGui::Text("Note: Some Scancodes may or may not trigger on modern Keyboards");
	}
	ImGui::Text(" ");
	{
		#define kMaxResX 1440
		#define kMinResX 300
		#define kMinResY 140
		
		//static uint32_t kX = kMargin;
		//static uint32_t kY = 0;
		uint32_t kResX = ImGui::GetWindowContentRegionWidth();
		if (kResX < kMinResX) {
			kResX = kMinResX;
		} else if (kResX > kMaxResX) {
			kResX = kMaxResX;
		}
		static int32_t kCurX = -1;
		static int32_t kCurY = -1;
		static uint32_t clickState = 0x0;
		static BufferBox kBuf; // Gets set when the keyboard is rendered

		ImVec2 CursorPos = {
			ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
			ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - 17 // Magic Correction Amount
		};
		clickState = SDL_GetMouseState(NULL,NULL);
		static SDL_Scancode keyHover = SDL_SCANCODE_UNKNOWN;
		static SDL_Scancode keyClick = SDL_SCANCODE_UNKNOWN;
		bool hoverInBounds = false;
		renderKeyboard(
			&kBuf, kResX, 5.75, 12.0,
			(uint8_t)Combo_keyboardSize, displayNumpad,
			kCurX, kCurY, ((clickState & 0x1) ? true : false), &keyHover, &hoverInBounds
		);
		SDL_Surface* kSurface = SDL_CreateRGBSurfaceWithFormatFrom(kBuf.vram, kBuf.resX, kBuf.resY, IMAGE_BUFFER_CHANNELS * 8, IMAGE_BUFFER_CHANNELS * kBuf.resX, SDL_PIXELFORMAT_ABGR8888);
		if (kSurface == nullptr) {
			fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
		}
		kTexture = SDL_CreateTextureFromSurface(renderer, kSurface);
		if (kTexture == nullptr) {
			fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
		}
		SDL_FreeSurface(kSurface);
		kCurX = CursorPos.x;
		kCurY = CursorPos.y;

		ImGui::Text("Hover: %s",Scancode_Name[keyHover]);
		if (clickState & 0x1 && hoverInBounds == true) {
			keyClick = keyHover;
		}
		ImGui::Image((void*)kTexture, ImVec2(kBuf.resX, kBuf.resY));
		// ImGui::Text("size = %d x %d", kBuf.resX, kBuf.resY);
		// ImGui::Text("Cursor Position: %d,%d",kCurX,kCurY);
		ImGui::Text("Clicked Key: %s",Scancode_Name[keyClick]);
		size_t funcCount = 0;
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			using namespace Key_Function;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == keyClick) {
					funcCount++;
				}
			}
			if (funcCount > 0) {
				//ImGui::Text("Key Functions:");
				for (const auto& bind : currentKBPreset->kList) {
					if (bind.key == keyClick) {
						ImGui::Text("- %s",Key_Function_Text[bind.func]);
					}
				}
			} else {
				ImGui::Text("- Key not bound to any functions");
			}
		} else {
			ImGui::Text(" ");
		}

		ImGui::Separator();

		static int Combo_functionSelect;
		ImGui::Text("Select Function to bind:");
		if (ImGui::Combo("##Combo_functionSelect",&Combo_functionSelect,BufAndLen(Key_Function::Key_Function_Text))) {
			if (Combo_functionSelect == Key_Function::Parameter_Function_Count) {
				Combo_functionSelect = Key_Function::NONE;
			} else {
				for (size_t i = 0; i < ARRAY_LENGTH(Key_Function::Key_Function_Map); i++) {
					if (Combo_functionSelect == (int)Key_Function::Key_Function_Map[i]) {
						Combo_functionSelect = Key_Function::NONE;
					}
				}
			}
		}
		
		if (Combo_functionSelect != Key_Function::NONE && keyClick != SDL_SCANCODE_UNKNOWN) {
			//ImGui::Text("Bind key %s to function %s",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);

			ImGui::Text("Bind key "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored({0.0,1.0,1.0,1.0},"%s",Scancode_Name[keyClick]); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" to function "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored({0.0,1.0,1.0,1.0},"%s",Key_Function::Key_Function_Text[Combo_functionSelect]);

			if(ImGui::Button("Set Key-Bind")) {
				if (addKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_functionSelect,keyClick) >= 0) {
					recolorKeyboard();
					Combo_functionSelect = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					printError("addKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);
				}
			}
			if (funcCount != 0) {
				ImGui::SameLine();
				if(ImGui::Button("Clear Key-bind")) {
					if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_functionSelect,keyClick) >= 0) {
						recolorKeyboard();
						Combo_functionSelect = Key_Function::NONE;
					} else {
						printError("removeKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);
					}
				}
			}
		} else {
			ImGui::Text("Click on a Key and Select a Function");
			ImGui::Text(" ");
		}
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			if(ImGui::Button("Clear Key bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,keyClick) >= 0) {
					recolorKeyboard();
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					printError("removeKeyBind(%s) failed",Scancode_Name[keyClick]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Scancode_Name[keyClick]);
		} else {
			ImGui::Button("Click on a Key");
		}
		if (Combo_functionSelect != Key_Function::NONE) {
			if(ImGui::Button("Clear Function bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_functionSelect) >= 0) {
					recolorKeyboard();
				} else {
					printError("removeKeyBind(%s) failed",Key_Function::Key_Function_Text[Combo_functionSelect]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Key_Function::Key_Function_Text[Combo_functionSelect]);
		} else {
			ImGui::Button("Select a Function");
		}
		ImGui::Text(" ");
		if (ImGui::Button("Reset current key-bind to defaults")) {
			Combo_functionSelect = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			currentKBPreset->kList = defaultKeyBind;
			recolorKeyboard();
		}
		ImGui::Text(" ");

		ImGui::Separator();

		static char KeyBindName[80];
		memset(KeyBindName,'\0',ARRAY_LENGTH(KeyBindName));
		memcpy(KeyBindName,currentKBPreset->name.c_str(),TEXT_LENGTH(KeyBindName));
		if (currentKBPreset->kList.size() < 6) {
			ImGui::Text("Warning: The current Key-bind Preset has %llu key-binds, and may not be functional or practical.",currentKBPreset->kList.size());
			ImGui::Text("Current Key-bind Preset[%d]: ",get_currentKBPreset_Pos()); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored({1.0,0.5,0.5,1.0},"%llu",currentKBPreset->kList.size()); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" key-binds");
		} else {
			ImGui::Text("Current Key-bind Preset[%d]: %llu key-binds",get_currentKBPreset_Pos(),currentKBPreset->kList.size());
		}

		ImGui::InputText("##KeyBindName",BufAndLen(KeyBindName));
		currentKBPreset->name = KeyBindName;
		if (ImGui::BeginCombo("##Combo_KeyBind", "Choose a key-bind")) {
			for (auto iterKB = KeyBind_PresetList.begin(); iterKB != KeyBind_PresetList.end(); iterKB++) {
				bool is_selected = (currentKBPreset == &(*iterKB));
				if (ImGui::Selectable(iterKB->name.c_str(), is_selected)) {
					Combo_functionSelect = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
					currentKBPreset = &(*iterKB);
					recolorKeyboard();
				}
				// Set the initial focus on the currently selected item
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text(" ");
		static uint32_t name_count = 1;
		if (ImGui::Button("Create key-bind")) {
			Combo_functionSelect = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			KeyBind_Preset temp_KeyBind;
			temp_KeyBind = *currentKBPreset;
			static char rand_name[324]; memset(rand_name,'\0',324);
			snprintf(rand_name,320,"KeyBind_%u",name_count++);
			temp_KeyBind.name = rand_name;
			KeyBind_PresetList.push_back(temp_KeyBind);
			currentKBPreset = &KeyBind_PresetList.back();
			recolorKeyboard();
		}
		if (KeyBind_PresetList.size() > 1 && ImGui::Button("Remove current key-bind preset")) {
			Combo_functionSelect = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			remove_currentKBPreset();
			recolorKeyboard();
		}
		if (ImGui::Button("Clear all key-binds")) {
			Combo_functionSelect = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			clearKeyBind(&currentKBPreset->kList);
			recolorKeyboard();
		}

		ImGui::Text(" ");
		if (ImGui::Button("Import Key-bind (.FracExpKB)")) {
			static char importKeyBindFile[324]; memset(importKeyBindFile,'\0',324);
			int openFileState = openFileInterface(
				importKeyBindFile,324,"Select a FracExpKB file",
				"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
				"FracExp Files (*.fracExp)\0*.fracExp\0"\
				"All Files (*.*)\0*.*\0"
			);
			if (openFileState == 0) {
				Combo_functionSelect = Key_Function::NONE;
				keyClick = SDL_SCANCODE_UNKNOWN;
				import_KeyBindPresets(&KeyBind_PresetList,&currentKBPreset,importKeyBindFile);
				recolorKeyboard();
			}
		}
		if (ImGui::Button("Export Current Key-bind (.FracExpKB)")) {
			static char exportKeyBindFile[324]; memset(exportKeyBindFile,'\0',324);
			int saveFileState = saveFileInterface(
				exportKeyBindFile,324,"Save FracExpKB file",
				"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
				"FracExp Files (*.fracExp)\0*.fracExp\0"\
				"All Files (*.*)\0*.*\0",
				"fracExpKB",
				currentKBPreset->name.c_str()
			);
			if (saveFileState == 0) {
				KeyBind_Preset temp_KeyBind = *currentKBPreset;
				export_KeyBind(&temp_KeyBind,exportKeyBindFile);
			}
		}
		#ifndef BUILD_RELEASE
			if (ImGui::Button("Export All Key-binds (.FracExpKB)")) {
				static char exportKeyBindFile[324]; memset(exportKeyBindFile,'\0',324);
				int saveFileState = saveFileInterface(
					exportKeyBindFile,324,"Save FracExpKB file",
					"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
					"FracExp Files (*.fracExp)\0*.fracExp\0"\
					"All Files (*.*)\0*.*\0",
					"fracExpKB",
					currentKBPreset->name.c_str()
				);
				if (saveFileState == 0) {
					export_KeyBindPresets(&KeyBind_PresetList,exportKeyBindFile);
				}
			}
		#endif
		// //	Disabling the name field since it can cause confusion when typing "./folder" + "KeyBind.fracExpKB" = "./folderKeyBind.fracExpKB"
		// // static char exportFracExpKBName[324] = "KeyBind";
		// static char exportFracExpKBDir[324] = "./KeyBind"; // "./"
		// ImGui::Text("Export current key-bind to directory:");
		// ImGui::InputText("##exportFracExpKBDir",exportFracExpKBDir,TEXT_LENGTH(exportFracExpKBDir));
		// // ImGui::Text("File Name:");
		// // ImGui::InputText("##exportFracExpKBName",exportFracExpKBName,TEXT_LENGTH(exportFracExpKBName));
		// static TimerBox exportTimer(1.0);
		// if (ImGui::Button("Export .FracExpKB File") && exportTimer.timerReset()) {
		// 	/* Move this file path code into fracExpKB.cpp */
		// 	const char* const exportFracExpKBExtension = ".fracExpKB";
		// 	size_t sizeOfPath = 0;
		// 	sizeOfPath += strnlen(exportFracExpKBExtension,sizeof(exportFracExpKBExtension)) * sizeof(exportFracExpKBExtension[0]);
		// 	sizeOfPath += strnlen(exportFracExpKBDir,sizeof(exportFracExpKBDir)) * sizeof(exportFracExpKBDir[0]);
		// 	// sizeOfPath += strnlen(exportFracExpKBName,sizeof(exportFracExpKBName)) * sizeof(exportFracExpKBName[0]);
		// 	char* exportFracExpKBPath = (char*)malloc(sizeOfPath);
		// 	if (exportFracExpKBPath != NULL) {
		// 		memset(exportFracExpKBPath,'\0',sizeOfPath);
		// 		strncat(exportFracExpKBPath,exportFracExpKBDir,sizeOfPath);
		// 		// strncat(exportFracExpKBPath,exportFracExpKBName,sizeOfPath);
		// 		strncat(exportFracExpKBPath,exportFracExpKBExtension,sizeOfPath);
		// 		if (write_FracExpKB_File(&currentKeyBind,exportFracExpKBPath) >= 0) {
		// 			printFlush("\n%s was exported successfully",exportFracExpKBPath);
		// 		} else {
		// 			printError("%s failed to export",exportFracExpKBPath);
		// 		}
		// 	} else {
		// 		printError("Unable to allocate memory to char* exportFracExpKBPath");
		// 	}
		// 	FREE(exportFracExpKBPath);
		// }
	}
	ImGui::Text(" ");
	ImGui::Separator();
	ImGui::Text(" ");
	ImGui::Text("Movement Sensitivity:");
	#define sen_slider(lbl,num,min,max) \
		{ \
			fp32 temp_sensitivity_float_slider = (fp32)num; \
			ImGui::SliderFloat(lbl,&temp_sensitivity_float_slider,min,max,"%.2f"); \
			num = (fp64)temp_sensitivity_float_slider; \
		}
	ImGui::Text(" ");
	ImGui::Text("Global Sensitivity Multiplier:");
	sen_slider("##sen_global",user_sensitivity.global,0.4,2.5);
	if (ImGui::Button("Reset Sensitvity")) {
		set_default_sensitivity(&user_sensitivity);
	}
	ImGui::Text(" ");
	ImGui::Text("Coordinate:");
	sen_slider("##sen_coordinate",user_sensitivity.coordinate,0.4,2.5);
	ImGui::Text("Zoom:");
	sen_slider("##sen_zoom",user_sensitivity.zoom,0.4,2.5);
	ImGui::Checkbox("Invert Zoom",&user_sensitivity.invert_zoom);
	ImGui::Text("Maximum Iterations:");
	sen_slider("##sen_maxIter",user_sensitivity.maxIter,0.4,2.5);
	ImGui::Text("Z-Value/Julia:");
	sen_slider("##sen_julia",user_sensitivity.julia,0.4,2.5);
	ImGui::Text("Rotation:");
	sen_slider("##sen_rotation",user_sensitivity.rotation,0.4,2.5);
	ImGui::Text("Stretch:");
	sen_slider("##sen_stretch",user_sensitivity.stretch,0.4,2.5);
	ImGui::Text("Polar Power:");
	sen_slider("##sen_polar_power",user_sensitivity.polar_power,0.4,2.5);
	ImGui::Text("Breakout Value:");
	sen_slider("##sen_breakout_value",user_sensitivity.breakout_value,0.4,2.5);
	ImGui::End();
}

int render_IMGUI() {
	if (window == nullptr) {
		printError("render_IMGUI(): window == nullptr");
	}
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({(fp32)Master.resX,(fp32)RESY_UI});
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	horizontal_buttons_IMGUI(window_flags);
	
	ImGui_WINDOW_FLAGS = 0;
	ImGui_WINDOW_FLAGS |= ImGuiWindowFlags_NoCollapse; 
	ImGui_WINDOW_FLAGS |= (AutoResizeWindows == true) ? ImGuiWindowFlags_NoResize : 0;

	if (buttonSelection != -1) {
		Lock_Key_Inputs = (LockKeyInputsInMenus == true) ? true : false;
		switch(buttonSelection) {
			case GUI_Menu_Coordinates:
				Menu_Coordinates();
			break;
			case GUI_Menu_Fractal:
				Menu_Fractal();
			break;
			case GUI_Menu_Import:
				//Menu_Import();
			break;
			case GUI_Menu_Rendering:
				Menu_Rendering();
			break;
			case GUI_Menu_Settings:
				Menu_Settings();
			break;
			case GUI_Menu_KeyBinds:
				Menu_Keybinds();
			break;
			default:
			Lock_Key_Inputs = false;
		}
	} else {
		Lock_Key_Inputs = false;
	}
	if (ShowTheXButton == false) {
		buttonSelection = -1;
		ShowTheXButton = true;
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	
	//SDL_RenderPresent(renderer); // Redundant
	return 0;
}

bool bootup_Fractal_Frame_Rendered = false;

TimerBox GUI_FrameTimer;
uint64_t GUI_FrameTimeNano = SECONDS_TO_NANO(1.0/60.0);
fp64 GUI_FrameTime = 1.0/60.0;
fp64 GUI_FrameRate = 60.0;

void correctFrameTime() {
	valueLimit(GUI_FrameTimeNano,SECONDS_TO_NANO(1.0/1200.0),SECONDS_TO_NANO(1.0/6.0));
	valueLimit(GUI_FrameTime,1.0/1200.0,1.0/6.0);
	valueLimit(GUI_FrameRate,6.0,1200.0);
	GUI_FrameTimer.setFreq(GUI_FrameTime);
}

void updateFrameTimeNano(uint64_t frameTime) {
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

int setupDisplayInfo(int32_t* initResX, int32_t* initResY, int32_t* initPosX, int32_t* initPosY) {
	if (initResX == nullptr || initResY == nullptr || initPosX == nullptr || initPosY == nullptr) {
		printError("Unable to get Display Info, NULL parameters");
		return -1;
	}
	using namespace Display_Bootup;
	DISPLAY_COUNT = SDL_GetNumVideoDisplays();
	printf("\n\tDisplay Count: %d",DISPLAY_COUNT);
	if (DISPLAY_COUNT == 0) {
		printError("No Displays Detected");
		return -1;
	}
	DisplayList = (DisplayInfo*)malloc(DISPLAY_COUNT * sizeof(DisplayInfo));
	if (DisplayList == nullptr) {
		FREE(DisplayList);
		printError("Unable to allocate memory for DisplayList");
		return -1;
	}
	for (size_t d = 0; d < Display_Bootup::Length; d++) {
		Display_Match[d] = 1; // Set to first monitor
	}
	Display_Match[Display_Bootup::Automatic] = 1;
	Display_Match[Display_Bootup::First] = 1;
	Display_Match[Display_Bootup::Last] = DISPLAY_COUNT;
	Display_Match[Display_Bootup::Specific] = SPECIFIC_BOOTUP_DISPLAY;
	#define Display(match) DisplayList[Display_Match[(match)] - 1]
	for (size_t i = 0; i < DISPLAY_COUNT; i++) {
		SDL_DisplayMode mode;
		SDL_Rect rect;
		SDL_GetDesktopDisplayMode(i,&mode);
		SDL_GetDisplayBounds(i, &rect);
		DisplayList[i].resX = mode.w;
		DisplayList[i].resY = mode.h;
		DisplayList[i].posX = rect.x;
		DisplayList[i].posY = rect.y;
		DisplayList[i].refreshRate = mode.refresh_rate;
		DisplayList[i].bbp = SDL_BITSPERPIXEL(mode.format);
		DisplayList[i].name = SDL_GetDisplayName(i);
		printf("\n\tDisplay[%zu]: %dx%d at %dHz (%d,%d) | %s",i+1,mode.w,mode.h,mode.refresh_rate,rect.x,rect.y,SDL_GetDisplayName(i)); //SDL_GetPixelFormatName(mode.format)
		/* Orthagonal */
		if (DisplayList[i].posX < Display(Display_Bootup::Left).posX) { Display_Match[Display_Bootup::Left] = i + 1; }
		if (DisplayList[i].posX + DisplayList[i].resX > Display(Display_Bootup::Right).posX + Display(Display_Bootup::Right).resX) { Display_Match[Display_Bootup::Right] = i + 1; }
		if (DisplayList[i].posY < Display(Display_Bootup::Top).posY) { Display_Match[Display_Bootup::Top] = i + 1; }
		if (DisplayList[i].posY + DisplayList[i].resY > Display(Display_Bootup::Bottom).posY + Display(Display_Bootup::Bottom).resY) { Display_Match[Display_Bootup::Bottom] = i + 1; }
		/* Diagonal */
		/* Resolution and Refresh-Rate */
		if (DisplayList[i].resX * DisplayList[i].resY > Display(Display_Bootup::HighResolution).resX * Display(Display_Bootup::HighResolution).resY) { Display_Match[Display_Bootup::HighResolution] = i + 1; }
		if (DisplayList[i].refreshRate > Display(Display_Bootup::HighFrameRate).refreshRate) { Display_Match[Display_Bootup::HighFrameRate] = i + 1; }
		if (DisplayList[i].resX * DisplayList[i].resY < Display(Display_Bootup::LowResolution).resX * Display(Display_Bootup::LowResolution).resY) { Display_Match[Display_Bootup::LowResolution] = i + 1; }
		if (DisplayList[i].refreshRate < Display(Display_Bootup::LowFrameRate).refreshRate) { Display_Match[Display_Bootup::LowFrameRate] = i + 1; }
	}
	*initResX = Display(Display_Bootup_Type).resX;
	*initResY = Display(Display_Bootup_Type).resY;
	*initPosX = Display(Display_Bootup_Type).posX;
	*initPosY = Display(Display_Bootup_Type).posY;
	if (Display(Display_Bootup_Type).resX < RESX_Minimum || Display(Display_Bootup_Type).resY < RESY_Minimum) {
		if (DISPLAY_COUNT == 1) {
			printWarning("This display does not meet the minimum screen resolution");
			return 0;
		}
		for (size_t i = 0; i < DISPLAY_COUNT; i++) {
			if (Display(Display_Bootup_Type).resX >= RESX_Minimum && Display(Display_Bootup_Type).resY >= RESY_Minimum) {
				*initResX = DisplayList[i].resX;
				*initResY = DisplayList[i].resX;
				*initPosX = DisplayList[i].resX;
				*initPosY = DisplayList[Display_Bootup_Type].resX;
				printWarning("Display %u does not meet the minimum screen resolution, switching to display %zu",Display_Match[Display_Bootup_Type],i + 1);
				return 0;
			}
		}
		printWarning("None of the available displays support the minimum screen resolution");
		return 0;
	}
	fflush(stdout);
	return 0;
}

int init_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	//SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printFatalError("SDL_Init(SDL_INIT_EVERYTHING) failed to initialize");
		return -1;
	}
	printf("\nSystem Information:");
	int32_t initResX, initResY, initPosX, initPosY;
	if (setupDisplayInfo(&initResX,&initResY,&initPosX,&initPosY) < 0) {
		printCriticalError("init_Render failed to setup Display Info");
	}

	initResX -= RESX_Margin;
	initResY -= RESY_Margin;
	if (useDefaultWindowSize == true) {
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
		initResX = calcMinMaxRatio((uint32_t)initResX,RESX_Minimum,RESX_Default,0.6);
		if (initResX > RESX_Maximum) {
			initResX = RESX_Maximum;
		}
		initResY = calcMinMaxRatio((uint32_t)initResY,RESY_Minimum,RESY_Default,0.6);
		if (initResY > RESX_Maximum) {
			initResY = RESX_Maximum;
		}
	}

	{
		#ifndef MANUAL_FRAME_RATE_OVERRIDE
			DisplayInfo* disp = getCurrentDisplayInfo();
			if (disp != nullptr) {
				FRAME_RATE = disp->refreshRate * Default_Frame_Rate_Multiplier;
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
	window = SDL_CreateWindow(PROGRAM_NAME " v" PROGRAM_VERSION " " PROGRAM_DATE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Master.resX, Master.resY, SDL_WINDOW_RESIZABLE);
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
    ImGui::StyleColorsDark();
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
	init_default_sensitivity(&user_sensitivity);
	bootup_Fractal_Frame_Rendered = false;
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

int terminate_Render() {
	terminateKeyboardGraphics();
	clear_KeyBind_PresetList();
	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
	FREE(DisplayList);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	FREE(TestGraphic.vram);
	return 0;
}

BufferBox* buf;

void setRenderedBufferBox(BufferBox* box) {
	buf = box;
}

#define fullColorTestGraphic

void renderTestGraphic(fp64 cycleSpeed, fp64 minSpeed, fp64 maxSpeed) {
	static fp64 f = 0.0;
	fp64 halfDiff = (maxSpeed - minSpeed) / 2.0;
	fp64 speed = halfDiff * sin(cycleSpeed * TAU * getDecimalTime()) + minSpeed + halfDiff;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
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
	}
}

void renderAbortGraphic(fp64 speed) {
	if (printValidateBufferBox(&TestGraphic) == false) {
		printError("renderAbortGraphic() failed");
		return;
	}
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = ((w + x + y) % 256) / 4; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = 0xFF; z++;
		}
	}
}

void renderPauseGraphic(fp64 speed) {
	if (printValidateBufferBox(&TestGraphic) == false) {
		printError("renderPauseGraphic() failed");
		return;
	}
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = 0xFF; z++;
		}
	}
}

void renderLoadingGraphic(fp64 speed) {
	if (printValidateBufferBox(&TestGraphic) == false) {
		printError("renderLoadingGraphic() failed");
		return;
	}
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0xFF; z++;
		}
	}
}

bool exportFractalBuffer = false;
bool exportSuperFractalBuffer = false;

int exportScreenshot() {
	static uint64_t resetTime = 0;
	if (getNanoTime() - resetTime > SECONDS_TO_NANO(0.5) && exportFractalBuffer == false) {
		resetTime = getNanoTime();
		exportFractalBuffer = true;
		printFlush("\nTaking Screenshot");
	}
	return 0;
}
int exportSuperScreenshot() {
	static uint64_t resetTime = 0;
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
		switch(screenshotFileType) {
			case Image_File_Format::PNG:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::PNG,User_PNG_Compression_Level);
			break;
			case Image_File_Format::JPG:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::JPG,User_JPG_Quality_Level);
			break;
			default:
				send_Image_Render(&superFrac,&superRenderData,Image_File_Format::PNG,User_PNG_Compression_Level);
		}
	}
	return 0;
}

#define Use_OpenCV_Scaler

SDL_Texture* scale_tex = nullptr;
SDL_Surface* scale_surface = nullptr;

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

	fp32 dx00 = 0.0f; fp32 dy00 = 0.0f; fp32 dx11 = 0.0f; fp32 dy11 = 0.0f;
	fp32 dx01 = 0.0f; fp32 dy01 = 0.0f; fp32 dx10 = 0.0f; fp32 dy10 = 0.0f;
	coordinate_to_image_cordinate(image->x00 - FRAC.r,image->y00 - FRAC.i,&dx00,&dy00,&FRAC,ren);
	coordinate_to_image_cordinate(image->x11 - FRAC.r,image->y11 - FRAC.i,&dx11,&dy11,&FRAC,ren);
	coordinate_to_image_cordinate(image->x01 - FRAC.r,image->y01 - FRAC.i,&dx01,&dy01,&FRAC,ren);
	coordinate_to_image_cordinate(image->x10 - FRAC.r,image->y10 - FRAC.i,&dx10,&dy10,&FRAC,ren);
	int32_t resX = (int32_t)(image->resX);
	int32_t resY = (int32_t)(image->resY);
	fp32 sx00 = 0.0f; fp32 sy00 = 0.0f;
	fp32 sx11 = resX; fp32 sy11 = (fp32)resY;
	fp32 sx01 = 0.0f; fp32 sy01 = (fp32)resY;
	fp32 sx10 = resX; fp32 sy10 = 0.0f;
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
			Frame_Interpolation_Method,
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
	// 		Frame_Interpolation_Method,
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
		primaryBufferValid = false;
		Waiting_To_Abort_Rendering = read_Abort_Render_Ongoing();
		if (Waiting_To_Abort_Rendering == true) {
			renderAbortGraphic(0.3);
		} else {
			renderPauseGraphic(0.4);
		}
		BufferBox temp_MASTER;
		Master.getBufferBox(&temp_MASTER);
		copyBuffer(TestGraphic,temp_MASTER,0,RESY_UI,false);
		exportFractalBuffer = false;
	} else if (primaryBufferValid == false) {
		renderLoadingGraphic(1.0); // Renders a loading screen if Fractal buffers are unavailable
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
			switch(screenshotFileType) {
				case Image_File_Format::PNG:
					writePNGImage(&temp_primaryBox,path,name,User_PNG_Compression_Level);
				break;
				case Image_File_Format::JPG:
					writeJPGImage(&temp_primaryBox,path,name,User_JPG_Quality_Level);
				break;
				case Image_File_Format::TGA:
					writeTGAImage(&temp_primaryBox,path,name);
				break;
				case Image_File_Format::BMP:
					writeBMPImage(&temp_primaryBox,path,name);
				break;
				default:
				printError("Unknown screenshot file type: %d",screenshotFileType);
			}
			FREE(name);
		}
		exportFractalBuffer = false;
	}
	render_IMGUI();
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(kTexture); // From render_IMGUI
}