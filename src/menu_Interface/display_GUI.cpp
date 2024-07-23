/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "display_GUI_Internal.h"

#include "display_GUI.h"
#include "../temp_global_render.h"

#include <SDL2/SDL.h>
// #include <opencv2/opencv.hpp>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "../user_data.h"
#include "../displayInfo.h"

#include "../programData.h"
#include "../render.h"
#include "../fractal_Information/Mandelbrot_Information.h"

#include "../fileManager.h"

#include "../render_Configuration.hpp"
#include "../keybind.h"

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

ImGuiWindowFlags ImGui_WINDOW_FLAGS = 0;

// /* Sets defualt window size and position along with size constraints */
void ImGui_DefaultWindowSize(
	const User_GUI_Settings& GUI_Settings,
	int32_t valX, int32_t bufX, int32_t minX, int32_t maxX,
	int32_t valY, int32_t bufY, int32_t minY, int32_t maxY
) {
	fp64 ratioX = GUI_Settings.WindowAutoScale;
	fp64 ratioY = GUI_Settings.WindowAutoScale;
	int32_t WINDOW_RESX = calcMinMaxRatio(valX - bufX,minX,maxX,ratioX);
	int32_t WINDOW_RESY = calcMinMaxRatio(valY - bufY,minY,maxY,ratioY);
	ImGui::SetNextWindowPos({(fp32)((valX - WINDOW_RESX) / 2),(fp32)((valY - WINDOW_RESY) / 2)}, ImGuiCond_Once);
	ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY}, ImGuiCond_Once);
	if (GUI_Settings.AutoResizeWindows == true) {
		ImGui::SetNextWindowSize({(fp32)WINDOW_RESX,(fp32)WINDOW_RESY});
	}
	ImGui::SetNextWindowSizeConstraints({(fp32)minX,(fp32)minY},{(fp32)(valX - bufX),(fp32)(valY - bufY)});
	WINDOW_RESX = (WINDOW_RESX > valX - bufX) ? (valX - bufX) : WINDOW_RESX;
	WINDOW_RESY = (WINDOW_RESY > valY - bufY) ? (valY - bufY) : WINDOW_RESY;
	ImGui::SetNextWindowBgAlpha(GUI_Settings.WindowOpacity);
}

void ImGui_BoundWindowPosition(const User_GUI_Settings& GUI_Settings) {
	if (GUI_Settings.PreventOutOfBoundsWindows == true) {
		int32_t WINDOW_POSX = (int32_t)ImGui::GetWindowPos().x;
		int32_t WINDOW_POSY = (int32_t)ImGui::GetWindowPos().y;
		valueClamp(WINDOW_POSX, ImGui_WINDOW_MARGIN, Master.resX - (dim32_t)ImGui::GetWindowSize().x - ImGui_WINDOW_MARGIN);
		valueClamp(WINDOW_POSY, ImGui_WINDOW_MARGIN, Master.resY - (dim32_t)ImGui::GetWindowSize().y - ImGui_WINDOW_MARGIN);
		ImGui::SetWindowPos({(fp32)(WINDOW_POSX),(fp32)(WINDOW_POSY)});
	}
}

void Item_Tooltip(const char* fmt, ...) {
    if (ImGui::IsItemHovered()) {
        va_list args;
        va_start(args, fmt);
        ImGui::SetTooltipV(fmt, args);
        va_end(args);
    }
}

void set_IMGUI_Theme(Display_GUI::IMGUI_Theme theme) {
	switch(theme) {
		case Display_GUI::IMGUI_Theme_Classic:
			ImGui::StyleColorsClassic();
		break;
		case Display_GUI::IMGUI_Theme_Dark:
			ImGui::StyleColorsDark();
		break;
		case Display_GUI::IMGUI_Theme_Light:
			ImGui::StyleColorsLight();
		break;
		default:
			ImGui::StyleColorsDark();
	};
}

void refresh_IMGUI(const User_Configuration_Data& config) {
	set_IMGUI_Theme((Display_GUI::IMGUI_Theme)config.GUI_Settings.GUI_Theme);
}

ImVec4 get_Theme_Highlight_Color() {
	bool useLightThemeColors = (config_data.GUI_Settings.GUI_Theme == Display_GUI::IMGUI_Theme_Light) ? true : false;
	return useLightThemeColors ? ImVec4{1.0,0.0,0.0,1.0} : ImVec4{0.0,1.0,1.0,1.0};
}

int render_IMGUI() {
	if (window == nullptr) {
		printError("render_IMGUI(): window == nullptr");
	}
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({(fp32)Master.resX,(fp32)RESY_UI});
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	horizontal_buttons_IMGUI(window_flags);
	
	ImGui_WINDOW_FLAGS = 0;
	ImGui_WINDOW_FLAGS |= ImGuiWindowFlags_NoCollapse; 
	ImGui_WINDOW_FLAGS |= (config_data.GUI_Settings.AutoResizeWindows == true) ? ImGuiWindowFlags_NoResize : 0;

	if (buttonSelection != -1) {
		Lock_Key_Inputs = (config_data.GUI_Settings.LockKeyInputsInMenus == true) ? true : false;
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
			case GUI_Menu_Status:
				Menu_Status();
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
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
	
	//SDL_RenderPresent(renderer); // Redundant
	return 0;
}

void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags) {
    ImGui::Begin("Horizontal Button Page", NULL, window_flags);

	const User_GUI_Settings& GUI_Settings = config_data.GUI_Settings;

	static ImVec4 GUI_FrameRateColor;
	static ImVec4 Render_FrameRateColor;
	fp64 Frame_FPS_Display = NANO_TO_FRAMERATE(Frame_Time_Display);
	fp64 Render_FPS_Display = NANO_TO_FRAMERATE(Render_Time_Display);
	
	GUI_FrameRateColor = {
		(fp32)linearInterpolationClamp(Frame_FPS_Display, 59.0, 119.0, 1.0, 0.0),
		(fp32)linearInterpolationClamp(Frame_FPS_Display,  0.0,  29.0, 0.0, 1.0),
		(fp32)linearInterpolationClamp(Frame_FPS_Display, 29.0,  59.0, 0.0, 1.0),
		1.0
	};
	Render_FrameRateColor = {
		(fp32)linearInterpolationClamp(Render_FPS_Display, 59.0, 119.0, 1.0, 0.0),
		(fp32)linearInterpolationClamp(Render_FPS_Display,  0.0,  29.0, 0.0, 1.0),
		(fp32)linearInterpolationClamp(Render_FPS_Display, 29.0,  59.0, 0.0, 1.0),
		1.0
	};
	if (GUI_Settings.GUI_Theme == Display_GUI::IMGUI_Theme_Light) {
		GUI_FrameRateColor.x = 1.0f - GUI_FrameRateColor.x;
		GUI_FrameRateColor.y = 1.0f - GUI_FrameRateColor.y;
		GUI_FrameRateColor.z = 1.0f - GUI_FrameRateColor.z;
		Render_FrameRateColor.x = 1.0f - Render_FrameRateColor.x;
		Render_FrameRateColor.y = 1.0f - Render_FrameRateColor.y;
		Render_FrameRateColor.z = 1.0f - Render_FrameRateColor.z;
	}
	
	ImGui::Text("GUI:"); ImGui::SameLine();
	ImGui::TextColored(GUI_FrameRateColor,"%.2lf", Frame_FPS_Display); ImGui::SameLine(0.0,1.0);
	ImGui::Text("FPS"); ImGui::SameLine();
	ImGui::TextColored(GUI_FrameRateColor,"%.2lf", NANO_TO_SECONDS(Frame_Time_Display) * 1000.0); ImGui::SameLine(0.0,1.0);
	ImGui::Text("ms");
	ImGui::SameLine();
	ImGui::Text("Render:"); ImGui::SameLine();
	ImGui::TextColored(Render_FrameRateColor,"%.2lf", Render_FPS_Display); ImGui::SameLine(0.0,1.0);
	ImGui::Text("FPS"); ImGui::SameLine();
	ImGui::TextColored(Render_FrameRateColor,"%.2lf", NANO_TO_SECONDS(Render_Time_Display) * 1000.0); ImGui::SameLine(0.0,1.0);
	ImGui::Text("ms");

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
	#ifndef BUILD_RELEASE
		if (ImGui::Button("Program-Status")) {
			buttonSelection = (buttonSelection == GUI_Menu_Status) ? -1 : GUI_Menu_Status;
		} ImGui::SameLine();
	#endif
	if (Waiting_To_Abort_Rendering == true) {
		ImGui::Text("Aborting...(%.1lfs)", NANO_TO_SECONDS(getNanoTime() - abortTimer));
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

	const ABS_Mandelbrot& FRAC = current_Fractal;
	size_t renderFP = Render_Config.get_Current_Float_Size();
	const char* const renderMethod = Render_Config.current_Render_Method_GPU() ? "GPU" : "CPU";
	
	static char powerText[64];
	if (FRAC.polarMandelbrot) {
		snprintf(powerText,sizeof(powerText),"%6.4lf",FRAC.polarPower);
	}

	ImGui::Text(
		"Formula: %3" PRIu64 " Power: %s Super-Sample: %" PRIu32 " Rendering: %s Float%zu",
		FRAC.formula,(FRAC.polarMandelbrot ? powerText : getPowerText(FRAC.power)),primaryRenderData.sample * primaryRenderData.sample,renderMethod,renderFP
	);
	constexpr size_t temp_FloatCoordinate_len = 64;
	static char temp_FloatCoordinate_r[temp_FloatCoordinate_len];
	static char temp_FloatCoordinate_i[temp_FloatCoordinate_len];
	static char temp_FloatCoordinate_zr[temp_FloatCoordinate_len];
	static char temp_FloatCoordinate_zi[temp_FloatCoordinate_len];
	FloatCoordinate_snprintf(temp_FloatCoordinate_r , temp_FloatCoordinate_len, "%15.12" PRIfpCord "f", FRAC.r);
	FloatCoordinate_snprintf(temp_FloatCoordinate_i , temp_FloatCoordinate_len, "%15.12" PRIfpCord "f", FRAC.i);
	FloatCoordinate_snprintf(temp_FloatCoordinate_zr, temp_FloatCoordinate_len, "%15.12" PRIfpCord "f", FRAC.zr);
	FloatCoordinate_snprintf(temp_FloatCoordinate_zi, temp_FloatCoordinate_len, "%15.12" PRIfpCord "f", FRAC.zi);

	ImGui::Text(
		"Zreal: %s Zimag: %s Rotation: %5.1" PRIfp64 "f Stetch: 2^%6.4" PRIfp64 "f",
		temp_FloatCoordinate_zr, temp_FloatCoordinate_zi, FRAC.rot * 360.0 / TAU, FRAC.stretch
	);
	ImGui::NewLine();
	fp64 adjustedZoomValue = FRAC.zoom;
	if (FRAC.adjustZoomToPower == true) {
		if (FRAC.polarMandelbrot == true) {
			adjustedZoomValue += log10(getABSFractalMaxRadius(FRAC.polarPower));
		} else {
			adjustedZoomValue += log10(getABSFractalMaxRadius((fp64)FRAC.power));
		}
		valueClamp(adjustedZoomValue,FRAC.zoom - 0.4, FRAC.zoom + 0.4);
	}
	
	ImGui::Text(
		"Real:  %s Imag:  %s Zoom: 10^%6.4" PRIfp64 "f Itr: %" PRIu32,
		temp_FloatCoordinate_r, temp_FloatCoordinate_i, adjustedZoomValue, FRAC.maxItr
	);
    // End the ImGui window
    ImGui::End();
}

void Menu_Coordinates() {
	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 240, 400,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 160, 320
	);
	ImGui::Begin("Coordinates Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);

	ABS_Mandelbrot& FRAC = current_Fractal;
	#define NumberTextLen (64)
	
	#define FloatCoordinate_InputText(lbl, num, fmt); do { \
			static char Temp_Text_Input_Buf[NumberTextLen]; \
			FloatCoordinate_snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
			if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
				num = stringTo_FloatCoordinate(Temp_Text_Input_Buf, nullptr); \
			} \
		} while(0)

	#define Float_InputText(lbl, num, fmt, func); do { \
			static char Temp_Text_Input_Buf[NumberTextLen]; \
			snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
			if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
				num = func(Temp_Text_Input_Buf, nullptr); \
			} \
		} while(0)
	#define Int_InputText(lbl, num, fmt, func, base); do { \
			static char Temp_Text_Input_Buf[NumberTextLen]; \
			snprintf(Temp_Text_Input_Buf, NumberTextLen, fmt, num); \
			if (ImGui::InputText(lbl,Temp_Text_Input_Buf,NumberTextLen)) { \
				num = func(Temp_Text_Input_Buf, nullptr, base); \
			} \
		} while(0)
	ImGui::SeparatorText("Cordinates"); { ImGui::Indent();
		ImGui::Text("Real and Imaginary Coordinate:");
				FloatCoordinate_InputText("C-Real##input_C_Real", FRAC.r, "%35.32" PRIfpCord "f");
				FloatCoordinate_InputText("C-Imag##input_C_Imag", FRAC.i, "%35.32" PRIfpCord "f");
		ImGui::Text("Zoom:");
			Float_InputText("##zoom_input", FRAC.zoom, "%.5lf", strtod);
			
		ImGui::NewLine();
	ImGui::Unindent(); }
	ImGui::SeparatorText("Parameters"); { ImGui::Indent();
		/* Iterations */
			ImGui::Text("Maximum Iterations:");
			uint32_t temp_Iterations = FRAC.maxItr;
			constexpr uint32_t Iterations_Step = 16;
			constexpr uint32_t Iterations_Step_Fast = 256;
			if (ImGui::InputScalar("##input_maxIter", ImGuiDataType_U32, &temp_Iterations, &Iterations_Step, &Iterations_Step_Fast, "%" PRIu32)) {
				setMaxItr(&FRAC, temp_Iterations);
			}
			ImGui::NewLine();

		/* Breakout Value */
			fp32 temp_input_breakoutValue = (fp32)log2(FRAC.breakoutValue);
			if (FRAC.breakoutValue < 100.0) {
				ImGui::Text("Breakout Value: %.3lf", sqrt(FRAC.breakoutValue));
			} else {
				ImGui::Text("Breakout Value: %.1lf", sqrt(FRAC.breakoutValue));
			}
			ImGui::SliderFloat("##input_breakoutValue",&temp_input_breakoutValue,-2.0,32.0,"");
			FRAC.breakoutValue = pow(2.0,(fp64)temp_input_breakoutValue);
			ImGui::NewLine();
	ImGui::Unindent(); }
	ImGui::SeparatorText("Julia Set:"); { ImGui::Indent();
		ImGui::Text("Julia Coordinate:");
		FloatCoordinate_InputText("Z-Real##input_Z_Real", FRAC.zr, "%35.32" PRIfpCord "f");
		FloatCoordinate_InputText("Z-Imag##input_Z_Imag", FRAC.zi, "%35.32" PRIfpCord "f");
		fp32 juliaAngle = (fp32)atan2(FRAC.zi, FRAC.zr);
		if (ImGui::SliderAngle("Julia Angle",&juliaAngle, -360.0f, 360.0f, "%.1f deg")) {
			fpCord juliaMagnitude = hypot(FRAC.zr, FRAC.zi);
			fpCord juliaTheta = (fpCord)juliaAngle;
			FRAC.zr = juliaMagnitude * cos(juliaTheta);
			FRAC.zi = juliaMagnitude * sin(juliaTheta);
		}
		ImGui::NewLine();
		if (ImGui::Button("Julia Set Options")) {
			buttonSelection = GUI_Menu_Fractal;
		}
		ImGui::NewLine();
	ImGui::Unindent(); }

	ImGui::SeparatorText("Transformations"); { ImGui::Indent();
		
		ImGui::Text("Rotate Image:");
			/* Input */
				constexpr fp64 Rotate_Image_Step = 15.0;
				constexpr fp64 Rotate_Image_Step_Fast = 45.0;
				fp64 frac_rot = RADIANS_TO_DEGREES(FRAC.rot);
				if (ImGui::InputScalar("##input_image_rotation", ImGuiDataType_Double, &frac_rot, &Rotate_Image_Step, &Rotate_Image_Step_Fast, "%.5" PRIfp64 "f")) {
					FRAC.rot = clampRotation(DEGREES_TO_RADIANS(frac_rot));
				}
			/* Slider */
				fp32 image_rotation = (fp32)FRAC.rot;
				if (ImGui::SliderAngle("##RotateImage", &image_rotation, -360.0f, 360.0f, "%.1f deg")) {
					FRAC.rot = (fp64)image_rotation;
				}
		if (ImGui::Button("Rotate 90 deg counter-clockwise")) { FRAC.rot -= DEGREES_TO_RADIANS(90.0); }
		ImGui::SameLine();
		if (ImGui::Button("Rotate 90 deg clockwise")) { FRAC.rot += DEGREES_TO_RADIANS(90.0); }
		ImGui::NewLine();

		ImGui::Text("Stretch Image: 2.0^%.5" PRIfp64 "f", FRAC.stretch);
		constexpr fp64 Stretch_Step = 1.0 / 4.0;
		constexpr fp64 Stretch_Step_Fast = 1.0;
		if (ImGui::InputScalar("##input_stretch", ImGuiDataType_Double, &FRAC.stretch, &Stretch_Step, &Stretch_Step_Fast, "%.5" PRIfp64 "f")) {
			valueRestore(FRAC.stretch, 0.0, STRETCH_VALUE_MINIMUM, STRETCH_VALUE_MAXIMUM);
		}
		ImGui::NewLine();
	ImGui::Unindent(); }
	ImGui::End();
}

