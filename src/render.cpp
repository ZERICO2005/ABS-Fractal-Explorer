/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "render.h"

#include "copyBuffer.h"
#include "fractal.h"
#include "keybind.h"
#include "engine.h"

#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

SDL_Renderer* renderer;
SDL_Window* window;
ImGuiIO* io_IMGUI;

SDL_Texture* texture;
SDL_Texture* kTexture; // Keyboard graphic
BufferBox Master; /* Do NOT manually malloc/realloc Master.vram */
BufferBox TestGraphic;
fp64 TestGraphicSpeed = 0.4;

//#define MANUAL_FRAME_RATE_OVERRIDE
fp64 FRAME_RATE = 60.0; // Double the max screen refresh rate
const fp64 FRAME_RATE_OFFSET = 0.01;
uint64_t FRAME_RATE_NANO;
#define  Default_Frame_Rate_Multiplier 1.0
const uint8_t color_square_divider = 3; //5 dark, 4 dim, 3 ambient, 2 bright, 1 the sun
fp64 DeltaTime = 0.0;
uint64_t END_SLEEP_HEADROOM = SECONDS_TO_NANO(0.02);

fp64 Frame_Time_Display = 0.0;

fp64 r = 0.0;
fp64 i = 0.0;
fp64 zoom = 0.0;
uint32_t RESY_UI = 120;

#define RESX_Default 800
#define RESY_Default 600
#define RESX_Minimum 400
#define RESY_Minimum 320
#define RESX_Maximum 8192
#define RESY_Maximum 4608
#define RESX_Margin 16
#define RESY_Margin 16

const uint8_t* KEYS;

struct _Key_Status {
	SDL_Scancode key;
	bool pressed;
	uint64_t timePressed;
	uint64_t timeReleased;
}; typedef struct _Key_Status Key_Status;

Key_Status Key_List[SDL_NUM_SCANCODES];

void updateKeys() {
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
	}
}

