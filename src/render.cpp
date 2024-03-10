/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "render.h"
//#include "glslFrac.h"
//#define SDL_MAIN_HANDLED

//#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <chrono>

#ifndef DISABLE_MULTITHREADING
	//#include <thread>
	#include "mingw.thread.h"
#endif

//#include "fracSingle.h"
#include "fracMulti.h"
//#include "fracCL.h"
#include "fileManager.h"

#include "primeUX.h"
#include "graphics2D.h"

//Debug integer scaling
#define scale 1
//Default starting resolution
#define RESX_Default 640
#define RESY_Default 480
//Minimum allowed resolution
#define RESX_Minimum 640
#define RESY_Minimum 360
//Maximum allowed resolution (To avoid allocating way too much resources)
#define RESX_Maximum 8192
#define RESY_Maximum 4608

#define RESX_Margin 8 // 8
#define RESY_Margin 78 // Tab(30) + Taskbar(40) + 8

// Rendering
SDL_Texture* texture;
SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

void* VRAM_Master;
uint32_t RESX_Master;
uint32_t RESY_Master;

uint8_t* VRAM_Render;
uint32_t RESX_Render;
uint32_t RESY_Render;

uint8_t* VRAM_UI;
uint32_t RESX_UI;
uint32_t RESY_UI;

// IO
const uint8_t* KEYS;
uint32_t threadCount = 1;
uint8_t UI_Camera = 0;

uint64_t systemTime = 0;
uint64_t frameTime = 0;
fp64 currentTime = 0.0;
fp64 deltaTime = 0.0;

fp64 getDeltaTime() {
	return deltaTime;
}
fp64 getCurrentTime() {
	return currentTime;
}

uint64_t getNanoTime() {
	using nanoseconds = std::chrono::duration<std::uint64_t, std::nano>;
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<nanoseconds>(now.time_since_epoch()).count();
}
fp64 getDecimalTime() {
	return (fp64)getNanoTime() / 1.0e9;
}

void updateTime() {
	uint64_t clockTime = getNanoTime();
	frameTime = (clockTime - systemTime);
	deltaTime = (fp64)(frameTime) / 1.0e9;
	systemTime = clockTime;
	currentTime = (fp64)(systemTime) / 1.0e9;
}

fp64 maxDisplayTime = 0.0;
fp64 maxTime = 0.0;
fp64 maxResetTime = 0.0;
void printFPS() {
	//setPrintInt(VRAM_UI,RESX_UI,RESY_UI);
	//printFloat(1.0/maxDisplayTime,4,2,10,285,8);
	
	if (deltaTime > maxTime) {
		maxTime = deltaTime;
	}
	if (currentTime - maxResetTime > 0.2) {
		maxDisplayTime = maxTime;
		maxTime = 0.0;
		maxResetTime = currentTime;
	}
}

uint8_t timerReady(Timer* timer) {
	if (systemTime - timer->cycle >= timer->reset) {
		timer->reset = systemTime;
		return 1;
	}
	return 0;
}

void setTimer(Timer* timer, fp64 cycle) { //Time in Seconds
	timer->cycle = (uint64_t)(cycle * 1.0e9);
	timer->reset = getNanoTime();
}

fp64 readTimer(Timer* timer) { //Time elasped in Seconds
	return (fp64)(systemTime - timer->reset) / 1.0e9;
}

void newFrame() {
	do {
		frameTime = getNanoTime() - systemTime;
	} while (frameTime < (uint64_t)((1.0/240.00390625) * 1.0e9));
	deltaTime = (fp64)(frameTime) / 1.0e9;
	systemTime = getNanoTime();
	currentTime = (fp64)(systemTime) / 1.0e9;
}


// Functions

void crashScreen(const char* errorMessage) {
	printf("\n\n%s\n",errorMessage);
	fp64 crashTimer = getDecimalTime(); fflush(stdout);
	while (1) {
		if (getDecimalTime() - crashTimer > 3.0) {
			break;
		}
	}
	exit(1);
}

uint8_t windowResizingCode();
uint8_t getVal();
void renderFrame(frac* parameter);
void blitBuffer(uint8_t* buffer, u32 x0, u32 y0, u32 x1, u32 y1); // Cord, Size

//Renders
void renderFractal(frac* parameter);
//void renderFormula_MultiThread(frac* parameter, uint8_t* data);
//void renderFormula_OpenCL(frac* parameter, uint8_t* data);