void Menu_Fractal() {
	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 240, 400,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 160, 320
	);
	// static const char* juliaBehaviour[] = {"Independant Movement", "Copy Movement", "Cordinates follow Z Value", "Z Value follows Coordinates"};

	ImGui::Begin("Fractal Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);

	ABS_Mandelbrot& FRAC = current_Fractal;

	int Combo_FractalType = FRAC.polarMandelbrot ? Fractal_Polar_Mandelbrot : Fractal_ABS_Mandelbrot;
	ImGui::Text("Fractal Type:"); {
		if (ImGui::Combo("##fractalType", &Combo_FractalType, BufAndLen(FractalTypeText))) {
			switch(Combo_FractalType) {
				case Fractal_ABS_Mandelbrot:
					setDefaultParameters(&FRAC, Fractal_ABS_Mandelbrot);
				break;
				case Fractal_Polar_Mandelbrot:
					setDefaultParameters(&FRAC, Fractal_Polar_Mandelbrot);
				break;
				default:
				printError("Unknown Fractal Type: %" PRId32, Combo_FractalType);
			}
		}
		ImGui::NewLine();
	}
	ImGui::SeparatorText("Fractal Formula and Power"); { ImGui::Indent();
		/* Radius */
			fp64 maxRadius = getABSFractalMaxRadius((Combo_FractalType == Fractal_ABS_Mandelbrot) ? (fp64)FRAC.power : FRAC.polarPower);
			fp64 minRadius = getABSFractalMinRadius((Combo_FractalType == Fractal_ABS_Mandelbrot) ? (fp64)FRAC.power : FRAC.polarPower);
			ImGui::Text("Fractal Radius: %.6lf",maxRadius);
			ImGui::Text("Cardioid Location: %.6lf",minRadius);
			ImGui::NewLine();
		/* Power */
			if (FRAC.polarMandelbrot == true) {
				static bool use_Power_Sliders = true;
				constexpr fp64 Polar_Power_Step = 1.0;
				constexpr fp64 Polar_Power_Step_Fast = 0.1;
				ImGui::Text("Power: %s", getPowerText(round(FRAC.polarPower)));
				if (use_Power_Sliders == true) {
					fp32 temp_input_polar_power = (fp32)FRAC.polarPower;
					ImGui::SliderFloat("##input_polar_power",&temp_input_polar_power,(fp32)POLAR_POWER_MINIMUM,(fp32)POLAR_POWER_MAXIMUM,"%.4f");
					FRAC.polarPower = (fp64)temp_input_polar_power;
				} else {
					if (ImGui::InputScalar("##input_polar_power", ImGuiDataType_Double, &FRAC.polarPower, &Polar_Power_Step, &Polar_Power_Step_Fast, "%.5" PRIfp64)) {
						valueClamp(FRAC.polarPower, POLAR_POWER_MINIMUM, POLAR_POWER_MAXIMUM);
					}
				}
				ImGui::Checkbox("Use Sliders##Use_Power_Sliders", &use_Power_Sliders);
			} else {
				constexpr uint32_t Mandelbrot_Power_Step = 1;
				constexpr uint32_t Mandelbrot_Power_Step_Fast = 1;
				ImGui::Text("Power: %s", getPowerText((uint32_t)FRAC.power));
				if (ImGui::InputScalar("##input_power", ImGuiDataType_U32, &FRAC.power, &Mandelbrot_Power_Step, &Mandelbrot_Power_Step_Fast, "%" PRIu32)) {
					valueClamp(FRAC.power, MANDELBROT_POWER_MINIMUM, MANDELBROT_POWER_MAXIMUM);
				}
			}
			ImGui::NewLine();

		/* Formula */
			if (FRAC.polarMandelbrot == false) {
				ImGui::Text("Fractal Formula:");
				static bool inputHexadecimal = false;
				if (inputHexadecimal == true) {
					Int_InputText("##input_formula", FRAC.formula, "%" PRIX64, stringTo_Uint64, 16);
				} else {
					Int_InputText("##input_formula", FRAC.formula, "%" PRIu64, stringTo_Uint64, 10);
				}
				ImGui::Checkbox("Hexadecimal", &inputHexadecimal);
			
				switch (FRAC.power) {
					case Mandelbrot_Quadratic: {
						ImGui::NewLine();
						ImGui::Text("Select a fractal from the \"75 Mandelbrot Variants\" video:");
						int Combo_Quadractic_Fractals = 0;
						if (ImGui::Combo("##Combo_Quadratic_Fractals", &Combo_Quadractic_Fractals, Quadratic_Fractals_Text, ARRAY_LENGTH(Quadratic_Fractals_Text))) {
							if (Combo_Quadractic_Fractals != 0) {
								FRAC.formula = Quadratic_Fractals_Formula[Combo_Quadractic_Fractals];
							}
						}
					} break;
					case Mandelbrot_Cubic: {
						ImGui::NewLine();
						ImGui::Text("Select a fractal from the \"330 Cubic Fractals\" video:");
						int Combo_Cubic_Fractals = 0;
						if (ImGui::Combo("##Combo_Cubic_Fractals", &Combo_Cubic_Fractals, Cubic_Fractals_Text, ARRAY_LENGTH(Cubic_Fractals_Text))) {
							if (Combo_Cubic_Fractals != 0) {
								FRAC.formula = Cubic_Fractals_Formula[Combo_Cubic_Fractals];
							}
						}
					} break;
					case Mandelbrot_Quartic: {
						ImGui::NewLine();
						ImGui::Text("Select one of the \"5265 Quartic Fractals\":");
						int Combo_Quartic_Fractals = 0;
						if (ImGui::Combo("##Combo_Cubic_Fractals", &Combo_Quartic_Fractals, Quartic_Fractals_Text, ARRAY_LENGTH(Quartic_Fractals_Text))) {
							if (Combo_Quartic_Fractals != 0) {
								FRAC.formula = Quartic_Fractals_Formula[Combo_Quartic_Fractals];
							}
						}
					} break;
				}
				ImGui::NewLine();
			}
	ImGui::Unindent(); }

	ImGui::SeparatorText("Julia Set"); { ImGui::Indent();
		ImGui::Text("Julia Set Options:");
		ImGui::Checkbox("Render Julia Set",&FRAC.juliaSet);
		ImGui::Checkbox("Toggle starting Z values",&FRAC.startingZ);
		Item_Tooltip("Toggles initial Z values when rendering the Mandelbrot Set");

		ImGui::Checkbox("Use Cursor for Z values",&FRAC.cursorZValue);
		if (FRAC.cursorZValue) {
			ImGui::Checkbox("Use relative Z values",&FRAC.relativeZValue);
			Item_Tooltip("Ignores the zoom value when calculating cursor Z values");
		}
		ImGui::NewLine();
	ImGui::Unindent(); }

	ImGui::SeparatorText("Fractal Options"); { ImGui::Indent();
		if (Combo_FractalType == Fractal_Polar_Mandelbrot) {
			ImGui::Checkbox("Lock position to Cardioid",&FRAC.lockToCardioid);
			if (FRAC.lockToCardioid) {
				ImGui::Checkbox("Flip Cardioid position",&FRAC.flipCardioidSide);
			}
			ImGui::Checkbox("Integer Powers",&FRAC.integerPolarPower);
		}
		ImGui::Checkbox("Adjust zoom value to power",&FRAC.adjustZoomToPower);
		
		ImGui::NewLine();
	ImGui::Unindent(); }
	
	#ifndef BUILD_RELEASE
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
		ImGui::NewLine();
	#endif
	ImGui::SeparatorText("Coloring"); { ImGui::Indent(); 
		struct Temp_Color {
			fp32 exterior_Alpha;
				fp32 exterior_R_Amp; fp32 exterior_R_Freq; fp32 exterior_R_Phase;
				fp32 exterior_G_Amp; fp32 exterior_G_Freq; fp32 exterior_G_Phase;
				fp32 exterior_B_Amp; fp32 exterior_B_Freq; fp32 exterior_B_Phase;
			fp32 interior_Alpha;
				fp32 interior_R_Amp; fp32 interior_R_Freq; fp32 interior_R_Phase;
				fp32 interior_G_Amp; fp32 interior_G_Freq; fp32 interior_G_Phase;
				fp32 interior_B_Amp; fp32 interior_B_Freq; fp32 interior_B_Phase;
		};
		Temp_Color temp_Color;
		temp_Color.exterior_Alpha = (fp32)FRAC.exterior_Alpha;
			temp_Color.exterior_R_Amp = (fp32)FRAC.exterior_R_Amp; temp_Color.exterior_R_Freq = (fp32)FRAC.exterior_R_Freq; temp_Color.exterior_R_Phase = (fp32)FRAC.exterior_R_Phase;
			temp_Color.exterior_G_Amp = (fp32)FRAC.exterior_G_Amp; temp_Color.exterior_G_Freq = (fp32)FRAC.exterior_G_Freq; temp_Color.exterior_G_Phase = (fp32)FRAC.exterior_G_Phase;
			temp_Color.exterior_B_Amp = (fp32)FRAC.exterior_B_Amp; temp_Color.exterior_B_Freq = (fp32)FRAC.exterior_B_Freq; temp_Color.exterior_B_Phase = (fp32)FRAC.exterior_B_Phase;
		temp_Color.interior_Alpha = (fp32)FRAC.interior_Alpha;
			temp_Color.interior_R_Amp = (fp32)FRAC.interior_R_Amp; temp_Color.interior_R_Freq = (fp32)FRAC.interior_R_Freq; temp_Color.interior_R_Phase = (fp32)FRAC.interior_R_Phase;
			temp_Color.interior_G_Amp = (fp32)FRAC.interior_G_Amp; temp_Color.interior_G_Freq = (fp32)FRAC.interior_G_Freq; temp_Color.interior_G_Phase = (fp32)FRAC.interior_G_Phase;
			temp_Color.interior_B_Amp = (fp32)FRAC.interior_B_Amp; temp_Color.interior_B_Freq = (fp32)FRAC.interior_B_Freq; temp_Color.interior_B_Phase = (fp32)FRAC.interior_B_Phase;
		
		ImGui::Text("EXTERIOR COLORING:");
			constexpr fp32 Maximum_Exterior_Freq = 2.0f;
			ImGui::Text("Amplitude:");
				fp32 exterior_Amp[4] = {temp_Color.exterior_R_Amp, temp_Color.exterior_G_Amp, temp_Color.exterior_B_Amp, temp_Color.exterior_Alpha};
				ImGui::ColorEdit4("##exterior_Amp", exterior_Amp);
					temp_Color.exterior_R_Amp = exterior_Amp[0];
					temp_Color.exterior_G_Amp = exterior_Amp[1];
					temp_Color.exterior_B_Amp = exterior_Amp[2];
					temp_Color.exterior_Alpha = exterior_Amp[3];
			ImGui::Text("Frequency:");
				fp32 exterior_Freq[3] = {temp_Color.exterior_R_Freq, temp_Color.exterior_G_Freq, temp_Color.exterior_B_Freq};
				ImGui::SliderFloat3("##exterior_Freq", exterior_Freq, 0.0f, Maximum_Exterior_Freq, "%.4f");
					temp_Color.exterior_R_Freq = exterior_Freq[0];
					temp_Color.exterior_G_Freq = exterior_Freq[1];
					temp_Color.exterior_B_Freq = exterior_Freq[2];
			ImGui::Text("Phase:");
				fp32 exterior_Phase[3] = {temp_Color.exterior_R_Phase, temp_Color.exterior_G_Phase, temp_Color.exterior_B_Phase};
				ImGui::SliderFloat3("##exterior_Phase", exterior_Phase, 0.0f, 1.0f, "%.4f");
					temp_Color.exterior_R_Phase = exterior_Phase[0];
					temp_Color.exterior_G_Phase = exterior_Phase[1];
					temp_Color.exterior_B_Phase = exterior_Phase[2];
			ImGui::NewLine();
		
		ImGui::Text("INTERIOR COLORING:");
			constexpr fp32 Maximum_Interior_Freq = 3.0f;
			ImGui::Text("Amplitude:");
				fp32 interior_Amp[4] = {temp_Color.interior_R_Amp, temp_Color.interior_G_Amp, temp_Color.interior_B_Amp, temp_Color.interior_Alpha};
				ImGui::ColorEdit4("##interior_Amp", interior_Amp);
					temp_Color.interior_R_Amp = interior_Amp[0];
					temp_Color.interior_G_Amp = interior_Amp[1];
					temp_Color.interior_B_Amp = interior_Amp[2];
					temp_Color.interior_Alpha = interior_Amp[3];
			ImGui::Text("Frequency:");
				fp32 interior_Freq[3] = {temp_Color.interior_R_Freq, temp_Color.interior_G_Freq, temp_Color.interior_B_Freq};
				ImGui::SliderFloat3("##interior_Freq", interior_Freq, 0.0f, Maximum_Interior_Freq, "%.4f");
					temp_Color.interior_R_Freq = interior_Freq[0];
					temp_Color.interior_G_Freq = interior_Freq[1];
					temp_Color.interior_B_Freq = interior_Freq[2];
			ImGui::Text("Phase:");
				fp32 interior_Phase[3] = {temp_Color.interior_R_Phase, temp_Color.interior_G_Phase, temp_Color.interior_B_Phase};
				ImGui::SliderFloat3("##interior_Phase", interior_Phase, 0.0f, 1.0f, "%.4f");
					temp_Color.interior_R_Phase = interior_Phase[0];
					temp_Color.interior_G_Phase = interior_Phase[1];
					temp_Color.interior_B_Phase = interior_Phase[2];
			ImGui::NewLine();
		
		FRAC.exterior_Alpha = (fp64)temp_Color.exterior_Alpha;
			FRAC.exterior_R_Amp = (fp64)temp_Color.exterior_R_Amp; FRAC.exterior_R_Freq = (fp64)temp_Color.exterior_R_Freq; FRAC.exterior_R_Phase = (fp64)temp_Color.exterior_R_Phase;
			FRAC.exterior_G_Amp = (fp64)temp_Color.exterior_G_Amp; FRAC.exterior_G_Freq = (fp64)temp_Color.exterior_G_Freq; FRAC.exterior_G_Phase = (fp64)temp_Color.exterior_G_Phase;
			FRAC.exterior_B_Amp = (fp64)temp_Color.exterior_B_Amp; FRAC.exterior_B_Freq = (fp64)temp_Color.exterior_B_Freq; FRAC.exterior_B_Phase = (fp64)temp_Color.exterior_B_Phase;
		FRAC.interior_Alpha = (fp64)temp_Color.interior_Alpha;
			FRAC.interior_R_Amp = (fp64)temp_Color.interior_R_Amp; FRAC.interior_R_Freq = (fp64)temp_Color.interior_R_Freq; FRAC.interior_R_Phase = (fp64)temp_Color.interior_R_Phase;
			FRAC.interior_G_Amp = (fp64)temp_Color.interior_G_Amp; FRAC.interior_G_Freq = (fp64)temp_Color.interior_G_Freq; FRAC.interior_G_Phase = (fp64)temp_Color.interior_G_Phase;
			FRAC.interior_B_Amp = (fp64)temp_Color.interior_B_Amp; FRAC.interior_B_Freq = (fp64)temp_Color.interior_B_Freq; FRAC.interior_B_Phase = (fp64)temp_Color.interior_B_Phase;
	ImGui::Unindent(); }

	ImGui::End();
}

