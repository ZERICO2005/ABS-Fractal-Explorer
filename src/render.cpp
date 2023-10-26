/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "render.h"

#include "copyBuffer.h"
#include "fractal.h"

#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

SDL_Renderer* renderer;
SDL_Window* window;
ImGuiIO* io_IMGUI;

SDL_Texture* texture;
BufferBox Master; /* Do NOT manually malloc/realloc Master.vram */
BufferBox TestGraphic;
fp64 TestGraphicSpeed = 0.4;

fp64 FRAME_RATE = 120.0; // Double the max screen refresh rate
fp64 DeltaTime = 0.0;
uint64_t END_SLEEP_HEADROOM = SECONDS_TO_NANO(0.02);

fp64 r = 0.0;
fp64 i = 0.0;
fp64 zoom = 0.0;
uint32_t RESY_UI = 120;

static const char* WindowDivider[] = {"Fullscreen","Split Vertical","Split Horizontally","NE Corner","NW Corner","SE Corner","SW Corner","Floating"};

bool windowResizingCode() {
	#define RESX_Minimum 320
	#define RESY_Minimum 240
	#define RESX_Maximum 8192
	#define RESY_Maximum 4608
	bool reVal = false;
	int32_t x = 0, y = 0;
	static int32_t rX = 0, rY = 0;
	SDL_GetWindowSize(window,&x,&y);
	
	if ((rX != x || rY != y) && (rX != 0 && rY != 0)) {
		if (x < RESX_Minimum) { x = RESX_Minimum; }
		if (y < RESY_Minimum) { y = RESY_Minimum; }
		if (x > RESX_Maximum) { x = RESX_Maximum; }
		if (y > RESY_Maximum) { y = RESY_Maximum; }
		if (x & 0x3) { x &= 0xFFFFFFFC; }
		
		SDL_SetWindowSize(window,x,y);
		SDL_RenderSetLogicalSize(renderer, x, y);
		Master.resX = x;
		Master.resY = y;
		TestGraphic.resX = x;
		TestGraphic.resY = y - RESY_UI;
		// printFlush("\n%d %d | %llu",x,y,getBufferBoxSize(&Master));
		//Master.vram = (uint8_t*)realloc((void*)(Master.vram),getBufferBoxSize(&Master));
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

void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
    ImGui::Begin("Horizontal Button Page", NULL, window_flags);

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

	ImGui::Begin("Fractal Menu");

	static int Combo_FractalType = 0;
	ImGui::Text("Fractal Type:");
    if (ImGui::Combo("##fractalType", &Combo_FractalType, FractalTypeText, ARRAY_LENGTH(FractalTypeText))) {

    }
	ImGui::Separator();
	if (Combo_FractalType == 0 || Combo_FractalType == 1) { /* ABS and Polar */
		if (Combo_FractalType == 0) {
			ImGui::Text("Fractal Power: %s",getPowerText(input_power));
			ImGui::InputInt("##input_power",&input_power,1,1); valueLimit(input_power,2,10);
		} else {
			ImGui::Text("Fractal Power: %s",getPowerText(round(input_polar_power)));
			ImGui::SliderFloat("##input_polar_power",&temp_input_polar_power,1.0,10.0,"%.4f"); input_polar_power = (fp64)temp_input_polar_power;
			ImGui::Checkbox("Lock position to Cardioid",&lockToCardioid);
			if (lockToCardioid) {
				ImGui::Checkbox("Flip Cardioid position",&flipCardioidSide);
			}
			ImGui::Text("Cardioid Location: %lf",getABSFractalMinRadius(input_polar_power));
		}
		ImGui::Checkbox("Adjust zoom value to power",&adjustZoomToPower);
		if (Combo_FractalType == 0) {
			ImGui::Text("Cardioid Location: %.5lf",getABSFractalMaxRadius((uint32_t)input_power));
		} else {
			ImGui::Text("Cardioid Location: %.5lf",getABSFractalMaxRadius(input_polar_power));
		}
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
		if (ImGui::Combo("##juliaScreen", &Combo_JuliaSplit, WindowDivider, ARRAY_LENGTH(WindowDivider))) {
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
		if (ImGui::Combo("##juliaBehaviour", &Combo_JuliaBehaviour, juliaBehaviour, ARRAY_LENGTH(juliaBehaviour))) {

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

int render_IMGUI() {

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({(fp32)Master.resX,(fp32)RESY_UI});
	
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	horizontal_buttons_IMGUI(window_flags);
	
	if (buttonSelection != -1) {
		switch(buttonSelection) {
			case 0:
			Menu_Fractal();
			break;
			case 1:
			//Export_FracExp();
			break;
			//Import_FracExp();
			case 2:
			break;
			//Take_Screenshot();
			case 3:
			break;
			//Menu_Rendering();
			case 4:
			break;
			//Menu_Settings();
			case 5:
			break;
			//Abort_Rendering();
		}
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
	return 0;
}

int start_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
	TimerBox frameTimer = TimerBox(1.0/FRAME_RATE);
	TimerBox quitTimer = TimerBox(6.1);
	while (QUIT_FLAG == false) {
		SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                QUIT_FLAG = true;
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
			windowResizingCode();
			newFrame();
		}
		
		if (quitTimer.timerReset()) {
			//QUIT_FLAG = true;
		}
	}
	return 0;
}

int init_Render(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex) {
	initBufferBox(&Master,NULL,800,600,3);
	initBufferBox(&TestGraphic,NULL,Master.resX,Master.resY - RESY_UI,3);
	SDL_Init(SDL_INIT_VIDEO);
	printf("\nSystem Information:");
	printf("\n\tDisplay Count: %d",SDL_GetNumVideoDisplays());
	printf("\n\tOperating System: %s",SDL_GetPlatform());
	printf("\n\tSystem RAM: %dMB",SDL_GetSystemRAM());
	// Allocate Buffers
	TestGraphic.vram = (uint8_t*)malloc(getBufferBoxSize(&TestGraphic));
	window = SDL_CreateWindow("Easy_GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Master.resX, Master.resY, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
	
	printf("\nInit_Render: %s", ((QUIT_FLAG == true) ? "True" : "False"));
	start_Render(QUIT_FLAG,Console_Mutex);
	return 0;
}

int terminate_Render() {
	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
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
			TestGraphic.vram[z] = (x - w) % 256; TestGraphic.vram[z] /= 3; z++;
			TestGraphic.vram[z] = (w - y) % 256; TestGraphic.vram[z] /= 3; z++;
			TestGraphic.vram[z] = (w + x + y) % 256; TestGraphic.vram[z] /= 3; z++;
		}
	}

}

void newFrame() {

	SDL_SetRenderDrawColor(renderer, 6, 24, 96, 255);
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