enum RenderType {SinglePrecision,DoublePrecision};
enum RenderType renderType = DoublePrecision;

bool halfThreads = false;

void valLimitInclusive(fp64* val, fp64 min, fp64 max) { //Inclusive
	if (*val < min) { *val = min; } else if (*val > max) { *val = max; }
}
void valLimitExclusive(fp64* val, fp64 min, fp64 max) { //Exclusive
	if (*val <= min) { *val = fpInc(min); } else if (*val >= max) { *val = fpInc(max); }
}

Timer keyInputTimer;
Timer numberInputTimer;
Timer imageSaveTimer;
Timer cameraUIColorTimer;
Timer updateTimer;
uint64_t numberAcc = 0;
uint8_t numberInputSelect = 0;

uint64_t getFractalFamilySize(uint8_t input_type) {
	if (input_type == 2) {
		return 8; // Bit 3
	} else if (input_type == 3) {
		return 64; // Bit 6
	} else if (input_type == 4) {
		return 128; // Bit 7
	} else if (input_type == 5) {
		return 256; // Bit 8
	}
	return 1;
}

uint64_t clampFractalFormula(uint8_t input_type, uint64_t input_formula) {
	if (input_type == 2) {
		input_formula %= 256;
	} else if (input_type == 3) {
		input_formula %= 16384;	
	} else if (input_type == 4) {
		input_formula %= 131072;
	} else if (input_type == 5) {
		input_formula %= 1048576;
	}
	return input_formula;
}