// Until it gets its own proper menu
void SubMenu_SuperScreenshot() {
	if (ImGui::CollapsingHeader("Unimplemented Screenshot Feature")) {
		ImGui::Button("Clear Markers");
		ImGui::Button("Set Top-Left Cordinate Marker");
		ImGui::Button("Set Bottom-Right Cordinate Marker");
		ImGui::NewLine();
		ImGui::Text("Bounding Box: (Unimplemented)");
		static int_enum Combo_BoundingBox = Namespace_Image_Render_Bounding_Box::Fill_Area;
		ImGui::Combo("##SuperScreenshotBoundingBox", &Combo_BoundingBox,
			Namespace_Image_Render_Bounding_Box::Image_Render_Bounding_Box_Text,
			ARRAY_LENGTH(Namespace_Image_Render_Bounding_Box::Image_Render_Bounding_Box_Text)
		);
		Item_Tooltip("Which area should be used for taking the sceenshot");
		ImGui::NewLine();
		ImGui::Button("Render Image");
	}
}

/* Rendering Selection */

	constexpr nano64_t Rendering_Selection_Error_Message_Duration = SECONDS_TO_NANO(2.5);

	void SubMenu_Rendering_Precision(const char* label, const char* tooltip_message = nullptr) {
		using namespace Rendering_Configuration;
		int_enum Combo_Rendering_Precision = Render_Config.get_Render_Precision();
		static std::string message_Rendering_Precision = "";
		static nano64_t timer_Rendering_Precision = 0;

		ImGui::Text("Rendering Precision:"); ImGui::SameLine(); ImGui::TextColored(
			get_Theme_Highlight_Color(), "%s",
			Rendering_Precision_Text[Render_Config.get_Render_Precision()]
		);
		if (getNanoTime() - Rendering_Selection_Error_Message_Duration < timer_Rendering_Precision) {
			ImGui::Button(message_Rendering_Precision.c_str());
		} else {
			if (ImGui::Combo(label, &Combo_Rendering_Precision,
				Rendering_Precision_Text,
				ARRAY_LENGTH(Rendering_Precision_Text)
			)) {
				if (
					(Render_Config.validate_Rendering_Precision(
						(Rendering_Precision)Combo_Rendering_Precision
					) == false) && (Combo_Rendering_Precision != Render_Precision_Automatic)
				) {
					timer_Rendering_Precision = getNanoTime();
					message_Rendering_Precision = "[";
					message_Rendering_Precision += Rendering_Precision_Name[Combo_Rendering_Precision];
					message_Rendering_Precision += "] is not available on your hardware";
				}
				if (Combo_Rendering_Precision != Render_Precision_Automatic) {
					Render_Config.suggest_Render_Precision_and_Fastest_Method(
						(Rendering_Precision)Combo_Rendering_Precision
					);
				}
			}
			if (tooltip_message != nullptr) {
				Item_Tooltip(tooltip_message);
			}
		}
	}
	void SubMenu_Rendering_Method(const char* label, const char* tooltip_message = nullptr) {
		using namespace Rendering_Configuration;
		int_enum Combo_Rendering_Method = Render_Config.get_Render_Method();
		static std::string message_Rendering_Method = "";
		static nano64_t timer_Rendering_Method = 0;

		ImGui::Text("Rendering Method:"); ImGui::SameLine(); ImGui::TextColored(
			get_Theme_Highlight_Color(), "%s",
			Rendering_Method_Text[Render_Config.get_Render_Method()]
		);
		if (getNanoTime() - Rendering_Selection_Error_Message_Duration < timer_Rendering_Method) {
			ImGui::Button(message_Rendering_Method.c_str());
		} else {
			if (ImGui::Combo(label, &Combo_Rendering_Method,
				Rendering_Method_Text,
				ARRAY_LENGTH(Rendering_Method_Text)
			)) {
				if (
					(Render_Config.validate_Rendering_Method(
						(Rendering_Method)Combo_Rendering_Method
					) == false) && (Combo_Rendering_Method != Render_Method_Automatic)
				) {
					timer_Rendering_Method = getNanoTime();
					message_Rendering_Method = "[";
					message_Rendering_Method += Rendering_Method_Name[Combo_Rendering_Method];
					message_Rendering_Method += "] is not available on your hardware";
				}
				if (Combo_Rendering_Method != Render_Method_Automatic) {
					Render_Config.suggest_Render_Method(
						(Rendering_Method)Combo_Rendering_Method
					);
				}
			}
			if (tooltip_message != nullptr) {
				Item_Tooltip(tooltip_message);
			}
		}
	}
	void SubMenu_Rendering_Preset(const char* label, const char* tooltip_message = nullptr) {
		using namespace Rendering_Configuration;
		int_enum Combo_Rendering_Preset = Render_Config.get_Render_Preset();
		static std::string message_Rendering_Preset = "";
		static nano64_t timer_Rendering_Preset = 0;

		ImGui::Text("Rendering Preset:"); ImGui::SameLine(); ImGui::TextColored(
			get_Theme_Highlight_Color(), "%s",
			Rendering_Preset_Text[Render_Config.get_Render_Preset()]
		);
		if (getNanoTime() - Rendering_Selection_Error_Message_Duration < timer_Rendering_Preset) {
			ImGui::Button(message_Rendering_Preset.c_str());
		} else {
			if (ImGui::Combo(label, &Combo_Rendering_Preset,
				Rendering_Preset_Text,
				ARRAY_LENGTH(Rendering_Preset_Text)
			)) {
				if (
					(Render_Config.validate_Rendering_Preset(
						(Rendering_Preset)Combo_Rendering_Preset
					) == false) && (Combo_Rendering_Preset != Render_Preset_Automatic)
				) {
					timer_Rendering_Preset = getNanoTime();
					message_Rendering_Preset = "[";
					message_Rendering_Preset += Rendering_Preset_Name[Combo_Rendering_Preset];
					message_Rendering_Preset += "] is not available on your hardware";
				}
				if (Combo_Rendering_Preset != Render_Preset_Automatic) {
					Render_Config.suggest_Render_Preset(
						(Rendering_Preset)Combo_Rendering_Preset
					);
				}
			}
			if (tooltip_message != nullptr) {
				Item_Tooltip(tooltip_message);
			}
		}
	}

