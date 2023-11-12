/*
**	Author: zerico2005 (2023)
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

#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "programData.h"

SDL_Renderer* renderer;
SDL_Window* window;
ImGuiIO* io_IMGUI;

bool Abort_Rendering_Flag = false;
bool Waiting_To_Abort_Rendering = false;

SDL_Texture* texture;
SDL_Texture* kTexture; // Keyboard graphic
ImageBuffer Master;
BufferBox TestGraphic;
fp64 TestGraphicSpeed = 0.4;

ImageBuffer* Primary_Image = nullptr;
ImageBuffer* Primary_Image_Preview = nullptr;
ImageBuffer* Secondary_Image = nullptr;
ImageBuffer* Secondary_Image_Preview = nullptr;

//#define MANUAL_FRAME_RATE_OVERRIDE
fp64 FRAME_RATE = 60.0; // Double the max screen refresh rate
const fp64 FRAME_RATE_OFFSET = 0.01;
uint64_t FRAME_RATE_NANO;
#define  Default_Frame_Rate_Multiplier 2.0
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

// ImageBuffer primaryFracImage;
// ImageBuffer secondaryFracImage;

int exportScreenshot();
int exportSuperScreenshot();

namespace Image_File_Format {
	enum Image_File_Format_Enum {
		PNG,JPG,TGA,BMP,HDR,Image_File_Format_Count
	};
}

Image_File_Format::Image_File_Format_Enum screenshotFileType = Image_File_Format::PNG;
uint32_t User_PNG_Compression_Level = 8;
uint32_t User_JPG_Quality_Level = 95;

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

std::list<KeyBind> currentKeyBind = defaultKeyBind;
std::list<KeyBind> importedKeyBind;


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

		for (const auto& bind : currentKeyBind) {
			if (bind.key == (SDL_Scancode)i) {
				Key_Function::Key_Function_Enum func = bind.func;
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
		if (currentKeyBind.size() <= 0) {
			currentKeyBind = defaultKeyBind;
		}
		for (size_t s = 0; s < SDL_NUM_SCANCODES; s++) {
			size_t keyColorSet = 0;
			for (const auto& bind : currentKeyBind) {
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
	if (argc >= 2) {
		for (int a = 1; a < argc; a++) {
			if (strstr(argv[a],".fracExpKB") != NULL) {
				printFlush("\nFracExp_KeyBind File: %s",argv[a]);
				read_FracExpKB_File(&importedKeyBind,argv[a]);
				currentKeyBind = importedKeyBind;
				cleanKeyBind(&currentKeyBind);
				printf("\n\tKeyBinds: %zu Preset: %s",importedKeyBind.size(),"<Unimplemented>");
				break;
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

const char* buttonLabels[] = {"Fractal", "Export","Import", "Screenshot", "Rendering", "Settings", "KeyBinds"};
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
		if (x & 0x3) { x &= 0xFFFFFFFC; } // Sets resX to a multiple of 4 so I don't have to deal with padded and unpadded image buffers
		
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
		Master.resizeBuffer(x,y,3);
		if (texture != NULL) {
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
		write_Buffer_Size({nullptr,Master.resX,Master.resY - RESY_UI,3,0});
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

#define stretchValue(s) pow(2.0,-abs(s))
#define zoomDefault(p) (-log10(getABSFractalMaxRadius((fp64)(p))) - 0.01)

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

int updateFractalParameters() {
	using namespace Key_Function;
	using namespace Change_Level;
	#define FRAC frac.type.abs_mandelbrot
	static fp64 temp_maxItr = log2(FRAC.maxItr);
	fp64 temp_breakoutValue = log2(FRAC.breakoutValue);
	fp64 moveDelta = (DeltaTime < 0.2) ? DeltaTime : 0.2;
	
	int update_level = Change_Level::Nothing;
	#define Update_Level(level) update_level = ((level) > update_level) ? (level) : update_level

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
			moveCord(&FRAC.r, &FRAC.i, 0.0 * TAU + FRAC.rot, 0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX);
			Update_Level(Translation);
		}
		if (func_stat[decRealPos].triggered == true) {
			moveCord(&FRAC.r, &FRAC.i, 0.5 * TAU + FRAC.rot, 0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX);
			Update_Level(Translation);
		}
		if (func_stat[incImagPos].triggered == true) {
			moveCord(&FRAC.r, &FRAC.i, 0.25 * TAU + FRAC.rot, 0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY);
			Update_Level(Translation);
		}
		if (func_stat[decImagPos].triggered == true) {
			moveCord(&FRAC.r, &FRAC.i, 0.75 * TAU + FRAC.rot, 0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY);
			Update_Level(Translation);
		}
		if (funcTimeDelay(resetRealPos,0.2)) {
			FRAC.r = 0.0;
			Update_Level(Jump);
		}
		if (funcTimeDelay(resetImagPos,0.2)) {
			FRAC.i = 0.0;
			Update_Level(Jump);
		}
		valueLimit(FRAC.r,-10.0,10.0);
		valueLimit(FRAC.i,-10.0,10.0);
	/* Real and Imaginary Julia Z Coordinates */
		if (func_stat[incZReal].triggered == true) {
			moveCord(&FRAC.zr, &FRAC.zi, 0.0 * TAU + FRAC.rot, 0.3 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX);
			Update_Level(Translation);
		}
		if (func_stat[decZReal].triggered == true) {
			moveCord(&FRAC.zr, &FRAC.zi, 0.5 * TAU + FRAC.rot, 0.3 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX);
			Update_Level(Translation);
		}
		if (func_stat[incZImag].triggered == true) {
			moveCord(&FRAC.zr, &FRAC.zi, 0.25 * TAU + FRAC.rot, 0.3 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY);
			Update_Level(Translation);
		}
		if (func_stat[decZImag].triggered == true) {
			moveCord(&FRAC.zr, &FRAC.zi, 0.75 * TAU + FRAC.rot, 0.3 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY);
			Update_Level(Translation);
		}
		if (funcTimeDelay(resetZReal,0.2)) {
			FRAC.zr = 0.0;
			Update_Level(Translation);
		}
		if (funcTimeDelay(resetZImag,0.2)) {
			FRAC.zi = 0.0;
			Update_Level(Translation);
		}
		if (FRAC.cursorZValue == true) {
			if (FRAC.relativeZValue == true) {
				fp64 resZ = (fp64)((Master.resX > Master.resY) ? Master.resY : Master.resX);
				FRAC.zr = 4.0 * ((fp64)ImGui::GetMousePos().x - ((fp64)Master.resX / 2.0)) / resZ;
				FRAC.zi = 4.0 * ((fp64)(ImGui::GetMousePos().y - RESY_UI) - ((fp64)Master.resY / 2.0)) / resZ;
				Update_Level(Minor_Reset);
			} else {
				pixel_to_coordinate(ImGui::GetMousePos().x,ImGui::GetMousePos().y - RESY_UI,&FRAC.zr,&FRAC.zi,&FRAC,&primaryRenderData);
				Update_Level(Minor_Reset);
			}
		}
		valueLimit(FRAC.zr,-4.0,4.0);
		valueLimit(FRAC.zi,-4.0,4.0);
	/* Zoom */
		if (func_stat[incZoom].triggered == true) {
			FRAC.zoom += 0.2 * moveDelta;
			Update_Level(Zoom);
		}
		if (func_stat[decZoom].triggered == true) {
			FRAC.zoom -= 0.2 * moveDelta;
			Update_Level(Zoom);
		}
		if (funcTimeDelay(resetZoom,0.2)) {
			FRAC.zoom = zoomDefault(FRAC.power);
			Update_Level(Jump);
		}
		valueLimit(FRAC.zoom,-20.0,40.0);
		if (funcTimeDelay(resetCoordinates,0.2)) {
			FRAC.r = 0.0; FRAC.i = 0.0;
			if (FRAC.polarMandelbrot == true) {
				FRAC.zoom = zoomDefault(FRAC.polarPower);
			} else {
				FRAC.zoom = zoomDefault((fp64)FRAC.power);
			}
			valueLimit(FRAC.zoom,-0.4,0.4);
			Update_Level(Zoom);
		}
	/* maxItr */
		if (func_stat[incMaxItr].triggered) {
			temp_maxItr += 2.0 * moveDelta;
			Update_Level(Minor_Reset);
		}
		if (func_stat[decMaxItr].triggered) {
			temp_maxItr -= 2.0 * moveDelta;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(resetMaxItr,0.2)) {
			temp_maxItr = log2(192.0);
			FRAC.maxItr = 192;
			Update_Level(Minor_Reset);
		}
	/* Formula*/
		if (funcTimeDelay(incFormula,1.0/10.0)) {
			FRAC.formula++;
			Update_Level(Major_Reset);
		}
		if (funcTimeDelay(decFormula,1.0/10.0)) {
			FRAC.formula--;
			Update_Level(Major_Reset);
		}
		if (funcTimeDelay(incFamily,1.0/10.0)) {
			FRAC.formula += getABSValue(FRAC.power);
			Update_Level(Major_Reset);
		}
		if (funcTimeDelay(decFamily,1.0/10.0)) {
			FRAC.formula -= getABSValue(FRAC.power);
			Update_Level(Major_Reset);
		}
		if (funcTimeDelay(resetFormula,0.2)) {
			if (FRAC.formula == 0) {
				Update_Level(Major_Reset);
			}
			FRAC.formula = 0;
		}
	/* Power */
		if (FRAC.polarMandelbrot == true) {
			if (FRAC.integerPolarPower == true) {
				if (funcTimeDelay(incPower,1.0/6.0)) {
					FRAC.polarPower++;
					Update_Level(Minor_Reset);
				}
				if (funcTimeDelay(decPower,1.0/6.0)) {
					FRAC.polarPower--;
					Update_Level(Minor_Reset);
				}
			} else {
				if (func_stat[incPower].triggered) {
					FRAC.polarPower += moveDelta * 1.0;
					Update_Level(Minor_Reset);
				}
				if (func_stat[decPower].triggered) {
					FRAC.polarPower -= moveDelta * 1.0;
					Update_Level(Minor_Reset);
				}
			}
			
			if (funcTimeDelay(resetPower,0.2)) {
				if (FRAC.polarPower != Polar_Mandelbrot_Default_Power) {
					Update_Level(Minor_Reset);
				}
				FRAC.polarPower = Polar_Mandelbrot_Default_Power;
			}
			if (funcTimeDelay(roundPower,0.2)) {
				if (FRAC.polarPower != round(FRAC.polarPower)) {
					Update_Level(Minor_Reset);
				}
				FRAC.polarPower = round(FRAC.polarPower);
			}
			if (funcTimeDelay(floorPower,0.2)) {
				if (FRAC.polarPower != floor(FRAC.polarPower)) {
					Update_Level(Minor_Reset);
				}
				FRAC.polarPower = floor(FRAC.polarPower);
			}
			if (funcTimeDelay(ceilingPower,0.2)) {
				if (FRAC.polarPower != ceil(FRAC.polarPower)) {
					Update_Level(Minor_Reset);
				}
				FRAC.polarPower = ceil(FRAC.polarPower);
			}
		} else {
			if (funcTimeDelay(incPower,1.0/6.0)) {
				FRAC.power++;
				Update_Level(Major_Reset);
			}
			if (funcTimeDelay(decPower,1.0/6.0)) {
				FRAC.power--;
				Update_Level(Major_Reset);
			}
			if (funcTimeDelay(resetPower,0.2)) {
				if (FRAC.power != ABS_Mandelbrot_Default_Power) {
					Update_Level(Major_Reset);
				}
				FRAC.power = ABS_Mandelbrot_Default_Power;
			}
			FRAC.formula = limitFormulaID(FRAC.power,FRAC.formula);
		}
	/* Rotations */
		if (func_stat[counterclockwiseRot].triggered) {
			FRAC.rot -= (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch);
			Update_Level(Rotation);
		}
		if (func_stat[clockwiseRot].triggered) {
			FRAC.rot += (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch);
			Update_Level(Rotation);
		}
		if (funcTimeDelay(clockwiseRot90,0.3)) {
			FRAC.rot += (TAU * (90.0/360.0));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(counterclockwiseRot90,0.3)) {
			FRAC.rot -= (TAU * (90.0/360.0));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(rotate180,0.3)) {
			FRAC.rot += (TAU * (180.0/360.0));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(clockwiseRotStep,1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(counterclockwiseRotStep,1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(clockwiseRotPower,1.0/6.0)) {
			FRAC.rot += (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(counterclockwiseRotPower,1.0/6.0)) {
			FRAC.rot -= (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
			Update_Level(Rotation);
		}
		if (funcTimeDelay(resetRotation,0.2)) {
			FRAC.rot = 0.0;
			Update_Level(Rotation);
		}
		FRAC.rot = (FRAC.rot >= 0.0) ? fmod(FRAC.rot,TAU) : fmod(FRAC.rot + TAU,TAU);
	/* Transformations */
		if (func_stat[incStretch].triggered) {
			FRAC.stretch += 1.0 * moveDelta;
			Update_Level(Zoom);
		}
		if (func_stat[decStretch].triggered) {
			FRAC.stretch -= 1.0 * moveDelta;
			Update_Level(Zoom);
		}
		if (funcTimeDelay(resetStretch,0.2)) {
			FRAC.stretch = 0.0;
			Update_Level(Jump);
		}
		if (funcTimeDelay(resetTransformations,0.2)) {
			FRAC.rot = 0.0;
			FRAC.stretch = 0.0;
			Update_Level(Minor_Reset);
		}
	/* Breakout Value */
		if (func_stat[incBreakout].triggered) {
			temp_breakoutValue += 2.0 * moveDelta;
			Update_Level(Minor_Reset);
		}
		if (func_stat[decBreakout].triggered) {
			temp_breakoutValue -= 2.0 * moveDelta;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(resetBreakout,0.2)) {
			temp_breakoutValue = log2(16777216.0);
			Update_Level(Minor_Reset);
		}
	/* Rendering */
		if (funcTimeDelay(incSubSample,1.0/6.0)) {
			primaryRenderData.subSample++;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(decSubSample,1.0/6.0)) {
			primaryRenderData.subSample--;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(resetSubSample,0.2)) {
			primaryRenderData.subSample = 1;
			Update_Level(Minor_Reset);
		}
		valueLimit(primaryRenderData.subSample,1,24);
		if (funcTimeDelay(incSuperSample,1.0/6.0)) {
			primaryRenderData.sample++;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(decSuperSample,1.0/6.0)) {
			primaryRenderData.sample--;
			Update_Level(Minor_Reset);
		}
		if (funcTimeDelay(resetSuperSample,0.2)) {
			primaryRenderData.sample = 1;
			Update_Level(Minor_Reset);
		}
		valueLimit(primaryRenderData.sample,1,24);
	/* Rendering Method */
	{
		using namespace Rendering_Method;
		if (funcTimeDelay(fp32CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 32;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp64CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 64;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp80CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 80;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp128CpuRendering,0.2)) {
			primaryRenderData.rendering_method = CPU_Rendering;
			primaryRenderData.CPU_Precision = 128;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp16GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 16;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp32GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 32;
			Update_Level(Method_of_Rendering);
		}
		if (funcTimeDelay(fp64GpuRendering,0.2)) {
			primaryRenderData.rendering_method = GPU_Rendering;
			primaryRenderData.GPU_Precision = 64;
			Update_Level(Method_of_Rendering);
		}
	}
	/* Other */
		valueLimit(FRAC.stretch,-100.0,100.0);
		if (FRAC.stretch >= 0.0) {
			FRAC.sX = 1.0;
			FRAC.sY = stretchValue(FRAC.stretch);
		} else {
			FRAC.sX = stretchValue(FRAC.stretch);
			FRAC.sY = 1.0;
		}
		FRAC.rot = (FRAC.rot >= 0.0) ? fmod(FRAC.rot,TAU) : fmod(FRAC.rot + TAU,TAU);
		valueLimit(temp_maxItr,log2(16.0),log2(16777216.0));
		FRAC.maxItr = pow(2.0,temp_maxItr);
		valueLimit(FRAC.maxItr,16,16777216);
		valueLimit(temp_breakoutValue,log2(0.25),log2(4294967296.0));
		FRAC.breakoutValue = pow(2.0,temp_breakoutValue);
		valueLimit(FRAC.breakoutValue,0.25,4294967296.0);
		valueLimit(FRAC.power,2,6); // Support up to Sextic
	/* ABS Mandelbrot */
	/* Polar Mandelbrot */
		if (frac.type_value == Fractal_Polar_Mandelbrot) {
			if (FRAC.lockToCardioid == true) {
				FRAC.r = getABSFractalMinRadius(FRAC.polarPower);
				FRAC.r *= (FRAC.flipCardioidSide == true) ? -1.0 : 1.0;
			}
		}
		if (FRAC.integerPolarPower == true) {
			FRAC.polarPower = round(FRAC.polarPower);
			valueLimit(FRAC.polarPower,ceil(POLAR_POWER_MINIMUM),floor(POLAR_POWER_MAXIMUM));
		} else {
			valueLimit(FRAC.polarPower,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM);
		}
	/* Global Application Functions */
		if (funcTimeDelay(openFractalMenu,0.2)) {
			buttonSelection = 0;
		}
		if (funcTimeDelay(takeScreenshot,0.5)) {
			exportScreenshot();
		}
		if (funcTimeDelay(takeSuperScreenshot,0.5)) {
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
	ImGui::SetNextWindowSizeConstraints({(fp32)minX,(fp32)minY},{(fp32)valX - bufX,(fp32)valY - bufY}); \
	WINDOW_RESX = (WINDOW_RESX > valX - bufX) ? (valX - bufX) : WINDOW_RESX; \
	WINDOW_RESY = (WINDOW_RESY > valY - bufY) ? (valY - bufY) : WINDOW_RESY;

enum Menu_Enum {GUI_Menu_Fractal, GUI_Menu_Import, GUI_Menu_Rendering, GUI_Menu_Settings, GUI_Menu_KeyBinds};

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

    ImGui::Text("Button: %d",buttonSelection); ImGui::SameLine();
    size_t buttonCount = sizeof(buttonLabels) / sizeof(buttonLabels[0]);
	if (ImGui::Button("Fractal")) {
		buttonSelection = (buttonSelection == GUI_Menu_Fractal) ? -1 : GUI_Menu_Fractal;
	} ImGui::SameLine();
	if (ImGui::Button("Export")) {

	} ImGui::SameLine();
	if (ImGui::Button("Import")) {
		buttonSelection = (buttonSelection == GUI_Menu_Import) ? -1 : GUI_Menu_Import;
	} ImGui::SameLine();
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

	/*
	static char input_real[128] = "-1.0";
	Param_Input_Box("Real:","##input_real",input_real);
	ImGui::SameLine();
	static char input_imag[128] = "0.0";
	Param_Input_Box("Imag:","##input_imag",input_imag);
	ImGui::SameLine();
	static fp64 input_zoom = -0.302;
	Param_Input_Double("Zoom: 10^","##input_zoom",&input_zoom,"%.6lf"); valueLimit(input_zoom,-10.0,40.0);
	ImGui::SameLine();
	static int32_t input_maxItr = 192;
	Param_Input_Int("Max-Itr:","##input_maxItr",&input_maxItr); valueLimit(input_maxItr,4,16777216);

	static char input_juliaReal[128] = "0.0";
	Param_Input_Box("Julia-Real:","##input_juliaReal",input_juliaReal);
	ImGui::SameLine();
	static char input_juliaImag[128] = "0.0";
	Param_Input_Box("Julia-Imag:","##input_juliaImag",input_juliaImag);
	ImGui::SameLine();
	static char input_zReal[128] = "0.0";
	Param_Input_Box("Z-Real:","##input_zReal",input_zReal);
	ImGui::SameLine();
	static char input_zImag[128] = "0.0";
	Param_Input_Box("Z-Imag:","##input_zImag",input_zImag);
	*/
	#define FRAC frac.type.abs_mandelbrot
	uint32_t renderFP = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? primaryRenderData.CPU_Precision : primaryRenderData.GPU_Precision;
	const char* const renderMethod = (primaryRenderData.rendering_method == Rendering_Method::CPU_Rendering) ? "CPU" : "GPU";
	ImGui::Text(
		"Formula: %llu Power: %6.4lf Super-Sample: %u Rendering: %s fp%u",
		FRAC.formula,(FRAC.polarMandelbrot ? FRAC.polarPower : (fp64)FRAC.power),primaryRenderData.sample * primaryRenderData.sample,renderMethod,renderFP
	);
	ImGui::Text(
		"Zreal: %10.8lf Zimag: %10.8lf Rotation: %3.1lf Stetch: %6.4lf",
		FRAC.zr,FRAC.zi,FRAC.rot * 360.0 / TAU,FRAC.stretch
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
		"Real: %10.8lf Imag: %10.8lf Zoom: 10^%6.4lf Itr: %u",
		FRAC.r,FRAC.i,adjustedZoomValue,FRAC.maxItr
	);
	#undef FRAC
    // End the ImGui window
    ImGui::End();
}

void Menu_Fractal() {
	ImGui_DefaultWindowSize(Master.resX,16,240,400,0.7,Master.resY,16,160,320,0.7);
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
	ImGui::Begin("Fractal Menu",&ShowTheXButton);
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
		static fp32 temp_input_breakoutValue = (fp32)log2(FRAC.breakoutValue);
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
	ImGui_DefaultWindowSize(Master.resX,16,240,400,0.7,Master.resY,16,160,320,0.7);
	static const char* CPU_RenderingModes[] = {"fp32 | 10^5.7","fp64 | 10^14.4 (Default)","fp80 | 10^17.7","fp128 | 10^32.5"};
	static const char* GPU_RenderingModes[] = {"fp16 | 10^1.8","fp32 | 10^5.7 (Default)","fp64 | 10^14.4"};
	static int input_subSample = primaryRenderData.subSample;
	static int input_superSample = primaryRenderData.sample;
	int CPU_ThreadCount = (int)std::thread::hardware_concurrency();
	static int input_CPU_MaxThreads = ((CPU_ThreadCount <= 1) ? 1 : (CPU_ThreadCount - 1));
	static int input_CPU_ThreadMultiplier = 1;
	static int Combo_CPU_RenderingMode = 1;
	static int Combo_GPU_RenderingMode = 1;

	ImGui::Begin("Rendering Menu",&ShowTheXButton);
	
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
	ImGui::Text("Maximum Threads:");
	ImGui::SliderInt("##input_CPU_MaxThreads",&input_CPU_MaxThreads,1,CPU_ThreadCount);
	ImGui::Text("Thread Multiplier:");
	ImGui::SliderInt("##input_CPU_ThreadMultiplier",&input_CPU_ThreadMultiplier,1,16);
	primaryRenderData.CPU_Threads = input_CPU_MaxThreads * input_CPU_ThreadMultiplier;
	
	ImGui::Text("GPU Rendering Mode:");
	if (ImGui::Combo("##GPU_RenderingMode", &Combo_GPU_RenderingMode, BufAndLen(GPU_RenderingModes))) {
		switch (Combo_GPU_RenderingMode) {
			case 0:
				primaryRenderData.GPU_Precision = 16;
			break;
			case 1:
				primaryRenderData.GPU_Precision = 32;
			break;
			case 2:
				primaryRenderData.GPU_Precision = 64;
			break;
		};
	}
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
	ImGui_DefaultWindowSize(Master.resX,16,240,400,0.7,Master.resY,16,160,320,0.7);
	ImGui::Begin("Settings Menu",&ShowTheXButton);
	ImGui::Text("Base maximum frame-rate off of:");
	if (ImGui::Combo("##initFrameRate", &Combo_initFrameRate, BufAndLen(initFrameRate))) {
	
	}
	switch(Combo_initFrameRate) {
		case 0:
			printDisplayInfo(CURRENT_DISPLAY);
		break;
		case 1:
			printDisplayInfo(Display_Match[Display_Bootup::HighFrameRate]);
		break;
		case 2:
			printDisplayInfo(Display_Match[Display_Bootup::LowFrameRate]);
		break;
	}

	DisplayInfo* disp = getCurrentDisplayInfo();
	static fp64 FPS_Constant_Value = (disp == NULL) ? 60.0 : (fp64)(disp->refreshRate);
	fp64 TEMP_FPS = (disp == NULL) ? 60.0 : (fp32)(disp->refreshRate); // Would normally be either the current, highest, or lowest refresh rate
	if (Combo_initFrameRate == 3) {
		static fp32 temp_FPS_Constant_Value = (disp == NULL) ? 60.0 : (fp32)(disp->refreshRate);
		ImGui::Text("%.3lfms",(1.0 / FPS_Constant_Value) * 1000.0);
		ImGui::InputFloat("##temp_FPS_Constant_Value",&temp_FPS_Constant_Value,6.0,30.0,"%.3f"); valueLimit(temp_FPS_Constant_Value,6.0,1200.0);
		FPS_Constant_Value = (fp64)temp_FPS_Constant_Value;
	} else {
		ImGui::Text(" "); // Blank Line
		static int temp_frameMultiplier = 2 - 1;
		static fp64 frameMultiplier = 2.0;
		if (temp_frameMultiplier >= 0) {
			frameMultiplier = (fp64)(temp_frameMultiplier + 1);
			ImGui::Text("Maximum FPS Multiplier: %dx",(temp_frameMultiplier + 1));
		} else {
			frameMultiplier = 1.0 / (fp64)(1 - temp_frameMultiplier);
			ImGui::Text("Maximum FPS Multiplier: 1/%dx", (1 - temp_frameMultiplier));
		}
		ImGui::Text("%.2lffps %.2lfms", frameMultiplier * TEMP_FPS, (1.0 / (frameMultiplier * TEMP_FPS)) * 1000.0);
		ImGui::SliderInt("##temp_frameMultiplier",&temp_frameMultiplier,-6 + 1,6 - 1,"");
	}

	ImGui::Separator();

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

	ImGui::Separator();
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
	}

	ImGui::End();
}

void Menu_Keybinds() {
	static int Combo_keyboardSize = 0;
	static const char* keyboardSizeText[] = {
		"ANSI (Default)","Extended (Contains some FN keys)","Complete (All 242 SDL Scancodes)"
	};
	static bool displayNumpad = true;
	ImGui_DefaultWindowSize(Master.resX,16,320,480,0.7,Master.resY,16,240,360,0.7);
	ImGui::Begin("Keybinds Menu",&ShowTheXButton);
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
		SDL_Surface* kSurface = SDL_CreateRGBSurfaceWithFormatFrom(kBuf.vram, kBuf.resX, kBuf.resY, 24, 3 * kBuf.resX, SDL_PIXELFORMAT_RGB24);
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
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			using namespace Key_Function;
			size_t funcCount = 0;
			for (const auto& bind : currentKeyBind) {
				if (bind.key == keyClick) {
					funcCount++;
				}
			}
			if (funcCount > 0) {
				//ImGui::Text("Key Functions:");
				for (const auto& bind : currentKeyBind) {
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
				if (addKeyBind(&currentKeyBind,(Key_Function::Key_Function_Enum)Combo_functionSelect,keyClick) >= 0) {
					recolorKeyboard();
					Combo_functionSelect = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					printError("addKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);
				}
			}
			ImGui::SameLine(); 
			if(ImGui::Button("Clear Key-bind")) {
				if (removeKeyBind(&currentKeyBind,(Key_Function::Key_Function_Enum)Combo_functionSelect,keyClick) >= 0) {
					recolorKeyboard();
					Combo_functionSelect = Key_Function::NONE;
				} else {
					printError("removeKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);
				}
			}
		} else {
			ImGui::Text("Click on a Key and Select a Function");
			ImGui::Text(" ");
		}
		if (Combo_functionSelect != Key_Function::NONE) {
			if(ImGui::Button("Clear Function bindings")) {
				if (removeKeyBind(&currentKeyBind,(Key_Function::Key_Function_Enum)Combo_functionSelect) >= 0) {
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
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			if(ImGui::Button("Clear Key bindings")) {
				if (removeKeyBind(&currentKeyBind,keyClick) >= 0) {
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
		ImGui::Text(" ");
		ImGui::Separator();
		ImGui::Text("Current Key-bind: %s","<Unimplemented>"/*currentKeyBind->name*/);
		if (importedKeyBind.size() > 0) {
			if (ImGui::Button("Switch to imported key-binds")) {
				Combo_functionSelect = Key_Function::NONE;
				keyClick = SDL_SCANCODE_UNKNOWN;
				currentKeyBind = importedKeyBind;
				recolorKeyboard();
			}
		} else {
			ImGui::Text("No imported key-binds available");
		}
		if (ImGui::Button("Reset to default key-binds")) {
			Combo_functionSelect = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			currentKeyBind = defaultKeyBind;
			recolorKeyboard();
		}
		ImGui::Text(" ");
		
		//	Disabling the name field since it can cause confusion when typing "./folder" + "KeyBind.fracExpKB" = "./folderKeyBind.fracExpKB"
		// static char exportFracExpKBName[324] = "KeyBind";
		static char exportFracExpKBDir[324] = "./KeyBind"; // "./"
		ImGui::Text("Export current key-bind to directory:");
		ImGui::InputText("##exportFracExpKBDir",exportFracExpKBDir,TEXT_LENGTH(exportFracExpKBDir));
		// ImGui::Text("File Name:");
		// ImGui::InputText("##exportFracExpKBName",exportFracExpKBName,TEXT_LENGTH(exportFracExpKBName));
		static TimerBox exportTimer(1.0);
		if (ImGui::Button("Export .FracExpKB File") && exportTimer.timerReset()) {
			/* Move this file path code into fracExpKB.cpp */
			const char* const exportFracExpKBExtension = ".fracExpKB";
			size_t sizeOfPath = 0;
			sizeOfPath += strnlen(exportFracExpKBExtension,sizeof(exportFracExpKBExtension)) * sizeof(exportFracExpKBExtension[0]);
			sizeOfPath += strnlen(exportFracExpKBDir,sizeof(exportFracExpKBDir)) * sizeof(exportFracExpKBDir[0]);
			// sizeOfPath += strnlen(exportFracExpKBName,sizeof(exportFracExpKBName)) * sizeof(exportFracExpKBName[0]);
			char* exportFracExpKBPath = (char*)malloc(sizeOfPath);
			if (exportFracExpKBPath != NULL) {
				memset(exportFracExpKBPath,'\0',sizeOfPath);
				strncat(exportFracExpKBPath,exportFracExpKBDir,sizeOfPath);
				// strncat(exportFracExpKBPath,exportFracExpKBName,sizeOfPath);
				strncat(exportFracExpKBPath,exportFracExpKBExtension,sizeOfPath);
				if (write_FracExpKB_File(&currentKeyBind,exportFracExpKBPath) >= 0) {
					printFlush("\n%s was exported successfully",exportFracExpKBPath);
				} else {
					printError("%s failed to export",exportFracExpKBPath);
				}
			} else {
				printError("Unable to allocate memory to char* exportFracExpKBPath");
			}
			FREE(exportFracExpKBPath);
		}
	}

	ImGui::End();
}

int render_IMGUI() {

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
	if (buttonSelection != -1) {
		switch(buttonSelection) {
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
		}
	}
	if (ShowTheXButton == false) {
		buttonSelection = -1;
		ShowTheXButton = true;
	}


	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(kTexture);
	return 0;
}

int start_Render(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING, std::mutex& Key_Function_Mutex) {
	/*
	static bool yeildPrint = false;
	static uint64_t yeildCount = 0;
	static uint64_t yeildSum = 0;
	static uint64_t yeildTimer = getNanoTime();
	static uint64_t yeildError = 0;
	static uint64_t yeildSave = 0;
	*/
	uint64_t yeildTimeNano = 80000; /* 80 micro seconds */
	uint64_t FRAME_RATE_NANO = SECONDS_TO_NANO(1.0 / FRAME_RATE);
	//printFlush("\nyeildTimeNano: %llu | %lf",yeildTimeNano,NANO_TO_SECONDS(yeildTimeNano));
	TimerBox frameTimer = TimerBox(1.0/FRAME_RATE);
	TimerBox maxFrameReset = TimerBox(1.0/5.0); /* Keeps track of longest frame times */
	write_Update_Level(Change_Level::Full_Reset);
	while (QUIT_FLAG == false) {
		{ // Accesses ABORT_RENDERING only when Abort_Rendering_Flag changes to reduce unnecessary accesses
			static bool Abort_Rendering_Change = Abort_Rendering_Flag;
			if (Abort_Rendering_Flag != Abort_Rendering_Change) {
				ABORT_RENDERING = Abort_Rendering_Flag;
				Abort_Rendering_Change = Abort_Rendering_Flag;
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
		/* Poorly implemented, please correct */
		if (frameTimer.timerReady() == false) {
			uint64_t yeildLimit = frameTimer.timeElapsedNano();
			if (yeildLimit < yeildTimeNano) {
				//uint64_t yeildStart = getNanoTime();
				std::this_thread::yield();
				/*
				uint64_t yeildEnd = getNanoTime() - yeildStart;
				yeildCount++;
				if (frameTimer.timerReady() == true) {
					yeildError++; yeildPrint = true;
					printFlush("\n%.3lfus",(frameTimer.timeElapsed() - (1.0/FRAME_RATE)) * 1.0e6);
				} else {
					yeildSave += yeildEnd;
				}
				*/
			}
		}
		
		/*
		#define TIME_SCALE 1000
		if (frameTimer.timerReady() == false) {
			uint64_t timeElapsed = frameTimer.timeElapsedNano();
			uint64_t timeLeft = frameTimer.timeToTimerReadyNano();
			printFlush("\n\nElapsed: %6lluus TimeLeft:  %6lluus",timeElapsed/TIME_SCALE, timeLeft/TIME_SCALE);
			if (timeLeft > END_SLEEP_HEADROOM) {
				uint64_t sleepTime = timeLeft - END_SLEEP_HEADROOM;
				std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
				uint64_t remainingTime = frameTimer.timeToTimerReadyNano();
				printFlush("\nSleep:   %6lluus Remaining: %6lluus",sleepTime/TIME_SCALE,remainingTime/TIME_SCALE);
			}
		}
		*/
		if (frameTimer.timerReset()) {
			DeltaTime = frameTimer.getDeltaTime();
			fp64 RenderTime = getRenderDelta();
			{
				static fp64 maxFrameTime = 0.0;
				static fp64 maxRenderTime = 0.0;
				if (maxFrameReset.timerReset()) {
					Frame_Time_Display = maxFrameTime;
					Render_Time_Display = maxRenderTime;
					maxFrameTime = 0.0;
					maxRenderTime = 0.0;
					/*
					if (yeildPrint == true || yeildSwitch == true) {
						yeildSwitch = false;
						yeildPrint = false;
						printFlush("\n\nYeild Count: %llu Errors: %llu",yeildCount, yeildError);
						printFlush("\nTime Saved: %.3lfms",NANO_TO_SECONDS(yeildSave) * 1.0e3);
						yeildSum += yeildCount;
						printFlush("\nYeild Error: %.3lfms per error\n",((fp64)(getNanoTime() - yeildTimer) / (fp64)yeildError) / 1.0e6);
						yeildCount = 0;
					}
					*/				
				}
				if (DeltaTime > maxFrameTime) {
					maxFrameTime = DeltaTime;
				}
				if (RenderTime > maxRenderTime) {
					maxRenderTime = RenderTime;
				}
			}
			if (FRAME_RATE_NANO < 2 * yeildTimeNano) {
				yeildTimeNano = 0;
			} else {
				yeildTimeNano = FRAME_RATE_NANO - yeildTimeNano;
			}
			windowResizingCode();
			updateFractalParameters();
			write_Parameters(&frac,&primaryRenderData,&secondaryRenderData);
			newFrame();
		}
	}
	return 0;
}

// Automatic,First,Last,Specific,Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,HighResolution,HighFrameRate,LowResolution,LowFrameRate,Length

int setupDisplayInfo(int32_t* initResX, int32_t* initResY, int32_t* initPosX, int32_t* initPosY) {
	if (initResX == NULL || initResY == NULL || initPosX == NULL || initPosY == NULL) {
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
	if (DisplayList == NULL) {
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
	SDL_Init(SDL_INIT_VIDEO);
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
			if (disp != NULL) {
				FRAME_RATE = disp->refreshRate * Default_Frame_Rate_Multiplier;
			}
		#endif
		FRAME_RATE += FRAME_RATE_OFFSET;
		valueLimit(FRAME_RATE,6.0,1200.0);
	}
	printf("\n\tOperating System: %s",SDL_GetPlatform());
	printf("\n\tSystem RAM: %dMB",SDL_GetSystemRAM());
	// Allocate Buffers
	//initBufferBox(&Master,NULL,initResX,initResY,3);
	Master = ImageBuffer(initResX,initResY,3);
	initBufferBox(&TestGraphic,NULL,Master.resX,Master.resY - RESY_UI,3);
	TestGraphic.vram = (uint8_t*)malloc(getBufferBoxSize(&TestGraphic));
	window = SDL_CreateWindow(PROGRAM_NAME " v" PROGRAM_VERSION " " PROGRAM_DATE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Master.resX, Master.resY, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, Master.resX, Master.resY);
	write_Buffer_Size({NULL,Master.resX,Master.resY - RESY_UI,3,0});
	// IMGUI
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_IMGUI = &ImGui::GetIO();
    io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark(); // ImGui::StyleColorsLight();
	ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	
	setDefaultParameters(&frac,Fractal_ABS_Mandelbrot);
	initRenderData(&primaryRenderData);
	initRenderData(&secondaryRenderData);
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
	// printf("\nInit_Render: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	initKeys();
	cleanKeyBind(&currentKeyBind);
	write_Render_Ready(true);
	start_Render(QUIT_FLAG,ABORT_RENDERING,Key_Function_Mutex);
	return 0;
}

int terminate_Render() {
	terminateKeyboardGraphics();
	currentKeyBind.clear();
	importedKeyBind.clear();
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
		}
	}
}

void renderAbortGraphic(fp64 speed) {
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = ((w + x + y) % 256) / 4; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0; z++;
		}
	}
}

void renderPauseGraphic(fp64 speed) {
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = 0; z++;
		}
	}
}

void renderLoadingGraphic(fp64 speed) {
	static fp64 f = 0.0;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = 0; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
		}
	}
}

bool exportFractalBuffer = false;

int exportScreenshot() {
	static uint64_t resetTime = 0;
	if (getNanoTime() - resetTime > SECONDS_TO_NANO(0.5)) {
		resetTime = getNanoTime();
		exportFractalBuffer = true;
		printFlush("\nTaking Screenshot");
	}
	return 0;
}
int exportSuperScreenshot() {
	static uint64_t resetTime = 0;
	if (getNanoTime() - resetTime > SECONDS_TO_NANO(0.5)) {
		resetTime = getNanoTime();
		exportFractalBuffer = true;
	}
	return 0;
}

SDL_Texture* scale_tex = NULL;
SDL_Surface* scale_surface = NULL;

int displayFracImage(ImageBuffer* image, Render_Data* ren) {
	if (image == NULL) { printError("ImageBuffer* image is NULL"); return -1; }
	if (image->vram == NULL ) { printError("ImageBuffer* image->vram is NULL"); return -1; }
	if (image->allocated() == false) { printError("ImageBuffer* image is not allocated"); return -1; }
	if (ren == NULL) { printError("ImageBuffer* image is NULL"); return -1; }
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
		scale_surface = SDL_CreateRGBSurfaceWithFormatFrom(image->vram, image->resX, image->resY, image->channels * 8, image->channels * image->resX, SDL_PIXELFORMAT_RGB24);
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

void newFrame() {
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
	bool primaryBufferChange = next_Read_Cycle_Pos(&Primary_Image,Primary_Full);
	bool primaryBufferValid = (Primary_Image == nullptr || Primary_Image->vram == nullptr || Primary_Image->allocated() == false) ? false : true;
	//int primaryBufState = read_Image_Buffers(&primaryFracImage);
	//printfChange(int,primaryBufState,"\nprimaryBufState: %d",primaryBufState);
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

	SDL_UpdateTexture(texture, NULL, Master.vram, Master.resX * Master.channels);
	{
		SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_Rect dstRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	}
	
	if (Abort_Rendering_Flag == false && primaryBufferValid == true) {
		BufferBox temp_primaryBox;
		Primary_Image->getBufferBox(&temp_primaryBox);
		//primaryFracImage.printTransformationData();
		//copyBuffer(temp_primaryBox,Master,0,RESY_UI,false);
		int dispRet = displayFracImage(Primary_Image,&primaryRenderData);
		printfChange(int,dispRet,"\ndisplayFracImage: %d",dispRet);
		if (exportFractalBuffer == true) {
			uint64_t curTime = getNanoTime();
			size_t size = snprintf(NULL,0,"%s_%llu",frac.type_name,curTime);
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
}