void renderLoop() {
	frac input = {
		2,0,0x0, // type, formula, julia
		0.0,0.0,-0.2, //r, i, zoom
		128, //itr
		0.0,0.0 // zr, zi
	};
	renderFractal(&input);
	uint8_t update;
	while (1) {
		scanKeyboard();
		if (SDL_PollEvent(&event)) {
			if( event.type == SDL_QUIT ) {
				return;
			}
		}
		
		fp64 moveDelta = deltaTime < 0.2 ? deltaTime : 0.2;
		
		if (keyPressed(SDL_SCANCODE_W) || keyPressed(SDL_SCANCODE_UP)) {
			input.i += 0.75 * pow(10,-input.zoom) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_S) || keyPressed(SDL_SCANCODE_DOWN)) {
			input.i -= 0.75 * pow(10,-input.zoom) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_A) || keyPressed(SDL_SCANCODE_LEFT)) {
			input.r -= 0.75 * pow(10,-input.zoom) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_D) || keyPressed(SDL_SCANCODE_RIGHT)) {
			input.r += 0.75 * pow(10,-input.zoom) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_E)) {
			input.zoom += 0.2 * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_Q)) {
			input.zoom -= 0.2 * moveDelta;
			update = 1;
		}
		
		if (keyPressed(SDL_SCANCODE_J)) {
			input.zr -= 0.3 * pow(10.0,-input.zoom / 2.0) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_L)) {
			input.zr += 0.3 * pow(10.0,-input.zoom / 2.0) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_U)) {
			input.zr = 0.0;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_I)) {
			input.zi += 0.3 * pow(10.0,-input.zoom / 2.0) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_K)) {
			input.zi -= 0.3 * pow(10.0,-input.zoom / 2.0) * moveDelta;
			update = 1;
		}
		if (keyPressed(SDL_SCANCODE_O)) {
			input.zi = 0.0;
			update = 1;
		}
		
		if (timerReady(&keyInputTimer)) {
			if (keyPressed(SDL_SCANCODE_C)) { //Julia
				input.julia ^= 0x1;
				update = 1;
			}
			if (keyPressed(SDL_SCANCODE_V)) { //Z0 Toggle
				input.julia ^= 0x2;
				update = (input.julia & 0x1) ? update : 1;
			}
			if (keyPressed(SDL_SCANCODE_B)) { // Half-Threads Toggle
				halfThreads = !halfThreads;
			}
			if (keyPressed(SDL_SCANCODE_Z)) { //Formula
				numberAcc = 0;
				numberInputSelect = 2; 
			}
			if (keyPressed(SDL_SCANCODE_X)) { //Type
				numberInputSelect = 1;
			}
			if (keyPressed(SDL_SCANCODE_T)) { input.itr *= 2; input.itr = (input.itr <= 131072) ? input.itr : 131072; update = 1; }
			if ((keyPressed(SDL_SCANCODE_RETURN) || (keyPressed(SDL_SCANCODE_Z) && numberAcc != 0)) && numberInputSelect == 2) { //Formula
				input.formula = clampFractalFormula(input.type,input.formula);
				input.formula = numberAcc;
				numberAcc = 0;
				numberInputSelect = 0;
				update = 1;
			}
			uint8_t n = getVal();
			if (numberInputSelect == 1 && n != 0xFF) {
				if (n >= 2 && n <= 5) {
					input.type = n;
					update = 1;
				}
				numberInputSelect = 0;
			} else if (numberInputSelect == 2) {
				if (n != 0xFF) {
					numberAcc *= 10; //Decimal
					numberAcc += n;
				}
				if (keyPressed(SDL_SCANCODE_BACKSPACE) || numberAcc >= 1048576) {
					numberAcc /= 10;
				}
			}
			
			// Increment/Decrement Fractal Formula
			if (keyPressed(SDL_SCANCODE_LEFTBRACKET)) {
				input.formula--;
				input.formula = clampFractalFormula(input.type,input.formula);
				update = 1;
			}
			if (keyPressed(SDL_SCANCODE_RIGHTBRACKET)) {
				input.formula++;
				input.formula = clampFractalFormula(input.type,input.formula);
				update = 1;
			}
			if (keyPressed(SDL_SCANCODE_MINUS)) {
				input.formula -= getFractalFamilySize(input.type);
				input.formula = clampFractalFormula(input.type,input.formula);
				update = 1;
			}
			if (keyPressed(SDL_SCANCODE_EQUALS)) {
				input.formula += getFractalFamilySize(input.type);
				input.formula = clampFractalFormula(input.type,input.formula);
				update = 1;
			}
			
			if (keyPressed(SDL_SCANCODE_T)) { input.itr *= 2; input.itr = (input.itr <= 65536) ? input.itr : 65536; update = 1; }
			if (keyPressed(SDL_SCANCODE_G)) { input.itr /= 2; input.itr = (input.itr >= 16) ? input.itr : 16; update = 1; }
			
			if (keyPressed(SDL_SCANCODE_N)) {
				update = (renderType == SinglePrecision) ? update : 1;
				renderType = SinglePrecision;
			} else if (keyPressed(SDL_SCANCODE_M)) {
				update = (renderType == DoublePrecision) ? update : 1;
				renderType = DoublePrecision;
			}
			
			if (keyPressed(SDL_SCANCODE_R)) { //Reset
				input.r = 0.0;
				input.i = 0.0;
				input.zoom = -0.2;
				update = 1;
			}
		}
		
		if (keyPressed(SDL_SCANCODE_P) && timerReady(&imageSaveTimer)) { //Save Image
			setTimer(&cameraUIColorTimer, 1.0);
			UI_Camera = 1;
			char tempName[64];
			time_t timeFormat;
			timeFormat = time(NULL);
			struct tm tm = *localtime(&timeFormat);
			sprintf(tempName,"Fractal-id-%ld-time-%04d-%02d-%02d_%02d-%02d-%02d",input.formula,tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
			writeImage(tempName,VRAM_Render,RESX_Render,RESY_Render);
		}
		if (UI_Camera == 1 && timerReady(&cameraUIColorTimer)) {
			UI_Camera = 0;
		}

		if (update || windowResizingCode()) { //If a parameter was changed, or the window size changed
			update = 0;
			
			valLimitExclusive(&input.i,-10.0,10.0); valLimitExclusive(&input.r,-10.0,10.0);
			valLimitExclusive(&input.zoom,-10.0,100.0);
			valLimitInclusive(&input.zr,-2.0,2.0); valLimitInclusive(&input.zi,-2.0,2.0);
			
			renderFractal(&input);
		}
		renderFrame(&input);
		newFrame();
	}
}

void scanKeyboard() {
	if (SDL_PollEvent(&event)) {
		KEYS = SDL_GetKeyboardState(NULL);
	}
	i32 mX = 0;
	i32 mY = 0;
	i32 mB = SDL_GetMouseState(&mX, &mY);
	mX /= scale;
	mY /= scale;
	updateMouse(mX, mY,mB);
}

uint8_t keyPressed(uint32_t key) {
	if (KEYS[key] != 0) {
		return 1;
	}
	return 0;
}