void Menu_Rendering() {
	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 240, 400,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 160, 320
	);
	
	User_Rendering_Settings& Rendering_Settings = config_data.Rendering_Settings;

	int32_t input_subSample = (int32_t)primaryRenderData.subSample;
	int32_t input_superSample = (int32_t)primaryRenderData.sample;
	int32_t CPU_ThreadCount = (int32_t)std::thread::hardware_concurrency();
	static int32_t input_CPU_MaxThreads = ((CPU_ThreadCount <= 1) ? 1 : (CPU_ThreadCount - 1));
	static int32_t input_CPU_ThreadMultiplier = 1;

	static int32_t input_super_CPU_MaxThreads = super_screenshot_maxThreads;
	static int32_t input_super_CPU_ThreadMultiplier = super_screenshot_threadMultiplier;
	

	ImGui::Begin("Rendering Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);

	/* Basic Configuration */ {
		ImGui::Text("Sub Sample: %" PRId32, input_subSample * input_subSample);
		if (ImGui::SliderInt("##input_subSample",&input_subSample,1,24,"")) {
			primaryRenderData.subSample = input_subSample;
		}
		ImGui::Text("Samples per pixel: %" PRId32, input_superSample * input_superSample);
		if (ImGui::SliderInt("##input_superSample", &input_superSample, 1, 24, "")) {
			primaryRenderData.sample = input_superSample;
		}
		dim32_t totalResX = primaryRenderData.resX * primaryRenderData.sample / primaryRenderData.subSample;
		dim32_t totalResY = primaryRenderData.resY * primaryRenderData.sample / primaryRenderData.subSample;
		ImGui::Text("Total Pixels Rendered: %" PRId32 "x%" PRId32 " %.3lfMP",totalResX,totalResY,(fp64)(totalResX * totalResY) / 1000000.0);
		ImGui::NewLine();
		SubMenu_Rendering_Precision("##input_precision_simple",
			"Select a higher precision to zoom further (at the cost of rendering speed)"
		);
		ImGui::NewLine();
	}

	ImGui::SeparatorText("CATEGORIES:"); {
		if (ImGui::CollapsingHeader("JULIA CORDINATE POINT")) { ImGui::Indent();
			constexpr fp32 defaultOutterRadius = 8.0f;
			constexpr fp32 defaultInnerRadius = 2.4f;
			constexpr fp32 maxOutterRadius = 24.0f;
			constexpr fp32 maxInnerRadius = maxOutterRadius - 1.0f;
			ImGui::Checkbox("Display Julia Point", &Rendering_Settings.JuliaPoint_Enabled);
			ImGui::Text("Outer-Radius:");
			if (ImGui::SliderFloat("##OuterRadius",&Rendering_Settings.JuliaPoint_OuterRadius, 1.0f, maxOutterRadius, "%.2f")) {
				valueClamp(Rendering_Settings.JuliaPoint_InnerRadius, 0.0f, Rendering_Settings.JuliaPoint_OuterRadius - 1.0f);
			}
			ImGui::Text("Inner-Radius:");
			if (ImGui::SliderFloat("##InnerRadius",&Rendering_Settings.JuliaPoint_InnerRadius, 0.0f, maxInnerRadius, "%.2f")) {
				valueClamp(Rendering_Settings.JuliaPoint_OuterRadius, Rendering_Settings.JuliaPoint_InnerRadius + 1.0f, maxOutterRadius);
			}
			if (ImGui::Button("Reset inner and outer radius")) {
				Rendering_Settings.JuliaPoint_OuterRadius = defaultOutterRadius;
				Rendering_Settings.JuliaPoint_InnerRadius = defaultInnerRadius;
			}
			ImGui::NewLine();
		ImGui::Unindent(); }
		if (ImGui::CollapsingHeader("SUPER SCREENSHOT SETTINGS")) { ImGui::Indent();

			ImGui::Text("Bounding Box:");
			ImGui::Combo("##SuperScreenshotBoundingBox", &config_data.Rendering_Settings.Image_Render_Bounding_Box,
				Namespace_Image_Render_Bounding_Box::Image_Render_Bounding_Box_Text,
				ARRAY_LENGTH(Namespace_Image_Render_Bounding_Box::Image_Render_Bounding_Box_Text)
			); Item_Tooltip("Which area should be used for taking the sceenshot");
			//SubMenu_SuperScreenshot();
			ImGui::NewLine();
			
			static fp32 temp_super_screenshot_maxItr = log2((fp32)default_Super_Screenshot_MaxItr);
			ImGui::Text("Maximum Iterations: %" PRId32,super_screenshot_maxItr);
			ImGui::SliderFloat("##temp_super_screenshot_maxItr",&temp_super_screenshot_maxItr,log2(16.0f),log2(16777216.0f),"");
			super_screenshot_maxItr = (uint32_t)(pow(2.0f,temp_super_screenshot_maxItr));
			valueClamp(super_screenshot_maxItr,16,16777216); valueClamp(temp_super_screenshot_maxItr,log2(16.0f),log2(16777216.0f));

			constexpr uint64_t MaximumImageSize = (uint64_t)2147000000; // INT32_MAX minus some arbritrary overhead amount

			ImGui::Text("Samples per pixel: %" PRId32,super_screenshot_super_sample * super_screenshot_super_sample);
			ImGui::SliderInt("##super_screenshot_super_sample",&super_screenshot_super_sample,1,32,"");
			size_t totalResX = (size_t)super_screenshot_resX * (size_t)super_screenshot_super_sample;
			size_t totalResY = (size_t)super_screenshot_resY * (size_t)super_screenshot_super_sample;

			ImGui::NewLine();

			int Combo_Common_ResolutionPreset = 0;

			const uint32_t Combo_Common_ResolutionPreset_RESX[] = {0,640,1280,1920,2560,3840,5120,7680};
			const uint32_t Combo_Common_ResolutionPreset_RESY[] = {0,480, 720,1080,1440,2160,2880,4320};
			static const char* Common_ResolutionPreset[] = {
				"Select a preset","640x480 SD","1280x720 HD","1920x1080 FHD","2560x1440 QHD","3840x2160 4K","5120x2880 5K","7680x4320 8K"
			};
			static_assert(
				ARRAY_LENGTH(Common_ResolutionPreset) == ARRAY_LENGTH(Combo_Common_ResolutionPreset_RESX) &&
				ARRAY_LENGTH(Common_ResolutionPreset) == ARRAY_LENGTH(Combo_Common_ResolutionPreset_RESY)
			);
			
			ImGui::Text("Resolution Presets:");
			if (ImGui::Combo("##Common_Resolutions",&Combo_Common_ResolutionPreset,BufAndLen(Common_ResolutionPreset))) {
				if (Combo_Common_ResolutionPreset != 0) {
					super_screenshot_resX = (int32_t)Combo_Common_ResolutionPreset_RESX[Combo_Common_ResolutionPreset];
					super_screenshot_resY = (int32_t)Combo_Common_ResolutionPreset_RESY[Combo_Common_ResolutionPreset];
				}
			}

			ImGui::Text("Resolution X:");
			ImGui::InputInt("##super_screenshot_resX",&super_screenshot_resX,16,64);
			super_screenshot_resX &= 0x7FFFFFFC; // Multiple of 4
			valueClamp(super_screenshot_resX,64,65536); valueMaximumClamp(super_screenshot_resX,(int32_t)MaximumImageSize / super_screenshot_resY / 3);
			ImGui::Text("Resolution Y:");
			ImGui::InputInt("##super_screenshot_resY",&super_screenshot_resY,16,64);
			valueClamp(super_screenshot_resY,64,65536); valueMaximumClamp(super_screenshot_resY,(int32_t)MaximumImageSize / super_screenshot_resX / 3);
			
			if (ImGui::Button("Use Current Screen Resolution##SuperScreenshotUseCurrentScreenResolution")) {
				const DisplayInfo* disp = getDisplayFromWindowPosition(window);
				if (disp != nullptr) {
					disp->getResolution(super_screenshot_resX, super_screenshot_resY);
				}
			}

			ImGui::NewLine();
			ImGui::Text("Total Pixels Rendered: %zux%zu %.3lfMP",totalResX,totalResY,(fp64)(totalResX * totalResY) / 1000000.0);
			if ((uint64_t)super_screenshot_resX * (uint64_t)super_screenshot_resY * (uint64_t)IMAGE_BUFFER_CHANNELS >= 1000000000) {
				ImGui::Text("Current Image Size: %.1lf megabytes",
					(fp64)((uint64_t)super_screenshot_resX * (uint64_t)super_screenshot_resY * (uint64_t)IMAGE_BUFFER_CHANNELS) / 1000000.0
				);
				ImGui::Text("Maximum Image Size: %.1lf megabytes",
					(fp64)(MaximumImageSize) / 1000000.0
				);
			}
			ImGui::NewLine();
			if (ImGui::Button("Take Super Screenshot")) {
				exportSuperScreenshot();
				//exportScreenshot();
			}
			ImGui::NewLine();
		ImGui::Unindent(); }
		if (ImGui::CollapsingHeader("FRAME INTERPOLATION")) { ImGui::Indent();
			#ifndef Enable_OpenCV_Scaler
				ImGui::Checkbox("Render Background Color", &Render_Background_Color);
				ImGui::NewLine();
			#endif
			#ifdef Enable_OpenCV_Scaler
				static const char* OpenCV_interpolation_mode_list[] = {"Nearest Neighbor (Default)","Linear","Cubic","Area","Lanczos"};
				int_enum& OpenCV_interpolation_mode = config_data.Rendering_Settings.Frame_Interpolation_Method;
				ImGui::Text("Frame Interpolation Method:");
				if (ImGui::Combo("##Frame_Interpolation_Method", &OpenCV_interpolation_mode, BufAndLen(OpenCV_interpolation_mode_list))) {
					// Should probably be replaced with a Map instead.
					// switch (OpenCV_interpolation_mode) {
					// 	case OPENCV_Interpolation::OPENCV_INTER_NEAREST:
					// 		Frame_Interpolation_Method = cv::INTER_NEAREST;
					// 	break;
					// 	case OPENCV_Interpolation::OPENCV_INTER_LINEAR:
					// 		Frame_Interpolation_Method = cv::INTER_LINEAR;
					// 	break;
					// 	case OPENCV_Interpolation::OPENCV_INTER_CUBIC:
					// 		Frame_Interpolation_Method = cv::INTER_CUBIC;
					// 	break;
					// 	case OPENCV_Interpolation::OPENCV_INTER_AREA:
					// 		Frame_Interpolation_Method = cv::INTER_AREA;
					// 	break;
					// 	case OPENCV_Interpolation::OPENCV_INTER_LANCZOS4:
					// 		Frame_Interpolation_Method = cv::INTER_LANCZOS4;
					// 	break;
					// 	default:
					// 		Frame_Interpolation_Method = cv::INTER_NEAREST;
					// };
				}
				ImGui::Text("Nearest Neighbor is the fastest method. Other methods might not be able to hit 60.0fps at higher resolutions.");
				ImGui::NewLine();
			#endif
		ImGui::Unindent(); }
		if (ImGui::CollapsingHeader("CPU INFORMATION")) { ImGui::Indent();
			const Supported_CPU_Instruction& Available_CPU_Instruction = get_Available_CPU_Instruction();
			
			ImGui::Text("CPU Threads: %u", std::thread::hardware_concurrency());
			ImGui::Text("System RAM: %dMiB", SDL_GetSystemRAM());
			ImGui::NewLine();
			ImGui::Text("SSE2 Rendering: %s",
				Enable_Text(Available_CPU_Instruction.SSE_Family.SSE2)
			); Item_Tooltip("SSE2 allows the CPU to process 4 32bit floats or 2 64bit floats at a time."\
				"\nSSE2 Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.SSE_Family.SSE2)
			);
			ImGui::Text("AVX Rendering: %s",
				Enable_Text(Available_CPU_Instruction.AVX_Family.AVX)
			); Item_Tooltip("AVX allows the CPU to process 8 32bit floats or 4 64bit floats at a time."\
				"\nAVX Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.AVX_Family.AVX)
			);
			ImGui::Text("AVX512F Rendering: %s",
				Enable_Text(Available_CPU_Instruction.AVX512_Family.AVX512_F)
			); Item_Tooltip("AVX512F allows the CPU to process 16 32bit floats or 8 64bit floats at a time."\
				"\nAVX512F Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.AVX512_Family.AVX512_F)
			);
			ImGui::NewLine();

			ImGui::SeparatorText("CPU Instruction Sets"); {
				
				ImGui::BeginChild(
					"CPU_InstructionList", ImVec2(0.0f, 108.0f), true
				);
					
				ImGui::Text("CPU Signature: <%s>", Available_CPU_Instruction.CPU_Signature);
				ImGui::NewLine();
				ImGui::Text("SSE Family:"); {
					const Supported_SSE_Family_Instruction& SSE_Family = Available_CPU_Instruction.SSE_Family;
					size_t count = 0;
					if (Available_CPU_Instruction.MMX) {
						ImGui::SameLine(); ImGui::Text("MMX"); count++;
					}
					if (SSE_Family.SSE   ) { ImGui::SameLine(); ImGui::Text("SSE"   ); count++; }
					if (SSE_Family.SSE2  ) { ImGui::SameLine(); ImGui::Text("SSE2"  ); count++; }
					if (SSE_Family.SSE3  ) { ImGui::SameLine(); ImGui::Text("SSE3"  ); count++; }
					if (SSE_Family.SSSE3 ) { ImGui::SameLine(); ImGui::Text("SSSE3" ); count++; }
					if (SSE_Family.SSE4_1) { ImGui::SameLine(); ImGui::Text("SSE4_1"); count++; }
					if (SSE_Family.SSE4_2) { ImGui::SameLine(); ImGui::Text("SSE4_2"); count++; }
					if (SSE_Family.SSE4a ) { ImGui::SameLine(); ImGui::Text("SSE4a" ); count++; }
					if (count == 0) {
						ImGui::SameLine(); ImGui::Text("<None>");
					}
				}
				ImGui::Text("AVX Family:"); {
					const Supported_AVX_Family_Instruction& AVX_Family = Available_CPU_Instruction.AVX_Family;
					size_t count = 0;
					if (AVX_Family.AVX           ) { ImGui::SameLine(); ImGui::Text("AVX"           ); count++; }
					if (AVX_Family.F16C          ) { ImGui::SameLine(); ImGui::Text("F16C"          ); count++; }
					if (AVX_Family.FMA           ) { ImGui::SameLine(); ImGui::Text("FMA"           ); count++; }
					if (AVX_Family.AVX2          ) { ImGui::SameLine(); ImGui::Text("AVX2"          ); count++; }
					if (AVX_Family.AVX_VNNI      ) { ImGui::SameLine(); ImGui::Text("AVX_VNNI"      ); count++; }
					if (AVX_Family.AVX_VNNI_INT8 ) { ImGui::SameLine(); ImGui::Text("AVX_VNNI_INT8" ); count++; }
					if (AVX_Family.AVX_NE_CONVERT) { ImGui::SameLine(); ImGui::Text("AVX_NE_CONVERT"); count++; }
					if (AVX_Family.AVX_IFMA      ) { ImGui::SameLine(); ImGui::Text("AVX_IFMA"      ); count++; }
					if (count == 0) {
						ImGui::SameLine(); ImGui::Text("<None>");
					}
				}
				ImGui::Text("AVX512 Family:"); {
					const Supported_AVX512_Family_Instruction& AVX512_Family = Available_CPU_Instruction.AVX512_Family;
					size_t count = 0;
					if (AVX512_Family.AVX512_F        ) { ImGui::SameLine(); ImGui::Text("AVX512_F"        ); count++; }
					if (AVX512_Family.AVX512_BW       ) { ImGui::SameLine(); ImGui::Text("AVX512_BW"       ); count++; }
					if (AVX512_Family.AVX512_CD       ) { ImGui::SameLine(); ImGui::Text("AVX512_CD"       ); count++; }
					if (AVX512_Family.AVX512_DQ       ) { ImGui::SameLine(); ImGui::Text("AVX512_DQ"       ); count++; }
					if (AVX512_Family.AVX512_IFMA52   ) { ImGui::SameLine(); ImGui::Text("AVX512_IFMA52"   ); count++; }
					if (AVX512_Family.AVX512_VL       ) { ImGui::SameLine(); ImGui::Text("AVX512_VL"       ); count++; }
					if (AVX512_Family.AVX512_VPOPCNTDQ) { ImGui::SameLine(); ImGui::Text("AVX512_VPOPCNTDQ"); count++; }
					if (AVX512_Family.AVX512_BF16     ) { ImGui::SameLine(); ImGui::Text("AVX512_BF16"     ); count++; }
					if (AVX512_Family.AVX512_BITALG   ) { ImGui::SameLine(); ImGui::Text("AVX512_BITALG"   ); count++; }
					if (AVX512_Family.AVX512_VBMI     ) { ImGui::SameLine(); ImGui::Text("AVX512_VBMI"     ); count++; }
					if (AVX512_Family.AVX512_VBMI2    ) { ImGui::SameLine(); ImGui::Text("AVX512_VBMI2"    ); count++; }
					if (AVX512_Family.AVX512_VNNI     ) { ImGui::SameLine(); ImGui::Text("AVX512_VNNI"     ); count++; }
					if (AVX512_Family.AVX512_FP16     ) { ImGui::SameLine(); ImGui::Text("AVX512_FP16"     ); count++; }
					if (count == 0) {
						ImGui::SameLine(); ImGui::Text("<None>");
					}
				}
				ImGui::EndChild();
				ImGui::NewLine();
			}
		ImGui::Unindent(); }
		#ifdef Enable_OpenCL
			if (ImGui::CollapsingHeader("GPU INFORMATION")) { ImGui::Indent();
				SubMenu_GPU_Information();
			ImGui::Unindent(); }
		#endif
	}
	ImGui::SeparatorText("ADVANCED RENDERING SETTINGS:"); {
		if (ImGui::CollapsingHeader("RENDERING CONFIGURATION")) { ImGui::Indent();
			
			ImGui::TextWrapped(
				"Note: The closest available rendering configuration will be used if the entered rendering configuration is unsupported/unavailable."
			);
			ImGui::NewLine();
			SubMenu_Rendering_Precision("##input_precision_advanced");
			SubMenu_Rendering_Method("##input_method_advanced");
			SubMenu_Rendering_Preset("##input_preset_advanced");
			ImGui::NewLine();
		ImGui::Unindent(); }
		if (ImGui::CollapsingHeader("CPU MULTI-THREADING SETTINGS")) { ImGui::Indent();
			ImGui::Text("Note: Only modify these settings if you know what you are doing.");
			ImGui::Text("Maximum Threads:");
			ImGui::SliderInt("##input_CPU_MaxThreads",&input_CPU_MaxThreads,1,CPU_ThreadCount);
			ImGui::Text("Thread Multiplier:");
			ImGui::SliderInt("##input_CPU_ThreadMultiplier",&input_CPU_ThreadMultiplier,1,16);
			ImGui::NewLine();
			ImGui::Text("Super Screenshot:");
			ImGui::Text("Maximum Threads:");
			ImGui::SliderInt("##input_Super_CPU_MaxThreads",&input_super_CPU_MaxThreads,1,CPU_ThreadCount);
			ImGui::Text("Thread Multiplier:");
			ImGui::SliderInt("##input_Super_CPU_ThreadMultiplier",&input_super_CPU_ThreadMultiplier,1,16);
			ImGui::NewLine();
			primaryRenderData.CPU_Threads = (uint32_t)(input_CPU_MaxThreads * input_CPU_ThreadMultiplier);
			super_screenshot_threadMultiplier = input_CPU_ThreadMultiplier;
			super_screenshot_maxThreads = input_CPU_MaxThreads;
			ImGui::NewLine();
		ImGui::Unindent(); }
		#ifndef BUILD_RELEASE
			if (ImGui::CollapsingHeader("GPU ADVANCED SETTINGS")) { ImGui::Indent();
				ImGui::Indent();
				ImGui::Text("Note: Only modify these settings if you know what you are doing.");
				ImGui::Text("GPU Render Partitions: (Default = 1)");
				ImGui::InputInt("##input_GPU_Partitions",(int32_t*)(&primaryRenderData.GPU_Partitions),1,16);
				valueClamp(primaryRenderData.GPU_Partitions,1,1024);
				ImGui::TextWrapped(
					"Increasing the amount of partitions can reduce the time it takes for the GPU to quit rendering when the Abort Rendering button is pressed. "\
					"However, increasing the rendering paritions can cause some performance loss due to the overhead of rendering smaller chunks of the fractal at a time. "\
					"For the best performance, set render paritions to 1."
				);
				ImGui::NewLine();
			ImGui::Unindent(); }
		#endif
	}



	// { // Doesn't work
	// 	dim32_t resX, resY, dimX, dimY;
	// 	SDL_GetWindowSize(window,&resX,&resY);
	// 	SDL_GetWindowSizeInPixels(window,&dimX,&dimY);
	// 	if (resX != dimX || resY != dimY) {
	// 		ImGui::Text(
	// 			"Warning: Window is rendering in high dpi mode, and may have scaling artifacts %" PRId32 "x%" PRId32 " != %" PRId32 "x%" PRId32,
	// 			resX, resY, dimX, dimY
	// 		);
	// 	}
	// }

	ImGui::End();
}

