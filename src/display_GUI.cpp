/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "display_GUI.h"
#include "temp_global_render.h"

#include <SDL2/SDL.h>
// #include <opencv2/opencv.hpp>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "render.h"

// #include "copyBuffer.h"
// #include "fractal.h"
// #include "keybind.h"
// #include "engine.h"
// #include "fracExpKB.h"
// #include "fileManager.h"
// #include "imageBuffer.h"
// #include "imageTransform.h"

// #include "programData.h"
// #include "user_data.h"

// /* Sets defualt window size and position along with size constraints */
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

void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
    ImGui::Begin("Horizontal Button Page", NULL, window_flags);

	static ImVec4 GUI_FrameRateColor;
	static ImVec4 Render_FrameRateColor;
	fp64 Frame_FPS_Display = 1.0 / Frame_Time_Display;
	fp64 Render_FPS_Display = 1.0 / Render_Time_Display;
	GUI_FrameRateColor = {
		(fp32)linearInterpolationClamp(Frame_FPS_Display,59.0,119.0,1.0,0.0),
		(fp32)linearInterpolationClamp(Frame_FPS_Display,0.0,29.0,0.0,1.0),
		(fp32)linearInterpolationClamp(Frame_FPS_Display,29.0,59.0,0.0,1.0),
		1.0
	};
	Render_FrameRateColor = {
		(fp32)linearInterpolationClamp(Render_FPS_Display,59.0,119.0,1.0,0.0),
		(fp32)linearInterpolationClamp(Render_FPS_Display,0.0,29.0,0.0,1.0),
		(fp32)linearInterpolationClamp(Render_FPS_Display,29.0,59.0,0.0,1.0),
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
	int input_subSample = primaryRenderData.subSample;
	int input_superSample = primaryRenderData.sample;
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
	if (ImGui::SliderInt("##input_subSample",&input_subSample,1,24,"")) {
		primaryRenderData.subSample = input_subSample;
	}
	ImGui::Text("Samples per pixel: %d",input_superSample * input_superSample);
	if (ImGui::SliderInt("##input_superSample",&input_superSample,1,24,"")) {
		primaryRenderData.sample = input_superSample;
	}
	uint32_t totalResX = primaryRenderData.resX * primaryRenderData.sample / primaryRenderData.subSample;
	uint32_t totalResY = primaryRenderData.resY * primaryRenderData.sample / primaryRenderData.subSample;
	ImGui::Text("Total Pixels Rendered: %ux%u %.3lfMP",totalResX,totalResY,(fp64)(totalResX * totalResY) / 1000000.0);
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

// int updateFractalParameters() {
// 	using namespace Key_Function;
// 	using namespace Change_Level;
// 	#define FRAC frac.type.abs_mandelbrot
// 	fp64 temp_breakoutValue = log2(FRAC.breakoutValue);
// 	fp64 moveDelta = (DeltaTime < 0.2) ? DeltaTime : 0.2;
	
// 	moveDelta *= user_sensitivity.global;

// 	int update_level = Change_Level::Nothing;

// 	/* Magic Constants */
// 		#define ABS_Mandelbrot_Default_Power 2
// 		#define Polar_Mandelbrot_Default_Power 3.0
// 	/* Boolean toggles */
// 		#define paramToggle(func,toggle,freq) if (funcTimeDelay(func,freq)) { toggle = !toggle; }
// 		#define paramToggleUpdate(func,toggle,freq,level) if (funcTimeDelay(func,freq)) { toggle = !toggle; Update_Level(level); }
// 		paramToggle(toggleAdjustZoomToPower,FRAC.adjustZoomToPower,0.4);
// 		paramToggleUpdate(toggleJulia,FRAC.juliaSet,0.4,Major_Reset);
// 		paramToggleUpdate(toggleABSandPolarMandelbrot,FRAC.polarMandelbrot,0.4,Major_Reset);
// 		paramToggle(toggleRelativeZValue,FRAC.relativeZValue,0.4);
// 		paramToggle(toggleCursorZValue,FRAC.cursorZValue,0.4);
// 		paramToggleUpdate(toggleStartingZ,FRAC.startingZ,0.4,Minor_Reset);
// 		paramToggleUpdate(toggleIntegerPower,FRAC.integerPolarPower,0.4,Minor_Reset);
// 		#undef paramToggle
// 	/* Real and Imaginary Coordinates */
// 		if (func_stat[incRealPos].triggered == true) {
// 			moveCord(
// 				&FRAC.r, &FRAC.i, 0.0 * TAU + FRAC.rot,
// 				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.coordinate
// 			);
// 		}
// 		if (func_stat[decRealPos].triggered == true) {
// 			moveCord(
// 				&FRAC.r, &FRAC.i, 0.5 * TAU + FRAC.rot,
// 				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.coordinate
// 			);
// 		}
// 		if (func_stat[incImagPos].triggered == true) {
// 			moveCord(
// 				&FRAC.r, &FRAC.i, 0.25 * TAU + FRAC.rot,
// 				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.coordinate
// 			);
// 		}
// 		if (func_stat[decImagPos].triggered == true) {
// 			moveCord(
// 				&FRAC.r, &FRAC.i, 0.75 * TAU + FRAC.rot,
// 				0.72 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.coordinate
// 			);
// 		}
// 		if (funcTimeDelay(resetRealPos,0.2)) {
// 			FRAC.r = 0.0;
// 			Update_Level(Jump);
// 		}
// 		if (funcTimeDelay(resetImagPos,0.2)) {
// 			FRAC.i = 0.0;
// 			Update_Level(Jump);
// 		}
// 	/* Real and Imaginary Julia Z Coordinates */
// 		if (func_stat[incZReal].triggered == true) {
// 			moveCord(
// 				&FRAC.zr, &FRAC.zi, 0.0 * TAU + FRAC.rot,
// 				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.julia
// 			);
// 		}
// 		if (func_stat[decZReal].triggered == true) {
// 			moveCord(
// 				&FRAC.zr, &FRAC.zi, 0.5 * TAU + FRAC.rot,
// 				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sX * user_sensitivity.julia
// 			);
// 		}
// 		if (func_stat[incZImag].triggered == true) {
// 			moveCord(
// 				&FRAC.zr, &FRAC.zi, 0.25 * TAU + FRAC.rot,
// 				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.julia
// 			);
// 		}
// 		if (func_stat[decZImag].triggered == true) {
// 			moveCord(
// 				&FRAC.zr, &FRAC.zi, 0.75 * TAU + FRAC.rot,
// 				0.24 * pow(10.0,-FRAC.zoom) * moveDelta * FRAC.sY * user_sensitivity.julia
// 			);
// 		}
// 		if (funcTimeDelay(resetZReal,0.2)) {
// 			FRAC.zr = 0.0;
// 			Update_Level(Jump);
// 		}
// 		if (funcTimeDelay(resetZImag,0.2)) {
// 			FRAC.zi = 0.0;
// 			Update_Level(Jump);
// 		}
// 		if (FRAC.cursorZValue == true) {
// 			if (FRAC.relativeZValue == true) {
// 				fp64 resZ = (fp64)((Master.resX > Master.resY) ? Master.resY : Master.resX);
// 				FRAC.zr = 4.0 * ((fp64)ImGui::GetMousePos().x - ((fp64)Master.resX / 2.0)) / resZ;
// 				FRAC.zi = 4.0 * ((fp64)(ImGui::GetMousePos().y - RESY_UI) - ((fp64)Master.resY / 2.0)) / resZ;
// 			} else {
// 				pixel_to_coordinate(ImGui::GetMousePos().x,ImGui::GetMousePos().y - RESY_UI,&FRAC.zr,&FRAC.zi,&FRAC,&primaryRenderData);
// 			}
// 		}
// 	/* Zoom */
// 		if (func_stat[incZoom].triggered == true) {
// 			FRAC.zoom += 0.25 * moveDelta * user_sensitivity.zoom * (user_sensitivity.invert_zoom ? -1.0 : 1.0);
// 		}
// 		if (func_stat[decZoom].triggered == true) {
// 			FRAC.zoom -= 0.25 * moveDelta * user_sensitivity.zoom * (user_sensitivity.invert_zoom ? -1.0 : 1.0);
// 		}
// 		if (funcTimeDelay(resetZoom,0.2)) {
// 			FRAC.zoom = zoomDefault(FRAC.power);
// 			Update_Level(Jump);
// 		}
// 		if (funcTimeDelay(resetCoordinates,0.2)) {
// 			FRAC.r = 0.0; FRAC.i = 0.0;
// 			FRAC.stretch = 0.0; FRAC.rot = 0.0;
// 			if (FRAC.polarMandelbrot == true) {
// 				FRAC.zoom = zoomDefault(FRAC.polarPower);
// 			} else {
// 				FRAC.zoom = zoomDefault((fp64)FRAC.power);
// 			}
// 			valueLimit(FRAC.zoom,-0.4,0.4);
// 			Update_Level(Jump);
// 		}
// 	/* maxItr */
// 		if (func_stat[incMaxItr].triggered) {
// 			FRAC.maxItr_Log2 += 2.0 * moveDelta * user_sensitivity.maxIter;
// 			setMaxItr(&FRAC,FRAC.maxItr_Log2);
// 		}
// 		if (func_stat[decMaxItr].triggered) {
// 			FRAC.maxItr_Log2 -= 2.0 * moveDelta * user_sensitivity.maxIter;
// 			setMaxItr(&FRAC,FRAC.maxItr_Log2);
// 		}
// 		if (funcTimeDelay(resetMaxItr,0.2)) {
// 			FRAC.maxItr_Log2 = log2(192.0);
// 			FRAC.maxItr = 192;
// 			setMaxItr(&FRAC,FRAC.maxItr_Log2);
// 		}
// 	/* Formula*/
// 		if (funcTimeDelay(incFormula,1.0/10.0)) {
// 			FRAC.formula++;
// 		}
// 		if (funcTimeDelay(decFormula,1.0/10.0)) {
// 			FRAC.formula--;
// 		}
// 		if (funcTimeDelay(incFamily,1.0/10.0)) {
// 			FRAC.formula += getABSValue(FRAC.power);
// 		}
// 		if (funcTimeDelay(decFamily,1.0/10.0)) {
// 			FRAC.formula -= getABSValue(FRAC.power);
// 		}
// 		if (funcTimeDelay(resetFormula,0.2)) {
// 			FRAC.formula = 0;
// 		}
// 	/* Power */
// 		if (FRAC.polarMandelbrot == true) {
// 			if (FRAC.integerPolarPower == true) {
// 				if (funcTimeDelay(incPower,1.0/6.0)) {
// 					FRAC.polarPower++;
// 				}
// 				if (funcTimeDelay(decPower,1.0/6.0)) {
// 					FRAC.polarPower--;
// 				}
// 			} else {
// 				if (func_stat[incPower].triggered) {
// 					FRAC.polarPower += moveDelta * 1.0 * user_sensitivity.polar_power;
// 				}
// 				if (func_stat[decPower].triggered) {
// 					FRAC.polarPower -= moveDelta * 1.0 * user_sensitivity.polar_power;
// 				}
// 			}
			
// 			if (funcTimeDelay(resetPower,0.2)) {
// 				FRAC.polarPower = Polar_Mandelbrot_Default_Power;
// 			}
// 			if (funcTimeDelay(roundPower,0.2)) {
// 				FRAC.polarPower = round(FRAC.polarPower);
// 			}
// 			if (funcTimeDelay(floorPower,0.2)) {
// 				FRAC.polarPower = floor(FRAC.polarPower);
// 			}
// 			if (funcTimeDelay(ceilingPower,0.2)) {
// 				FRAC.polarPower = ceil(FRAC.polarPower);
// 			}
// 		} else {
// 			if (funcTimeDelay(incPower,1.0/6.0)) {
// 				FRAC.power++;
// 			}
// 			if (funcTimeDelay(decPower,1.0/6.0)) {
// 				FRAC.power--;
// 			}
// 			if (funcTimeDelay(resetPower,0.2)) {
// 				FRAC.power = ABS_Mandelbrot_Default_Power;
// 			}
// 			FRAC.formula = limitFormulaID(FRAC.power,FRAC.formula);
// 		}
// 	/* Rotations */
// 		if (func_stat[counterclockwiseRot].triggered) {
// 			FRAC.rot -= (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch) * user_sensitivity.rotation;
// 		}
// 		if (func_stat[clockwiseRot].triggered) {
// 			FRAC.rot += (TAU/3.0) * moveDelta * stretchValue(FRAC.stretch) * user_sensitivity.rotation;
// 		}
// 		if (funcTimeDelay(clockwiseRot90,0.3)) {
// 			FRAC.rot += (TAU * (90.0/360.0));
// 		}
// 		if (funcTimeDelay(counterclockwiseRot90,0.3)) {
// 			FRAC.rot -= (TAU * (90.0/360.0));
// 		}
// 		if (funcTimeDelay(rotate180,0.3)) {
// 			FRAC.rot += (TAU * (180.0/360.0));
// 		}
// 		if (funcTimeDelay(clockwiseRotStep,1.0/10.0)) {
// 			FRAC.rot += (TAU * (15.0/360.0));
// 		}
// 		if (funcTimeDelay(counterclockwiseRotStep,1.0/10.0)) {
// 			FRAC.rot += (TAU * (15.0/360.0));
// 		}
// 		if (funcTimeDelay(clockwiseRotPower,1.0/6.0)) {
// 			FRAC.rot += (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
// 		}
// 		if (funcTimeDelay(counterclockwiseRotPower,1.0/6.0)) {
// 			FRAC.rot -= (TAU * (1.0/(fp64)((FRAC.power - 1) * 2)));
// 		}
// 		if (funcTimeDelay(resetRotation,0.2)) {
// 			FRAC.rot = 0.0;
// 		}
// 		FRAC.rot = (FRAC.rot >= 0.0) ? fmod(FRAC.rot,TAU) : fmod(FRAC.rot + TAU,TAU);
// 	/* Transformations */
// 		if (func_stat[incStretch].triggered) {
// 			FRAC.stretch += 1.0 * moveDelta * user_sensitivity.stretch;
// 		}
// 		if (func_stat[decStretch].triggered) {
// 			FRAC.stretch -= 1.0 * moveDelta * user_sensitivity.stretch;
// 		}
// 		if (funcTimeDelay(resetStretch,0.2)) {
// 			FRAC.stretch = 0.0;
// 		}
// 		if (funcTimeDelay(resetTransformations,0.2)) {
// 			FRAC.rot = 0.0;
// 			FRAC.stretch = 0.0;
// 		}
// 	/* Breakout Value */
// 		if (func_stat[incBreakout].triggered) {
// 			temp_breakoutValue += 2.0 * moveDelta * user_sensitivity.breakout_value;
// 		}
// 		if (func_stat[decBreakout].triggered) {
// 			temp_breakoutValue -= 2.0 * moveDelta * user_sensitivity.breakout_value;
// 		}
// 		if (funcTimeDelay(resetBreakout,0.2)) {
// 			temp_breakoutValue = log2(16777216.0);
// 		}
// 	/* Rendering */
// 		if (funcTimeDelay(incSubSample,1.0/6.0)) {
// 			primaryRenderData.subSample++;
// 		}
// 		if (funcTimeDelay(decSubSample,1.0/6.0)) {
// 			primaryRenderData.subSample--;
// 		}
// 		if (funcTimeDelay(resetSubSample,0.2)) {
// 			primaryRenderData.subSample = 1;
// 		}
// 		valueLimit(primaryRenderData.subSample,1,24);
// 		if (funcTimeDelay(incSuperSample,1.0/6.0)) {
// 			primaryRenderData.sample++;
// 		}
// 		if (funcTimeDelay(decSuperSample,1.0/6.0)) {
// 			primaryRenderData.sample--;
// 		}
// 		if (funcTimeDelay(resetSuperSample,0.2)) {
// 			primaryRenderData.sample = 1;
// 		}
// 		valueLimit(primaryRenderData.sample,1,24);
// 	/* Rendering Method */
// 	{
// 		using namespace Rendering_Method;
// 		if (funcTimeDelay(fp32CpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = CPU_Rendering;
// 			primaryRenderData.CPU_Precision = 32;
// 		}
// 		if (funcTimeDelay(fp64CpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = CPU_Rendering;
// 			primaryRenderData.CPU_Precision = 64;
// 		}
// 		if (funcTimeDelay(fp80CpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = CPU_Rendering;
// 			primaryRenderData.CPU_Precision = 80;
// 		}
// 		if (funcTimeDelay(fp128CpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = CPU_Rendering;
// 			primaryRenderData.CPU_Precision = 128;
// 		}
// 		if (funcTimeDelay(fp16GpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = GPU_Rendering;
// 			primaryRenderData.GPU_Precision = 16;
// 		}
// 		if (funcTimeDelay(fp32GpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = GPU_Rendering;
// 			primaryRenderData.GPU_Precision = 32;
// 		}
// 		if (funcTimeDelay(fp64GpuRendering,0.2)) {
// 			primaryRenderData.rendering_method = GPU_Rendering;
// 			primaryRenderData.GPU_Precision = 64;
// 		}
// 	}
// 	/* Other */
// 	FRAC.breakoutValue = pow(2.0,temp_breakoutValue);
// 	correctFracParameters(&FRAC);
// 	update_level = get_ABS_Mandelbrot_Update_Level(&FRAC,&primaryRenderData,update_level);
// 	/* ABS Mandelbrot */
// 	/* Polar Mandelbrot */
// 	/* Global Application Functions */
// 		#define GUI_MENU_TOGGLE(m) buttonSelection = (buttonSelection == (m)) ? -1 : (m);
// 		if (funcTimeDelay(inputFormula,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_Coordinates);
// 		}
// 		if (funcTimeDelay(inputPower,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
// 		}
// 		if (funcTimeDelay(openFractalMenu,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
// 		}
// 		if (funcTimeDelay(openKeyBindsMenu,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_KeyBinds);
// 		}
// 		if (funcTimeDelay(openRenderingMenu,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_Rendering);
// 		}
// 		if (funcTimeDelay(openSettingsMenu,0.4)) {
// 			GUI_MENU_TOGGLE(GUI_Menu_Settings);
// 		}
// 		if (funcTimeDelay(takeScreenshot,0.4)) {
// 			exportScreenshot();
// 		}
// 		if (funcTimeDelay(takeSuperScreenshot,0.4)) {
// 			exportSuperScreenshot();
// 		}
// 	#undef FRAC
// 	write_Update_Level(update_level);
// 	return update_level;
// }