uint8_t getVal() {
	u8 value = 0xFF;
	//if (timerReady(&numberInputTimer)) {
		if (keyPressed(SDL_SCANCODE_9)) { value = 0x9; }
		if (keyPressed(SDL_SCANCODE_8)) { value = 0x8; }
		if (keyPressed(SDL_SCANCODE_7)) { value = 0x7; }
		if (keyPressed(SDL_SCANCODE_6)) { value = 0x6; }
		if (keyPressed(SDL_SCANCODE_5)) { value = 0x5; }
		if (keyPressed(SDL_SCANCODE_4)) { value = 0x4; }
		if (keyPressed(SDL_SCANCODE_3)) { value = 0x3; }
		if (keyPressed(SDL_SCANCODE_2)) { value = 0x2; }
		if (keyPressed(SDL_SCANCODE_1)) { value = 0x1; }
		if (keyPressed(SDL_SCANCODE_0)) { value = 0x0; }
	//}
	if (value < 10) { //Base 10
		return value;
	}
	return 0xFF;
}

uint8_t status() {
	if (SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {
			return 1;
        }
	}
	return 0;
}
struct _displayInfo {
	uint32_t resX;
	uint32_t resY;
	uint32_t refreshRate;
	uint8_t bbp;
	char name[11];
}; typedef struct _displayInfo displayInfo;
void setDisplayInfo(displayInfo* display, uint32_t resX, uint32_t resY, uint32_t refreshRate, uint8_t bbp, const char* name) {
	display->resX = resX;
	display->resY = resY;
	display->refreshRate = refreshRate;
	display->bbp = bbp;
	snprintf(display->name, 11, "%s", name);
}
displayInfo* displayList;

void initLCDcontroller() {
	// Bootup
	printf("\nVersion 1.0.2 | 2023/08/17 | zerico2005"); fflush(stdout);
	SDL_Init(SDL_INIT_VIDEO);
	// Retrive Display Information
	int32_t displayCount = SDL_GetNumVideoDisplays();
	displayList = (displayInfo*)malloc(sizeof(displayInfo) * displayCount);
	uint32_t displaySelection = 0;
	int32_t setX = 0, setY = 0;
	if (displayCount == 0) {
		setX = RESX_Minimum;
		setY = RESY_Minimum;
	} else {
		for (u8 i = 0; i < displayCount; i++) {
			SDL_DisplayMode mode;
			SDL_GetDesktopDisplayMode(i,&mode);
			printf("\nDisplay(%d): %dx%d at %dHz | %s",i,mode.w,mode.h,mode.refresh_rate,SDL_GetDisplayName(i)); fflush(stdout); //SDL_GetPixelFormatName(mode.format)
			setDisplayInfo(&displayList[i],mode.w,mode.h,mode.refresh_rate,SDL_BITSPERPIXEL(mode.format),SDL_GetDisplayName(i));
		}
		/*
		setX = displayList[displaySelection].resX; setY = displayList[displaySelection].resY;
		if (setX < (RESX_Minimum - 16) || setY < (RESY_Minimum - 16)) { //Should crash below 624x384
			crashScreen("CRITICAL ERROR: Screen Resolution is too low to initialize the program\nThe MINIMUM screen resolution to run the program is 640x480");
		}
		setX -= RESX_Margin; setY -= RESY_Margin;
		*/
		setX = displayList[displaySelection].resX - RESX_Margin;
		setY = displayList[displaySelection].resY - RESY_Margin;
		if (setX > RESX_Default) { setX = RESX_Default; }
		if (setY > RESY_Default) { setY = RESY_Default; }
		if (setX < RESX_Minimum) { setX = RESX_Minimum; }
		if (setY < RESY_Minimum) { setY = RESY_Minimum; }
	}
	// Set resolution parameters
	RESX_Master = setX;
	RESY_Master = setY;
	RESX_UI = RESX_Master;
	RESY_UI = 40;
	RESX_Render = RESX_Master;
	RESY_Render = RESY_Master - RESY_UI;
	// Create SDL2 Window
	//SDL_CreateWindowAndRenderer(setX, setY, SDL_WINDOW_RESIZABLE, &window, &renderer);
	window = SDL_CreateWindow("ABS-Fractal-Explorer",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,setX,setY,SDL_WINDOW_RESIZABLE);
	SDL_SetWindowMinimumSize(window,RESX_Minimum,RESY_Minimum);
	SDL_SetWindowMaximumSize(window,RESX_Maximum,RESY_Maximum);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
//	SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
	SDL_RenderSetLogicalSize(renderer, RESX_Master, RESY_Master);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, RESX_Master, RESY_Master);
	// Retrive Processor Information
	#ifndef DISABLE_MULTITHREADING
		auto processor_count = std::thread::hardware_concurrency();
		threadCount = (uint32_t)processor_count;
	#else
		threadCount = 1;
	#endif
	printf("\nCPU Thread-Count: %d",threadCount); fflush(stdout);
		// Initialize OpenCL
		//init_OpenCL(RESX_Render,RESY_Render);
		//queryOpenCL_GPU();
	// Allocate Buffers
	VRAM_Render = (uint8_t*)calloc(RESX_Render * RESY_Render * 3, sizeof(uint8_t));
	VRAM_UI = (uint8_t*)calloc(RESX_UI * RESX_UI * 3, sizeof(uint8_t));
	VRAM_Master = (uint8_t*)calloc(RESX_Master * RESY_Master * 3, sizeof(uint8_t));
	// Set Timers
	setTimer(&keyInputTimer,0.116);
	setTimer(&imageSaveTimer,5.0);