void initKeys() {
	{
		using namespace Key_Function;
		initKeyboardGraphics(0.0,0.0,0.5);
		//size_t function_map[] = {NONE,COORDINATES,TRANSFORMATIONS,JULIA,PARAMETERS,POLAR,FORMULA,SCREEN_SPLIT,FUNCTIONS,RENDERING};
		struct init_key_HSV {
			enum Key_Function_Enum type;
			fp64 h; fp64 s; fp64 v;
		}; typedef init_key_HSV init_key_HSV;
		init_key_HSV InitKeyHSV[] = {
			{NONE,0.0,0.0,0.5},
			{COORDINATES,0.0,0.6,1.0},{TRANSFORMATIONS,40.0,0.6,1.0},{JULIA,80.0,0.6,1.0},
			{PARAMETERS,120.0,0.6,1.0},{POLAR,160.0,0.6,1.0},{FORMULA,200.0,0.6,1.0},
			{SCREEN_SPLIT,240.0,0.6,1.0},{FUNCTIONS,280.0,0.6,1.0},{RENDERING,320.0,0.6,1.0},
		};
		struct init_key_RGB {
			enum Key_Function_Enum type;
			uint8_t r; uint8_t g; uint8_t b;
		}; typedef init_key_RGB init_key_RGB;
		init_key_RGB InitKeyRGB[ARRAY_LENGTH(InitKeyHSV)];
		for (size_t i = 0; i < ARRAY_LENGTH(InitKeyHSV); i++) {
			getRGBfromHSV(&(InitKeyRGB[i].r),&(InitKeyRGB[i].g),&(InitKeyRGB[i].b),InitKeyHSV[i].h,InitKeyHSV[i].s,InitKeyHSV[i].v);
		}
		for (size_t s = 0; s < SDL_NUM_SCANCODES; s++) {
			size_t r = s % ARRAY_LENGTH(InitKeyRGB);
			setRGB_Scancode(InitKeyRGB[r].r,InitKeyRGB[r].g,InitKeyRGB[r].b,(SDL_Scancode)s);
		}
	}
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

bool windowResizingCode() {
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

const char* buttonLabels[] = {"Fractal", "Export","Import", "Screenshot", "Rendering", "Settings", "Keybinds", "Abort"};
int buttonSelection = -1;
bool ShowTheXButton = true;
//bool yeildSwitch = true;

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

void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
    ImGui::Begin("Horizontal Button Page", NULL, window_flags);
	ImGui::Text("%.2lfFPS",1.0 / Frame_Time_Display);
	ImGui::SameLine();
	ImGui::Text("%.2lfms",Frame_Time_Display * 1000.0);
    ImGui::Text("Button: %d",buttonSelection); ImGui::SameLine();
    size_t buttonCount = sizeof(buttonLabels) / sizeof(buttonLabels[0]);
    for (size_t i = 0; i < buttonCount; i++) {
        if (i != 0) { ImGui::SameLine(); }
        if (ImGui::Button(buttonLabels[i])) {
			if (buttonSelection == (int)i) {
				buttonSelection = -1;
			} else {
				buttonSelection = i;
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
    // End the ImGui window
    ImGui::End();
}

void Menu_Fractal() {
	ImGui_DefaultWindowSize(Master.resX,16,240,400,0.7,Master.resY,16,160,320,0.7);
	static const char* juliaBehaviour[] = {"Independant Movement","Copy Movement","Cordinates follow Z Value","Z Value follows Coordinates"};
	static int32_t input_power = 2;
	static fp64 input_polar_power = 2.0; static fp32 temp_input_polar_power = (fp32)input_polar_power;
	static bool juliaSet = false;
	static bool startingZ = false;
	static bool swapJuliaSplit = false;
	static bool cursorZValue = false;
	static bool relativeZValue = false;
	static bool showFloatingJulia = true;
	static bool adjustZoomToPower = false;
	static bool lockToCardioid = false;
	static bool flipCardioidSide = false;
	static fp32 temp_input_breakoutValue = 16.0;
	static fp64 input_breakoutValue = pow(2.0,(fp64)temp_input_breakoutValue);
	ImGui::Begin("Fractal Menu",&ShowTheXButton);
	static int Combo_FractalType = 0;
	ImGui::Text("Fractal Type:");
    if (ImGui::Combo("##fractalType", &Combo_FractalType, BufAndLen(FractalTypeText))) {

    }
	ImGui::Separator();
	if (Combo_FractalType == 0 || Combo_FractalType == 1) { /* ABS and Polar */
		fp64 maxRadius = getABSFractalMaxRadius((Combo_FractalType == 0) ? (fp64)input_power : input_polar_power);
		fp64 minRadius = getABSFractalMinRadius((Combo_FractalType == 0) ? (fp64)input_power : input_polar_power);
		if (keyPressed(SDL_SCANCODE_A)) {
			ImGui::Text("Pressed for: %lfms",(fp64)(getNanoTime() - Key_List[SDL_SCANCODE_A].timePressed) * 1e-6);
		}
		ImGui::Text("Fractal Radius: %.6lg",maxRadius);
		ImGui::Text("Cardioid Location: %.6lg",minRadius);
		if (Combo_FractalType == 0) {
			ImGui::Text("Fractal Power: %s",getPowerText(input_power));
			ImGui::InputInt("##input_power",&input_power,1,1); valueLimit(input_power,2,10);
		} else {
			ImGui::Text("Fractal Power: %s",getPowerText(round(input_polar_power)));
			ImGui::SliderFloat("##input_polar_power",&temp_input_polar_power,1.0001,10.0,"%.4f"); input_polar_power = (fp64)temp_input_polar_power;
			ImGui::Checkbox("Lock position to Cardioid",&lockToCardioid);
			if (lockToCardioid) {
				ImGui::Checkbox("Flip Cardioid position",&flipCardioidSide);
			}
		}
		ImGui::Checkbox("Adjust zoom value to power",&adjustZoomToPower);
		if (input_breakoutValue < 100.0) {
			ImGui::Text("Breakout Value: %.3lf",input_breakoutValue);
		} else {
			ImGui::Text("Breakout Value: %.1lf",input_breakoutValue);
		}
		ImGui::SliderFloat("##input_breakoutValue",&temp_input_breakoutValue,-2.0,24.0,""); input_breakoutValue = pow(2.0,(fp64)temp_input_breakoutValue);
		ImGui::Separator();
		ImGui::Text("Julia Set Options:");
		ImGui::Checkbox("Render Julia Set",&juliaSet);
		ImGui::Checkbox("Toggle starting Z values",&startingZ);
		ImGui::Checkbox("Use Cursor for Z values",&cursorZValue);
		if (cursorZValue) { ImGui::Checkbox("Use relative Z values",&relativeZValue); }
		ImGui::Separator();
		static int Combo_JuliaSplit = 1;
		ImGui::Text("Split Screen:");
		if (ImGui::Combo("##juliaScreen", &Combo_JuliaSplit, BufAndLen(WindowDivider))) {
			if (Combo_JuliaSplit == 7) { /* Floating */
				showFloatingJulia = true;
			}
		}
		if (Combo_JuliaSplit == 7) { /* Floating */
			ImGui::Checkbox("Show floating Julia Set window",&showFloatingJulia);
		}
		ImGui::Checkbox("Swicth Mandelbrot and Julia Set",&swapJuliaSplit);
		static int Combo_JuliaBehaviour = 0;
		ImGui::Text("Julia Set behaviour:");
		if (ImGui::Combo("##juliaBehaviour", &Combo_JuliaBehaviour, BufAndLen(juliaBehaviour))) {

		}
	} else if (Combo_FractalType == 2) { /* Sierpinski Carpet */
		static bool wallisSieve = false;
		static bool renderOutOfBounds = false;
		static bool fixateOnCorner = false;
		static fp64 squareSize = 1.0; fp32 temp_squareSize = (fp32)squareSize;
		ImGui::Checkbox("Wallis Sieve",&wallisSieve);
		ImGui::Checkbox("Render out of bounds",&renderOutOfBounds);
		ImGui::Checkbox("Fixate on top-left corner",&fixateOnCorner);
		ImGui::Text("Square Size Multiplier:");
		ImGui::SliderFloat("##input_squareSize",&temp_squareSize,1.0e-4,1.0,"%.4f"); squareSize = (fp64)temp_squareSize;
	}

	ImGui::End();
}

void Menu_Rendering() {
	ImGui_DefaultWindowSize(Master.resX,16,240,400,0.7,Master.resY,16,160,320,0.7);
	static const char* CPU_RenderingModes[] = {"fp32 | 10^5.7","fp64 | 10^14.4 (Default)","fp80 | 10^7.7","fp128 | 10^32.5"};
	static const char* GPU_RenderingModes[] = {"fp16 | 10^1.8","fp32 | 10^5.7 (Default)","fp64 | 10^14.4"};
	static int input_subSample = 1;
	static int input_superSample = 1;
	int CPU_ThreadCount = (int)std::thread::hardware_concurrency();
	static int input_CPU_MaxThreads = CPU_ThreadCount;
	static int input_CPU_ThreadMultiplier = 4;
	static int Combo_CPU_RenderingMode = 1;
	static int Combo_GPU_RenderingMode = 1;


	ImGui::Begin("Rendering Menu",&ShowTheXButton);
	
	ImGui::Text("CPU Rendering Mode:");
	if (ImGui::Combo("##CPU_RenderingMode", &Combo_CPU_RenderingMode, BufAndLen(CPU_RenderingModes))) {

	}
	ImGui::Text("Maximum Threads:");
	ImGui::SliderInt("##input_CPU_MaxThreads",&input_CPU_MaxThreads,1,CPU_ThreadCount);
	ImGui::Text("Thread Multiplier:");
	ImGui::SliderInt("##input_CPU_ThreadMultiplier",&input_CPU_ThreadMultiplier,1,32);
	
	ImGui::Text("GPU Rendering Mode:");
	if (ImGui::Combo("##GPU_RenderingMode", &Combo_GPU_RenderingMode, BufAndLen(GPU_RenderingModes))) {
		
	}
	ImGui::Text("Sub Sample:");
	ImGui::SliderInt("##input_subSample",&input_subSample,1,24);
	ImGui::Text("Samples per pixel:");
	ImGui::SliderInt("##input_superSample",&input_superSample,1,24);
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
	if (Combo_initMonitorLocation == 3) {
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
	ImGui::End();
}

void Menu_Keybinds() {
	static int Combo_keyboardSize = 0;
	static const char* keyboardSizeText[] = {
		"ANSI (Default)","Extended","Complete"
	};
	static bool displayNumpad = true;
	ImGui_DefaultWindowSize(Master.resX,16,320,480,0.7,Master.resY,16,240,360,0.7);
	ImGui::Begin("Keybinds Menu",&ShowTheXButton);
	ImGui::Text("Keyboard:");
	if (ImGui::Combo("##keyboardSize", &Combo_keyboardSize, BufAndLen(keyboardSizeText))) {
		
	}
	ImGui::Checkbox("Display Numberpad",&displayNumpad);
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

		renderKeyboard(
			&kBuf, kResX, 5.75, 12.0,
			(uint8_t)Combo_keyboardSize, displayNumpad,
			kCurX,kCurY,((clickState & 0x1) ? true : false)
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
		ImGui::Text("size = %d x %d", kBuf.resX, kBuf.resY);
		ImGui::Image((void*)kTexture, ImVec2(kBuf.resX, kBuf.resY));
		ImGui::Text("Cursor Position: %d,%d",kCurX,kCurY);
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
			case 0:
			Menu_Fractal();
			break;
			case 1:
			//Export_FracExp();
			break;
			case 2:
			//Import_FracExp();
			break;
			case 3:
			//Take_Screenshot();
			break;
			case 4:
			Menu_Rendering();
			break;
			case 5:
			Menu_Settings();
			break;
			case 6:
			Menu_Keybinds();
			break;
			case 7:
			//yeildSwitch = (yeildSwitch == true) ? false : true;
			//Abort_Rendering();
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

int start_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
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
	TimerBox maxFrameReset = TimerBox(1.0/6.0); /* Keeps track of longest frame times */
	TimerBox quitTimer = TimerBox(6.1);
	while (QUIT_FLAG == false) {
		SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
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
			{
				static fp64 maxFrameTime = 0.0;
				if (maxFrameReset.timerReset()) {
					Frame_Time_Display = maxFrameTime;
					maxFrameTime = 0.0;

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
			}
			if (FRAME_RATE_NANO < 2 * yeildTimeNano) {
				yeildTimeNano = 0;
			} else {
				yeildTimeNano = FRAME_RATE_NANO - yeildTimeNano;
			}
			windowResizingCode();
			newFrame();
		}
		
		if (quitTimer.timerReset()) {
			//QUIT_FLAG = true;
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

int init_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
	SDL_Init(SDL_INIT_VIDEO);
	printf("\nSystem Information:");
	int32_t initResX, initResY, initPosX, initPosY;
	if (setupDisplayInfo(&initResX,&initResY,&initPosX,&initPosY) < 0) {
		printCriticalError("init_Render failed to setup Display Info");
	}

	initResX -= RESX_Margin;
	if (initResX > RESX_Default) {
		initResX = RESX_Default;
	} else if (initResX < RESX_Minimum) {
		initResX = RESX_Minimum;
	}
	initResY -= RESY_Margin;
	if (initResY > RESY_Default) {
		initResY = RESY_Default;
	} else if (initResY < RESY_Minimum) {
		initResY = RESY_Minimum;
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
	initBufferBox(&Master,NULL,initResX,initResY,3);
	initBufferBox(&TestGraphic,NULL,Master.resX,Master.resY - RESY_UI,3);
	TestGraphic.vram = (uint8_t*)malloc(getBufferBoxSize(&TestGraphic));
	window = SDL_CreateWindow("Easy_GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Master.resX, Master.resY, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, Master.resX, Master.resY);
	// IMGUI
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_IMGUI = &ImGui::GetIO();
    io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark(); // ImGui::StyleColorsLight();
	ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	
	// printf("\nInit_Render: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	initKeys();
	start_Render(QUIT_FLAG,Console_Mutex);
	return 0;
}

int terminate_Render() {
	terminateKeyboardGraphics();
	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
	FREE(DisplayList);
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

void renderTestGraphic(fp64 cycleSpeed, fp64 minSpeed, fp64 maxSpeed) {
	static fp64 f = 0.0;
	fp64 halfDiff = (maxSpeed - minSpeed) / 2.0;
	fp64 speed = halfDiff * sin(cycleSpeed * TAU * getDecimalTime()) + minSpeed + halfDiff;
	f += DeltaTime * speed;
	uint32_t w = (uint32_t)(f * (256.0));
	size_t z = 0;
	for (uint32_t y = 0; y < TestGraphic.resY; y++) {
		for (uint32_t x = 0; x < TestGraphic.resX; x++) {
			TestGraphic.vram[z] = (x - w) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = (w - y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= color_square_divider; z++;
		}
	}

}

void newFrame() {

	SDL_SetRenderDrawColor(renderer, 6, 24, 96, 255); // Some shade of dark blue
    SDL_RenderClear(renderer);

	int pitch;
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
	void* SDL_Master_VRAM = (void*)Master.vram;
	SDL_LockTexture(texture, NULL, &SDL_Master_VRAM,&pitch);
	Master.vram = (uint8_t*)SDL_Master_VRAM;
	
	renderTestGraphic(0.1, 0.3, 1.0);
	copyBuffer(TestGraphic,Master,0,RESY_UI,false);	
	/*
	if (buf == NULL && buf->vram == NULL) { 
		
	} else {
		copyBuffer(buf,Master,false);	
	}
	*/

	SDL_UnlockTexture(texture);
	{
		SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_Rect dstRect = {0,0,(int)Master.resX,(int)Master.resY};
		SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	}

	SDL_DestroyTexture(texture);
	render_IMGUI();
	SDL_RenderPresent(renderer);
}