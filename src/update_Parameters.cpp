#include "Common_Def.h"
#include "Program_Def.h"
#include "render.h"
#include "temp_global_render.h"

#include "fractal.h"
#include "engine.h"

#include <SDL2/SDL.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "programData.h"
#include "user_data.h"

#include "menu_Interface/display_GUI.h"
#include "displayInfo.h"

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

template<typename fpX, typename fpA, typename fpB>
static void moveCord(fpX* x, fpX* y, fpA angle, fpB speed) {
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

int_enum get_ABS_Mandelbrot_Update_Level(
    ABS_Mandelbrot* frac_data,
    Render_Data* ren,
    int_enum update_level = Change_Level::Nothing
) {
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

int_enum updateFractalParameters(
    ABS_Mandelbrot& FRAC,
    bool& force_quit_flag
) {
	using namespace Key_Function;
	using namespace Change_Level;

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
		paramToggleUpdate(togglePolarMandelbrot, FRAC.polarMandelbrot, 0.4, Major_Reset);
		paramToggle(toggleRelativeZValue, FRAC.relativeZValue, 0.4);
		paramToggle(toggleCursorZValue, FRAC.cursorZValue, 0.4);
		paramToggleUpdate(toggleStartingZ, FRAC.startingZ, 0.4, Minor_Reset);
		paramToggleUpdate(toggleIntegerPower, FRAC.integerPolarPower, 0.4, Minor_Reset);

		paramToggle(lockToCardioid, FRAC.lockToCardioid, 0.4);
		paramToggle(flipCardioidSide, FRAC.flipCardioidSide, 0.4);
		
	/* Real and Imaginary Coordinates */
		if (func_stat[incRealPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.0 * TAU + FRAC.rot,
				0.72 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.coordinate
			);
		}
		if (func_stat[decRealPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.5 * TAU + FRAC.rot,
				0.72 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.coordinate
			);
		}
		if (func_stat[incImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.25 * TAU + FRAC.rot,
				0.72 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.coordinate
			);
		}
		if (func_stat[decImagPos].triggered == true) {
			moveCord(
				&FRAC.r, &FRAC.i, 0.75 * TAU + FRAC.rot,
				0.72 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.coordinate
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
				0.24 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.julia
			);
		}
		if (func_stat[decZReal].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.5 * TAU + FRAC.rot,
				0.24 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sX * config_sensitivity.julia
			);
		}
		if (func_stat[incZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.25 * TAU + FRAC.rot,
				0.24 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.julia
			);
		}
		if (func_stat[decZImag].triggered == true) {
			moveCord(
				&FRAC.zr, &FRAC.zi, 0.75 * TAU + FRAC.rot,
				0.24 * pow(10.0, -FRAC.zoom) * moveDelta * FRAC.sY * config_sensitivity.julia
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
				ABS_Mandelbrot temp_FRAC = FRAC;
				temp_FRAC.zoom = -log10(getABSFractalMaxRadius(FRAC.polarMandelbrot ? FRAC.polarPower : (fp64)FRAC.power));
				pixel_to_coordinate(
					(int32_t)(ImGui::GetMousePos().x), (int32_t)ImGui::GetMousePos().y - (int32_t)RESY_UI,
					FRAC.zr, FRAC.zi, temp_FRAC, primaryRenderData.resX, primaryRenderData.resY
				);
				FRAC.zr -= FRAC.r;
				FRAC.zi -= FRAC.i;
			} else {
				pixel_to_coordinate(
					(int32_t)(ImGui::GetMousePos().x), (int32_t)ImGui::GetMousePos().y - (int32_t)RESY_UI,
					FRAC.zr, FRAC.zi, FRAC, primaryRenderData.resX, primaryRenderData.resY
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
		if (funcTimeDelay(clockwiseRot15, 1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRot15, 1.0/10.0)) {
			FRAC.rot -= (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(clockwiseRot5, 1.0/10.0)) {
			FRAC.rot += (TAU * (15.0/360.0));
		}
		if (funcTimeDelay(counterclockwiseRot5, 1.0/10.0)) {
			FRAC.rot -= (TAU * (15.0/360.0));
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
		if (funcTimeDelay(reverseStretch, 0.4)) {
			FRAC.stretch *= -1.0;
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
			if (Render_Config.suggest_Render_Precision_and_Fastest_CPU_Method(Render_Precision_Float32)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp64CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision_and_Fastest_CPU_Method(Render_Precision_Float64)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp80CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision_and_Fastest_CPU_Method(Render_Precision_Float80)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		if (funcTimeDelay(fp128CpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Precision_and_Fastest_CPU_Method(Render_Precision_Float128)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		// if (funcTimeDelay(fp16GpuRendering, 0.2)) {
		// 	if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float16)) {
		// 		write_Update_Level(Change_Level::Method_of_Rendering);
		// 	}
		// }
		if (funcTimeDelay(fp32GpuRendering, 0.2)) {
			if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float32)) {
				write_Update_Level(Change_Level::Method_of_Rendering);
			}
		}
		// if (funcTimeDelay(fp64GpuRendering, 0.2)) {
		// 	if (Render_Config.suggest_Render_Preset(Render_Preset_GPU_Float64)) {
		// 		write_Update_Level(Change_Level::Method_of_Rendering);
		// 	}
		// }
		
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
		if (funcTimeDelay(closeMenu,0.2)) {
			buttonSelection = -1;
		}
		if (funcTimeDelay(inputFormula,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Coordinates);
		}
		if (funcTimeDelay(inputPower,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
		}
		if (funcTimeDelay(openCordinateMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Coordinates);
		}
		if (funcTimeDelay(openFractalMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Fractal);
		}
		if (funcTimeDelay(openRenderingMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Rendering);
		}
		if (funcTimeDelay(openSettingsMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_Settings);
		}
		if (funcTimeDelay(openKeyBindsMenu,0.4)) {
			GUI_MENU_TOGGLE(GUI_Menu_KeyBinds);
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
		if (funcTimeDelay(abortRendering,0.2)) {
			Abort_Rendering_Flag = true;
			Waiting_To_Abort_Rendering = true;
			write_Abort_Render_Ongoing(true);
			abortTimer = getNanoTime();
		}
		if (funcTimeDelay(exitApplication,0.4)) {
			force_quit_flag = true;
		}
	write_Update_Level(update_level);
	return update_level;
}