//	setTimer(&numberInputTimer,0.145);
}
void terminateLCDcontroller() {
	SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	//terminate_OpenCL();
	free(VRAM_Master);
	free(VRAM_UI);
	free(VRAM_Render);
	free(displayList);
}

uint8_t windowResizingCode() {
	u8 reVal = 0;
	i32 x,y;
	static i32 rX = 0, rY = 0;
	SDL_GetWindowSize(window,&x,&y);
	
	if ((rX != x || rY != y) && (rX != 0 && rY != 0)) {
		if (x < RESX_Minimum) { x = RESX_Minimum; }
		if (y < RESY_Minimum) { y = RESY_Minimum; }
		if (x > RESX_Maximum) { x = RESX_Maximum; }
		if (y > RESY_Maximum) { y = RESY_Maximum; }
		if (x & 0x3) { x &= 0xFFFFFFFC; }
		SDL_SetWindowSize(window,x,y);
		SDL_RenderSetLogicalSize(renderer, x, y);
		RESX_Master = x;
		RESY_Master = y;
		RESX_UI = RESX_Master;
		RESY_UI = 40;
		RESX_Render = RESX_Master;
		RESY_Render = RESY_Master - RESY_UI;
		
		VRAM_Render = (uint8_t*)realloc(VRAM_Render, RESX_Render * RESY_Render * 3);
		VRAM_UI = (uint8_t*)realloc(VRAM_UI, RESX_UI * RESY_UI * 3);
		VRAM_Master = (uint8_t*)realloc(VRAM_Master, RESX_Master * RESY_Master * 3);
		//printf("\n%d %d",x,y);
		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, RESX_Master, RESY_Master);
		reVal = 1;
	}
	rX = x;
	rY = y;
	return reVal;
}

void overlayPixel(u32 x, u32 y, uint8_t* buffer) {
	if (x >= RESX_Render || y >= RESY_Render) { // Out of bounds
		return;	
	}
	u32 z = ((y * RESX_Render) + x) * 3;
	buffer[z] = (buffer[z] + 255) / 2; // R
	buffer[z+1] = (buffer[z+1] + 255) / 2; // G
	buffer[z+2] = (buffer[z+2] + 255) / 2; // B
}