void printDisplayInfo(const DisplayInfo* Disp, bool printWarning = true) {
	if (Disp == nullptr) {
		if (printWarning == true) {
			ImGui::Text("<Unable to print display information>");
		}
		return;
	}
	int32_t resX, resY, posX, posY;
	Disp->getResolution(resX,resY);
	Disp->getPosition(posX,posY);
	ImGui::Text(
		"Display[%" PRId32 "]: %" PRId32 "x%" PRId32 " at %" PRId32 "Hz (%" PRId32 ",%" PRId32 ") | %s",
		Disp->getIndex(),
		resX, resY,
		(int32_t)Disp->getRefreshRate(),
		posX, posY,
		Disp->getName().c_str()
	);
}

void Menu_Settings() {
	// static const char* initFrameRate[] = {
	// 	"Current Monitor","Highest Refresh-Rate","Lowest Refresh-Rate","Constant Value"
	// };
	
	// static const char* initMonitorLocations[] = {
	// 	"Automatic","Cursor Position","First Monitor","Last Monitor","Specific Monitor",
	// 	"Left","Right","Center","Top","Bottom","Top-Left","Top-Right","Bottom-Left","Bottom-Right",
	// 	"Highest Resolution","Lowest Resolution","Highest Framerate","Lowest Framerate","Widest Aspect Ratio","Tallest Aspect Ratio"
	// };

	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 240, 400,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 160, 320
	);

	ImGui::Begin("Settings Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);

	User_GUI_Settings& config_GUI_Settings = config_data.GUI_Settings;

	User_Display_Preferences& config_Display = config_data.Display_Preferences;

	ImGui::Text("ABS-Fractal-Explorer v%s (%s)", PROGRAM_VERSION, PROGRAM_DATE);
	ImGui::Separator();
	ImGui::Checkbox("Lock key inputs in menus",&config_data.GUI_Settings.LockKeyInputsInMenus);
	ImGui::NewLine();
	#ifdef PLATFORM_WINDOWS
		if(ImGui::Button("Import fracExpConfig")) {
			static char filePath[324]; memset(filePath,'\0',sizeof(filePath));
			openFileInterface(
				filePath,sizeof(filePath),
				"Select a FracExp Configuration File",
				"Config Files (*.fracExpConfig)\0*.fracExpConfig\0"\
				"All Files (*.*)\0*.*\0"
			);
			import_config_data(config_data,filePath);
			refresh_IMGUI(config_data);
		}
		if(ImGui::Button("Export fracExpConfig")) {
			static char filePath[324]; memset(filePath,'\0',sizeof(filePath));
			saveFileInterface(
				filePath,sizeof(filePath),
				"Save FracExp-Configuration File",
				"Config Files (*.fracExpConfig)\0*.fracExpConfig\0"\
				"All Files (*.*)\0*.*\0",
				"config"
			);
			export_config_data(config_data,filePath);
		}
		ImGui::NewLine();
	#else
		{
			static char filePath[324] = "./config.fracExpConfig";
			ImGui::Text("Import .fracExpConfig Path:");
			ImGui::InputText("##Import_fracExpConfig_Path",filePath, sizeof(filePath));
			if (ImGui::Button("Load FracExp-Configuration File from path")) {
				import_config_data(config_data,filePath);
			}
			ImGui::NewLine();
		}
		{
			static char filePath[324] = "./config.fracExpConfig";
			ImGui::Text("Export .fracExpConfig Path:");
			ImGui::InputText("##Export_fracExpConfig_Path",filePath, sizeof(filePath));
			if (ImGui::Button("Save FracExp-Configuration File to path")) {
				export_config_data(config_data,filePath);
			}
			ImGui::NewLine();
		}
	#endif
	ImGui::Checkbox("Automatically load fracExpConfig File",&config_data.Automatic_Behaviour.AutoLoad_Config_File);
	ImGui::Checkbox("Automatically save fracExpConfig File",&config_data.Automatic_Behaviour.AutoSave_Config_File);
	ImGui::NewLine();
	ImGui::SeparatorText("CATEGORIES:");
	if (ImGui::CollapsingHeader("MENU WINDOW SETTINGS")) { ImGui::Indent();
		{
			static const char* GUI_Theme_Options[] = {
				"Classic","Dark-mode (Default)","Light-mode"
			};
			ImGui::Text("ImGui Theme:");
			if (ImGui::Combo("##IMGUI_Theme",&config_GUI_Settings.GUI_Theme,BufAndLen(GUI_Theme_Options))) {
				set_IMGUI_Theme((Display_GUI::IMGUI_Theme)config_GUI_Settings.GUI_Theme);
			}
		}
		ImGui::Checkbox("Prevent out of bounds menu windows",&config_data.GUI_Settings.PreventOutOfBoundsWindows);
		ImGui::Checkbox("Auto-resize menu windows",&config_data.GUI_Settings.AutoResizeWindows);
		ImGui::Text("Window Auto-Scale: (0.7 default)");
		fp32 temp_WindowAutoScale = (fp32)config_data.GUI_Settings.WindowAutoScale;
		ImGui::SliderFloat("##WindowAutoScale",&temp_WindowAutoScale,0.3f,1.0f,"%.3f");
		config_data.GUI_Settings.WindowAutoScale = (fp64)temp_WindowAutoScale;
		ImGui::Text("Window Opacity: (0.95 default)");
		ImGui::SliderFloat("##WindowOpacity",&config_data.GUI_Settings.WindowOpacity,0.3f,1.0f,"%.3f");
		
		ImGui::NewLine();
	ImGui::Unindent(); }
	if (ImGui::CollapsingHeader("DISPLAYS AND FRAME-RATE")) { ImGui::Indent();
		// Completely Arbtritrary
		constexpr int32_t Maximum_Allowed_Displays = 144;

		int32_t cursorPosX, cursorPosY; SDL_GetGlobalMouseState(&cursorPosX, &cursorPosY);
		int32_t windowPosX, windowPosY; SDL_GetWindowPosition(window, &windowPosX, &windowPosY);
		int32_t windowResX, windowResY; SDL_GetWindowSize(window, &windowResX, &windowResY);
		ImGui::Text("Display Count: %" PRId32, getDisplayCount());
		
		static bool changesToDisplayList = false;
		static nano64_t displayTimer = 0;
		static nano64_t displayDuration = SECONDS_TO_NANO(1.0);
		if (getNanoTime() - displayTimer < displayDuration) {
			if (changesToDisplayList == true) {
				ImGui::Button("Detected changes to display configuration");
			} else {
				ImGui::Button("No changes detected");
			}
		} else {
			if (ImGui::Button("Reload/Update Displays")) {
				reloadDisplays();
				uint64_t newDisplayHash = getDisplayConfigHash();
				changesToDisplayList =
				(config_Display.Display_Config_Hash == newDisplayHash) ? false : true;
				config_Display.Display_Config_Hash = newDisplayHash;
				displayTimer = getNanoTime();
				displayDuration = (changesToDisplayList == true) ? SECONDS_TO_NANO(2.5) : SECONDS_TO_NANO(1.0);
			}
		}

		if (getDisplayCount() != 0) {
			fp32 displayListWidth = ImGui::GetContentRegionAvail().x;
			displayListWidth = calcMinRatioMax(displayListWidth, 384.0f, 0.8f, 768.0f);
			fp32 displayListHeight = (getDisplayCount() > 3) ? 120.0f : 64.0f;
			ImGui::BeginChild(
				"DisplayList", ImVec2(displayListWidth, displayListHeight), true
			);
			const std::vector<DisplayInfo> &DisplayList = getDisplayList();
			for (int32_t i = 0; i < getDisplayCount(); i++) {
				printDisplayInfo(&DisplayList[(size_t)i], false);
			}
			ImGui::EndChild();
		}
		ImGui::NewLine();
		ImGui::SeparatorText("Bootup-Diplay"); {

			ImGui::Text("Which monitor should the application open to:");
			if (ImGui::Combo("##initMonitorLocation", &config_Display.Display_Bootup_Type,
				Display_Bootup::Display_Bootup_Text, ARRAY_LENGTH(Display_Bootup::Display_Bootup_Text)
			)) {
			}
			if (config_Display.Display_Bootup_Type == Display_Bootup::Specific) { // Specific Monitor
				static bool overrideDisplayCount = false;
				// if (overrideDisplayCount == false && config_Display.Specific_Bootup_Display > getDisplayCount()) {
				// 	config_Display.Specific_Bootup_Display = getDisplayCount();
				// }
				if (config_Display.Specific_Bootup_Display > getDisplayCount()) {
					overrideDisplayCount = true;
				}
				int32_t limitDisplayCount = (overrideDisplayCount == false || getDisplayCount() > Maximum_Allowed_Displays) ? getDisplayCount() : Maximum_Allowed_Displays;
				if (getDisplayCount() != 1 || overrideDisplayCount == true) {
					if(ImGui::InputInt("##specificMonitor",&config_Display.Specific_Bootup_Display, 1, 1)) {
						valueClamp(config_Display.Specific_Bootup_Display,1,limitDisplayCount);
					} 
				} else {
					ImGui::Text("Only 1 display detected");
				}
				const DisplayInfo* specificDisp = getDisplayFromIndex(config_Display.Specific_Bootup_Display);
				if (specificDisp == nullptr) {
					ImGui::Text("Display %" PRId32 " is not detected",config_Display.Specific_Bootup_Display);
				} else {
					printDisplayInfo(specificDisp);
				}
				if (ImGui::Checkbox("Override Display Count",&overrideDisplayCount)) {
					if (overrideDisplayCount == false) {
						valueClamp(config_Display.Specific_Bootup_Display,1,getDisplayCount());
					}
				}
				if (config_Display.Specific_Bootup_Display > getDisplayCount()) {
					ImGui::Text(
						"Note: Display %" PRId32 " will be used if Display %" PRId32 " is not detected",
						getDisplayCount(),
						config_Display.Specific_Bootup_Display
					);
				}
			} else if (true
				// (config_Display.Display_Bootup_Type != Display_Bootup::Automatic) &&
				// (config_Display.Display_Bootup_Type != Display_Bootup::CursorPosition)
			) {
				const DisplayInfo* initDisp = matchDisplayAttribute(
					(Display_Bootup::Display_Bootup_Enum)config_Display.Display_Bootup_Type,
					config_Display,
					RESX_Minimum, RESY_Minimum,
					cursorPosX, cursorPosY
				);
				printDisplayInfo(initDisp);
			}
			ImGui::NewLine();
			ImGui::Checkbox("Scale window based on screen-size", &config_Display.ScaleWindowToScreenSize);
			if (config_Display.ScaleWindowToScreenSize == true) {
				ImGui::Text("Bootup window scale: (0.6 default)");
				fp32 temp_Bootup_Window_Scale = (fp32)config_Display.Bootup_Window_Scale;
				ImGui::SliderFloat("##slider_bootup_window_scale: ", &temp_Bootup_Window_Scale, 0.0f, 1.0f);
				config_Display.Bootup_Window_Scale = (fp64)temp_Bootup_Window_Scale;
			}
			ImGui::NewLine();
		}
		ImGui::SeparatorText("Windowed/Fullscreen"); {
			using namespace Display_Fullscreen;
			static int_enum Combo_WindowFullscreen = get_Window_Fullscreen_Mode();
			ImGui::Text("Set Current Window Mode:");
			ImGui::Combo("##Current_Fullscreen",&Combo_WindowFullscreen,
				Display_Fullscreen_Text, ARRAY_LENGTH(Display_Fullscreen_Text)
			);
			if (ImGui::Button("Update Window")) {
				set_Window_Fullscreen_Mode((Display_Fullscreen_Enum)Combo_WindowFullscreen);
				Combo_WindowFullscreen = get_Window_Fullscreen_Mode();
			}
			ImGui::NewLine();
			ImGui::Text("Set Bootup Window Mode:");
			ImGui::Combo("##Bootup_Fullscreen",&config_Display.Bootup_Fullscreen,
				Display_Fullscreen_Text, ARRAY_LENGTH(Display_Fullscreen_Text)
			); Item_Tooltip("Choose if the program will start in Fullscreen, Windowed, etc.");
			ImGui::NewLine();
		}
		ImGui::SeparatorText("Frame-Rate"); {
			ImGui::Text("Base maximum frame-rate off of:");
			if (ImGui::Combo("##initFrameRate", &config_Display.Display_RefreshRate_Type,
				Display_RefreshRate::Display_RefreshRate_Text, ARRAY_LENGTH(Display_RefreshRate::Display_RefreshRate_Text)
			)) {

			}
			const DisplayInfo* Select_Display = matchDisplayRefreshRate(
				(Display_Bootup::Display_Bootup_Enum)config_Display.Display_RefreshRate_Type,
				config_Display, window, RESX_Minimum, RESY_Minimum
			);
			
			printDisplayInfo(Select_Display,false);

			static fp64 FPS_Constant_Value = config_Display.Constant_RefreshRate_Value;
			fp64 TEMP_FPS = (Select_Display == nullptr) ? config_Display.Constant_RefreshRate_Value : Select_Display->getRefreshRate();
			
			if (config_Display.Display_RefreshRate_Type == Display_RefreshRate::ConstantValue) { // Constant
				static fp32 temp_FPS_Constant_Value = (fp32)config_Display.Constant_RefreshRate_Value;
				ImGui::NewLine();
				ImGui::Text("%.3lfms",(1.0 / FPS_Constant_Value) * 1000.0);
				ImGui::InputFloat("##temp_FPS_Constant_Value",&temp_FPS_Constant_Value,6.0f,30.0f,"%.3f"); valueClamp(temp_FPS_Constant_Value,12.0f,1200.0f);
				FPS_Constant_Value = (fp64)temp_FPS_Constant_Value;
				if (ImGui::Button("Apply FPS")) {
					config_Display.Constant_RefreshRate_Value = FPS_Constant_Value;
					updateFrameRate(CALC_FRAMERATE_OFFSET(config_Display.Constant_RefreshRate_Value));
				}
			} else { // Relative
				ImGui::NewLine(); // Blank Line
				static int32_t temp_frameMultiplier = config_Display.Maximum_FPS_Multiplier;
				fp64 frameMultiplier;
				//int temp_frameMultiplier = (Default_Frame_Rate_Multiplier >= 0.0) ? (int)(Default_Frame_Rate_Multiplier - 1.0) : (int)(1.0 - 1.0);
				//static fp64 frameMultiplier = Default_Frame_Rate_Multiplier;
				if (temp_frameMultiplier >= 0) {
					frameMultiplier = (fp64)(temp_frameMultiplier + 1);
					ImGui::Text("Maximum FPS Multiplier: %" PRId32 "x",(temp_frameMultiplier + 1));
				} else {
					frameMultiplier = 1.0 / (fp64)(1 - temp_frameMultiplier);
					ImGui::Text("Maximum FPS Multiplier: 1/%" PRId32 "x", (1 - temp_frameMultiplier));
				}
				fp64 calculatedFPS = frameMultiplier * TEMP_FPS;
				valueClamp(calculatedFPS,FRAMERATE_MINIMUM,FRAMERATE_MAXIMUM);
				ImGui::Text("%.2lffps %.2lfms", calculatedFPS, (1.0 / (calculatedFPS)) * 1000.0);
				if (calculatedFPS <= FRAMERATE_MINIMUM) {
					ImGui::SameLine();
					ImGui::Text("(Minimum)");
				} else if (calculatedFPS >= FRAMERATE_MAXIMUM) {
					ImGui::Text("(Maximum)");
				}
				
				ImGui::SliderInt("##temp_frameMultiplier",&temp_frameMultiplier,(-6) + 1,(6) - 1,"");
				if (ImGui::Button("Apply FPS")) {
					config_Display.Maximum_FPS_Multiplier = temp_frameMultiplier;
					updateFrameRate(CALC_FRAMERATE_OFFSET(calculatedFPS));
				}
			}
			ImGui::NewLine(); 
		}
	ImGui::Unindent(); }
	#ifndef BUILD_RELEASE
		if (ImGui::CollapsingHeader("FRACEXP FILES")) { ImGui::Indent();
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
			ImGui::NewLine();
		ImGui::Unindent(); }
	#endif
	if (ImGui::CollapsingHeader("SCREEN-SHOTS")) { ImGui::Indent();
		User_Screenshot_Settings& screenshot_settings = config_data.Screenshot_Settings;
		static int_enum Combo_ScreenshotFileType = screenshot_settings.screenshotFileType;
		static const char* Text_ScreenshotFileType[] = {"PNG","JPG/JPEG","TGA","BMP"};
		ImGui::Text("Screenshot File Type:");
		if (ImGui::Combo("##Combo_ScreenshotFileType",&Combo_ScreenshotFileType,BufAndLen(Text_ScreenshotFileType))) {
			screenshot_settings.screenshotFileType = (Image_File_Format::Image_File_Format_Enum)Combo_ScreenshotFileType;
		}
		if (screenshot_settings.screenshotFileType == Image_File_Format::PNG) {
			int32_t temp_User_PNG_Compression_Level = (int32_t)screenshot_settings.PNG_Compression_Level;
			ImGui::Text("PNG Compression Level (Default = 8)");
			ImGui::SliderInt("##temp_User_PNG_Compression_Level",&temp_User_PNG_Compression_Level,1,9);
			screenshot_settings.PNG_Compression_Level = (uint32_t)temp_User_PNG_Compression_Level;
			if (screenshot_settings.PNG_Compression_Level < 3) { ImGui::Text("Fastest Saving (Large File Size)"); } else
			if (screenshot_settings.PNG_Compression_Level < 5) { ImGui::Text("Faster Saving"); } else
			if (screenshot_settings.PNG_Compression_Level < 7) { ImGui::Text("Balanced"); } else
			if (screenshot_settings.PNG_Compression_Level < 9) { ImGui::Text("Smaller File Size (Recommended)"); } else
			{ ImGui::Text("Smallest File Size"); }
		} else if (screenshot_settings.screenshotFileType == Image_File_Format::JPG) {
			int32_t temp_User_JPG_Quality_Level = (int32_t)screenshot_settings.JPG_Quality_Level;
			ImGui::Text("JPG/JPEG Quality Level (Default = 95)");
			ImGui::SliderInt("##temp_User_JPG_Quality_Level",&temp_User_JPG_Quality_Level,25,100);
			screenshot_settings.JPG_Quality_Level = (uint32_t)temp_User_JPG_Quality_Level;
			if (screenshot_settings.JPG_Quality_Level < 75) { ImGui::Text("Low Quality (Not Recommended)"); } else
			if (screenshot_settings.JPG_Quality_Level < 90) { ImGui::Text("Medium Quality"); } else
			if (screenshot_settings.JPG_Quality_Level < 95) { ImGui::Text("High Quality"); } else
			{ ImGui::Text("Very High Quality (Recommended)"); }
		} else if (screenshot_settings.screenshotFileType == Image_File_Format::TGA || screenshot_settings.screenshotFileType == Image_File_Format::BMP) {
			ImGui::Text("Note: Super Screenshots only support PNG and JPG.");
		}
		
		ImGui::NewLine();
		/* Screenshot path */ {
			ImVec4 highlight_color = get_Theme_Highlight_Color();
			ImGui::Text("Current Screenshot Directory:"); ImGui::SameLine();
			ImGui::TextColored(highlight_color, "%s",
				config_data.File_Paths.Path_Screenshot.c_str()
			);
			#ifdef PLATFORM_WINDOWS
				if (ImGui::Button("Set Screenshot Directory")) {
					static char path_Screenshot[324]; memset(path_Screenshot,'\0',sizeof(path_Screenshot));
					int setDirectoryState = selectFolderInterface(
						path_Screenshot, sizeof(path_Screenshot),
						"Select Screenshot Folder or Directory"
					);
					if (setDirectoryState == 0) { // Success
						config_data.File_Paths.Path_Screenshot.assign(path_Screenshot);
						write_Screenshot_Path(config_data.File_Paths.Path_Screenshot.c_str());
					}
				}
			#else
				static char path_Screenshot[324];

				ImGui::Text("Set Screenshot Directory: (Ending with the slash"); ImGui::SameLine();
				ImGui::TextColored(highlight_color, "/"); ImGui::SameLine(); ImGui::Text("character)");

				ImGui::InputText("##input_path_Screenshot", path_Screenshot, sizeof(path_Screenshot));
				if (
					(strnlen(path_Screenshot, sizeof(path_Screenshot)) > 1) && 
					(path_Screenshot[strnlen(path_Screenshot, sizeof(path_Screenshot)) - 1] == '/')
				) {
					if (ImGui::Button("Apply changes##button_path_Screenshot")) {
						config_data.File_Paths.Path_Screenshot.assign(path_Screenshot);
						if (config_data.File_Paths.Path_Screenshot.length() == 0) {
							config_data.File_Paths.Path_Screenshot = "./";
						}
					}
				} else {
					if (strnlen(path_Screenshot, sizeof(path_Screenshot)) == 0) {
						ImGui::Button("Enter a folder path or directory above##empty_button_path_Screenshot");
					} else {
						ImGui::Button("Directories must end with the slash '/' character##error_button_path_Screenshot");
					}
				}
				
			#endif
			
			ImGui::NewLine();
		}
		
	ImGui::Unindent(); }
	
	if (ImGui::CollapsingHeader("RESET DATA")) { ImGui::Indent();
		ImGui::Text("Reset Configurations:");
		if(ImGui::Button("Default Configuration Data")) {
			default_User_Configuration_Data(config_data, false);
			set_IMGUI_Theme((Display_GUI::IMGUI_Theme)config_data.GUI_Settings.GUI_Theme);
			refresh_IMGUI(config_data);
		}
		if(ImGui::Button("Reset Parameter Sensitivity")) { default_Parameter_Sensitivity(config_data.Parameter_Sensitivity, false); }
		#ifndef BUILD_RELEASE
			if(ImGui::Button("Reset Display Preferences")) { default_Display_Preferences(config_data.Display_Preferences); }
			if(ImGui::Button("Reset GUI Settings")) { default_GUI_Settings(config_data.GUI_Settings, false, false); }
			if(ImGui::Button("Reset Screenshot")) { default_Screenshot_Settings(config_data.Screenshot_Settings); }
			
			ImGui::NewLine();
			if(ImGui::Button("Full Reset")) { 
				default_User_Configuration_Data(config_data, true);
				set_IMGUI_Theme((Display_GUI::IMGUI_Theme)config_data.GUI_Settings.GUI_Theme);
			}
		#endif
		ImGui::NewLine();
	ImGui::Unindent(); }
	
	ImGui::End();
}