char* const HeaderText[] = {"Real:","Imag:","Zoom: 10^","Itr:","ZR:","ZI:"};
char* const PowerText[] = {"Quadratic","Cubic","Quartic","Quintic"};
void renderUI(frac* param) {
	char tempString[64];
	#define printAFloat(x,y,z,l) snprintf(tempString,l+1,"%.10lf",z); printText(x,y,tempString);
	#define printAnInt(x,y,z,l) snprintf(tempString,l+1,"%ld",z); printText(x,y,tempString);
	#define printAType(x,y,z,w,l) snprintf(tempString,l+1,w,z); printText(x,y,tempString);
	setBuffer(VRAM_UI,RESX_UI,RESY_UI);
	if (UI_Camera == 0) { setHexColor(0x555555); } else { setHexColor(0x111111); }
	fillScreen();
	if (UI_Camera == 0) { setHexColor(0xBBBBBB); } else { setHexColor(0xAAFFFF); }
	fillRect(6, 6, RESX_Master - 12, 28);
	
	fp64 cXR = ((((fp64)cursorX() - (((fp64)RESX_Render - 1.0f) / 2.0f)) / (((fp64)RESY_Render - 1.0f) / 2.0f)) / pow(10.0f, param->zoom)) + param->r;
	fp64 cYI = -((((fp64)(cursorY()-(i32)RESY_UI) - (((fp64)RESY_Render - 1.0f) / 2.0f)) / (((fp64)RESY_Render - 1.0f) / 2.0f)) / pow(10.0f, param->zoom)) + param->i;
	uint64_t f0 = (numberInputSelect == 2) ? numberAcc : param->formula;
	
	setHexColor(0x000000);
	printText(10,10,"Real:"); printText(136,10,"Zoom:"); printText(241,10,"Zr0:"); printText(346,10,"crsR:"); 
	printText(10,22,"Imag:"); printText(136,22,"Iter:"); printText(241,22,"Zi0:"); printText(346,22,"crsI:"); 
	printAFloat(52,10,param->r,11); printAFloat(178,10,param->zoom,8); printAFloat(276,10,param->zr,9); printAFloat(390,10,cXR,9); 
	printAFloat(52,22,param->i,11); printAType(178,22,param->itr,"%d",8); printAFloat(276,22,param->zi,9); printAFloat(390,22,cYI,9); 
	printText(458,10,"FPS:"); //printText(570,10,"Type:");
	printText(458,22,"mili:"); //printText(570,22,"f-ID:");
	printAType(500,10,1.0 / deltaTime,"%.3lf",7);
	printAType(500,22,deltaTime * 1000.0,"%.3lf",7);
	if (numberInputSelect == 1) {setHexColor(0xAA0000); } printText(556,10,PowerText[param->type - 2]); setHexColor(0x000000);
	printText(556,22,"ID"); if (numberInputSelect == 2) {setHexColor(0xAA0000); } printAnInt(577,22,f0,7); //setHexColor(0x000000);
	//printFPS();
	uint8_t* bufUI = getBuffer();
	for (u32 z = 0; z < RESX_UI * RESY_UI * 3; z++) {
		VRAM_UI[z] = bufUI[z];
	}
}

void blitBuffer(uint8_t* buffer, u32 x0, u32 y0, u32 x1, u32 y1) { // Cord, Size
	if (x0 + x1 > RESX_Master || y0 + y1 > RESY_Master) {
		return;
	}
	u32 z = 0;
	u32 w = ((y0 * RESX_Master) + x0) * 3;
	for (u32 y = 0; y < y1; y++) {
		//	for (u32 x = 0; x < x1; x++) {
		//		((uint8_t*)VRAM_Master)[w] = buffer[z]; z++; w++; // R
		//		((uint8_t*)VRAM_Master)[w] = buffer[z]; z++; w++; // G
		//		((uint8_t*)VRAM_Master)[w] = buffer[z]; z++; w++; // B
		//	}
		memcpy(&(((uint8_t*)VRAM_Master)[w]),&buffer[z],x1 * 3);
		z += x1 * 3;
		w += x1 * 3;
		w += (RESX_Master - x0 - x1);
	}
}

void renderFrame(frac* param) {
		//static int pT;
	//int pitch;
		//if (pitch & 0x3) { pitch &= 0xFFFFFFFC; pitch += 0x4; }
		//texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, RESX_Master, RESY_Master);
		// if (pT != pitch) {
		// 	printf("\nPitch %d", pitch); fflush(stdout);
		// }
		// pT = pitch;
	//SDL_LockTexture(texture, NULL, &VRAM_Master,&pitch);
		//setPrintInt(VRAM_Render,resX,resY);
		//printFloat(1.0 / deltaTime,4,3,10,resX-31,0);
		//printFloat(parameter->r,2,6,10,0,0);
		//printFloat(parameter->i,2,6,10,39,0);
		//printFloat(parameter->zoom,2,3,10,78,0);
		/*
		printInt(cursorX(),4,10,8,30);
		printInt(cursorY()-(i32)UIresY,4,10,8,40);
		fp64 cXR = ((((fp64)cursorX() - (((fp64)RESX_Render - 1.0f) / 2.0f)) / (((fp64)RESY_Render - 1.0f) / 2.0f)) / pow(10.0f, parameter->zoom)) + parameter->r;
		fp64 cYI = -((((fp64)(cursorY()-(i32)UIresY) - (((fp64)RESY_Render - 1.0f) / 2.0f)) / (((fp64)RESY_Render - 1.0f) / 2.0f)) / pow(10.0f, parameter->zoom)) + parameter->i;
		printFloat(cXR,2,6,10,52,30);
		printFloat(cYI,2,6,10,52,40);
		*/
		//printf("\n%p\nRes: %ux%u\n",VRAM_Master,RESX_Master,RESY_Master);
		//fflush(stdout);
	blitBuffer(VRAM_Render,0,RESY_UI,RESX_Render,RESY_Render);
	renderUI(param);
	//memset((uint8_t*)VRAM_Master,0xA0,RESX_Master * RESY_Master);
	//printf("\n%p\nRes: %ux%u\n",VRAM_Master,RESX_Master,RESY_Master);
	blitBuffer(VRAM_UI,0,0,RESX_UI,RESY_UI);
	//memcpy(&(((uint8_t*)VRAM_Master)[0]),VRAM_UI,(RESX_UI * 3) * RESY_UI);
	
	
	SDL_UpdateTexture(texture, NULL, VRAM_Master, RESX_Master * 3); 
	{
		SDL_Rect srcRect = {0,0,(int)RESX_Master,(int)RESY_Master};
		SDL_Rect dstRect = {0,0,(int)RESX_Master,(int)RESY_Master};
		SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	}
	//SDL_UnlockTexture(texture);
	//SDL_RenderCopy(renderer, texture, NULL, NULL);
		//SDL_DestroyTexture(texture);
	SDL_RenderPresent(renderer);
}

void renderFractal(frac* param) {
	uint32_t renderThreadCount = threadCount; // Removed the threadCount * 4 thing
	if (halfThreads == true) {
		renderThreadCount = (threadCount <= 1) ? 1 : (threadCount / 2);
	}
	
	if (renderType == SinglePrecision) {
		renderFormula_MultiThread(param,VRAM_Render, RESX_Render, RESY_Render, renderThreadCount,32);
	} else if (renderType == DoublePrecision) {
		renderFormula_MultiThread(param,VRAM_Render, RESX_Render, RESY_Render, renderThreadCount,64);
	}
	
	//Crosshair
	for (u32 x = (RESX_Render / 2) - 9; x <= (RESX_Render / 2) - 4; x++) {
		overlayPixel(x,(RESY_Render / 2) - 1,VRAM_Render);
		overlayPixel(x,(RESY_Render / 2),VRAM_Render);
	}
	for (u32 x = (RESX_Render / 2) + 3; x <= (RESX_Render / 2) + 8; x++) {
		overlayPixel(x,(RESY_Render / 2) - 1,VRAM_Render);
		overlayPixel(x,(RESY_Render / 2),VRAM_Render);
	}
	for (u32 y = (RESY_Render / 2) - 9; y <= (RESY_Render / 2) - 4; y++) {
		overlayPixel((RESX_Render / 2) - 1,y,VRAM_Render);
		overlayPixel((RESX_Render / 2),y,VRAM_Render);
	}
	for (u32 y = (RESY_Render / 2) + 3; y <= (RESY_Render / 2) + 8; y++) {
		overlayPixel((RESX_Render / 2) - 1,y,VRAM_Render);
		overlayPixel((RESX_Render / 2),y,VRAM_Render);
	}

	if (param->zr != 0.0 || param->zi != 0.0) {
		u32 pX = (u32)((param->zr - param->r) * pow(10.0, param->zoom) / 2.0 * (((fp64)RESY_Render - 1.0))) + (((fp64)RESX_Render - 1.0) / 2.0);
		u32 pY = (u32)(-(param->zi - param->i) * pow(10.0, param->zoom) / 2.0 * (((fp64)RESY_Render - 1.0))) + (((fp64)RESY_Render - 1.0) / 2.0);
		//printf("\n%d %d",pX,pY); fflush(stdout);
		for (i32 pV = -3; pV <= 3; pV++) {
			for (i32 pU = -3; pU <= 3; pU++) {
				overlayPixel(pX+pU,pY+pV,VRAM_Render);
			}
		}
	}
	//renderUI(VRAM_Render, parameter);
}