void Menu_Status() {
	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 320, 480,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 240, 360
	);
	ImGui::Begin("Program Status",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);

	ImGui::Text("NOT IMPLEMENTED YET");


	ImGui::NewLine();
	ImGui::SeparatorText("Basic Information");
		ImGui::Text("Program: %s", PROGRAM_NAME);
		ImGui::Text("Version: %s", PROGRAM_VERSION);
		ImGui::Text("Date: %s", PROGRAM_DATE);
		ImGui::NewLine();

	ImGui::SeparatorText("System Hardware"); {
		ImGui::Text("System RAM: %dMiB", SDL_GetSystemRAM());
		ImGui::Text("Display Count: %d", SDL_GetNumVideoDisplays());
		ImGui::NewLine();
	}

	ImGui::SeparatorText("CPU Information"); {
		ImGui::Text("CPU Threads: %u", std::thread::hardware_concurrency());
		const Supported_CPU_Instruction& Available_CPU_Instruction = get_Available_CPU_Instruction();

		ImGui::Text("SSE2 Rendering: %s",
			Enable_Text(Available_CPU_Instruction.SSE_Family.SSE2)
		); Item_Tooltip("SSE2 allows the CPU to process 4 32bit floats or 2 64bit floats at a time."\
			"\nSSE2 Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.SSE_Family.SSE2)
		);
		ImGui::Text("AVX Rendering: %s",
			Enable_Text(Available_CPU_Instruction.AVX_Family.AVX)
		); Item_Tooltip("AVX allows the CPU to process 8 32bit floats or 4 64bit floats at a time."\
			"\nAVX Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.AVX_Family.AVX)
		);
		ImGui::Text("AVX512F Rendering: %s",
			Enable_Text(Available_CPU_Instruction.AVX512_Family.AVX512_F)
		); Item_Tooltip("AVX512F allows the CPU to process 16 32bit floats or 8 64bit floats at a time."\
			"\nAVX512F Rendering is %s on your CPU.", Available_Text(Available_CPU_Instruction.AVX512_Family.AVX512_F)
		);
		ImGui::NewLine();

		if (ImGui::CollapsingHeader("CPU Instruction Sets")) {
			ImGui::BeginChild(
				"CPU_InstructionList", ImVec2(0.0f, 108.0f), true
			);
				
			ImGui::Text("CPU Signature: <%s>", Available_CPU_Instruction.CPU_Signature);
			ImGui::NewLine();
			ImGui::Text("SSE Family:"); {
				const Supported_SSE_Family_Instruction& SSE_Family = Available_CPU_Instruction.SSE_Family;
				size_t count = 0;
				if (Available_CPU_Instruction.MMX) {
					ImGui::SameLine(); ImGui::Text("MMX"); count++;
				}
				if (SSE_Family.SSE   ) { ImGui::SameLine(); ImGui::Text("SSE"   ); count++; }
				if (SSE_Family.SSE2  ) { ImGui::SameLine(); ImGui::Text("SSE2"  ); count++; }
				if (SSE_Family.SSE3  ) { ImGui::SameLine(); ImGui::Text("SSE3"  ); count++; }
				if (SSE_Family.SSSE3 ) { ImGui::SameLine(); ImGui::Text("SSSE3" ); count++; }
				if (SSE_Family.SSE4_1) { ImGui::SameLine(); ImGui::Text("SSE4_1"); count++; }
				if (SSE_Family.SSE4_2) { ImGui::SameLine(); ImGui::Text("SSE4_2"); count++; }
				if (SSE_Family.SSE4a ) { ImGui::SameLine(); ImGui::Text("SSE4a" ); count++; }
				if (count == 0) {
					ImGui::SameLine(); ImGui::Text("<None>");
				}
			}
			ImGui::Text("AVX Family:"); {
				const Supported_AVX_Family_Instruction& AVX_Family = Available_CPU_Instruction.AVX_Family;
				size_t count = 0;
				if (AVX_Family.AVX           ) { ImGui::SameLine(); ImGui::Text("AVX"           ); count++; }
				if (AVX_Family.F16C          ) { ImGui::SameLine(); ImGui::Text("F16C"          ); count++; }
				if (AVX_Family.FMA           ) { ImGui::SameLine(); ImGui::Text("FMA"           ); count++; }
				if (AVX_Family.AVX2          ) { ImGui::SameLine(); ImGui::Text("AVX2"          ); count++; }
				if (AVX_Family.AVX_VNNI      ) { ImGui::SameLine(); ImGui::Text("AVX_VNNI"      ); count++; }
				if (AVX_Family.AVX_VNNI_INT8 ) { ImGui::SameLine(); ImGui::Text("AVX_VNNI_INT8" ); count++; }
				if (AVX_Family.AVX_NE_CONVERT) { ImGui::SameLine(); ImGui::Text("AVX_NE_CONVERT"); count++; }
				if (AVX_Family.AVX_IFMA      ) { ImGui::SameLine(); ImGui::Text("AVX_IFMA"      ); count++; }
				if (count == 0) {
					ImGui::SameLine(); ImGui::Text("<None>");
				}
			}
			ImGui::Text("AVX512 Family:"); {
				const Supported_AVX512_Family_Instruction& AVX512_Family = Available_CPU_Instruction.AVX512_Family;
				size_t count = 0;
				if (AVX512_Family.AVX512_F        ) { ImGui::SameLine(); ImGui::Text("AVX512_F"        ); count++; }
				if (AVX512_Family.AVX512_BW       ) { ImGui::SameLine(); ImGui::Text("AVX512_BW"       ); count++; }
				if (AVX512_Family.AVX512_CD       ) { ImGui::SameLine(); ImGui::Text("AVX512_CD"       ); count++; }
				if (AVX512_Family.AVX512_DQ       ) { ImGui::SameLine(); ImGui::Text("AVX512_DQ"       ); count++; }
				if (AVX512_Family.AVX512_IFMA52   ) { ImGui::SameLine(); ImGui::Text("AVX512_IFMA52"   ); count++; }
				if (AVX512_Family.AVX512_VL       ) { ImGui::SameLine(); ImGui::Text("AVX512_VL"       ); count++; }
				if (AVX512_Family.AVX512_VPOPCNTDQ) { ImGui::SameLine(); ImGui::Text("AVX512_VPOPCNTDQ"); count++; }
				if (AVX512_Family.AVX512_BF16     ) { ImGui::SameLine(); ImGui::Text("AVX512_BF16"     ); count++; }
				if (AVX512_Family.AVX512_BITALG   ) { ImGui::SameLine(); ImGui::Text("AVX512_BITALG"   ); count++; }
				if (AVX512_Family.AVX512_VBMI     ) { ImGui::SameLine(); ImGui::Text("AVX512_VBMI"     ); count++; }
				if (AVX512_Family.AVX512_VBMI2    ) { ImGui::SameLine(); ImGui::Text("AVX512_VBMI2"    ); count++; }
				if (AVX512_Family.AVX512_VNNI     ) { ImGui::SameLine(); ImGui::Text("AVX512_VNNI"     ); count++; }
				if (AVX512_Family.AVX512_FP16     ) { ImGui::SameLine(); ImGui::Text("AVX512_FP16"     ); count++; }
				if (count == 0) {
					ImGui::SameLine(); ImGui::Text("<None>");
				}
			}
			ImGui::EndChild();
			ImGui::NewLine();
		}
	}

	ImGui::SeparatorText("GPU Hardware");
		ImGui::Text("OpenCL Enabled: %s", "<Not Implemented>");
		ImGui::Text("GPU Name: %s", "<Not Implemented>");
		ImGui::NewLine();
	ImGui::End();
}
