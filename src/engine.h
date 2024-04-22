/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "Common_Def.h"
#include "Program_Def.h"

int setup_fracExp(int argc, char* argv[]);

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::atomic<bool>& ABORT_RENDERING);

int terminate_Engine();

void get_GPU_Hardware_Hash(uint64_t& hash);

namespace Rendering_Configuration {
	enum Rendering_Precision {
		Precision_Automatic,
		Precision_Float16,
		Precision_Float32,
		Precision_Float64,
		Precision_Float80,
		Precision_Float128
	};
	const char* const Rendering_Precision_Text[] = {
		"Automatic Precision",
		"(10^1.8) 16bit Float",
		"(10^5.7) 32bit Float",
		"(10^14.4) 64bit Float",
		"(10^17.7) 80bit Float",
		"(10^32.5) 128bit Float"
	};
	enum Rendering_Method {
		Render_Method_Automatic,
		Render_Method_GPU,
		Render_Method_CPU_Generic,
		Render_Method_CPU_SSE2,
		Render_Method_CPU_AVX,
		Render_Method_CPU_AVX512
	};
	const char* const Rendering_Method_Text[] = {
		"Automatic Render Method",
		"GPU Rendering",
		"CPU-Generic Rendering",
		"CPU-SSE2 Rendering",
		"CPU-AVX Rendering",
		"CPU-AVX512 Rendering"
	};
	enum Rendering_Preset {
		Render_Preset_Automatic,
		/* GPU Rendering */
		Render_Preset_GPU_Float16,
		Render_Preset_GPU_Float32,
		Render_Preset_GPU_Float64,
		/* CPU Generic Rendering */
		Render_Preset_CPU_Generic_Float16,
		Render_Preset_CPU_Generic_Float32,
		Render_Preset_CPU_Generic_Float64,
		Render_Preset_CPU_Generic_Float80,
		Render_Preset_CPU_Generic_Float128,
		/* CPU SSE2 Rendering */
		Render_Preset_CPU_SSE2_Float32,
		Render_Preset_CPU_SSE2_Float64,
		/* CPU AVX Rendering */
		Render_Preset_CPU_AVX_Float32,
		Render_Preset_CPU_AVX_Float64,
		/* CPU AVX512 Rendering */
		Render_Preset_CPU_AVX512_Float16,
		Render_Preset_CPU_AVX512_Float32,
		Render_Preset_CPU_AVX512_Float64
	};
	const char* const Rendering_Preset_Text[] = {
		"Automatic Render Preset",
		/* GPU Rendering */
		"(10^1.8) GPU 16bit Float",
		"(10^5.7) GPU 32bit Float",
		"(10^14.4) GPU 64bit Float",
		/* CPU Generic Rendering */
		"(10^1.8) CPU-Generic 16bit Float",
		"(10^5.7) CPU-Generic 32bit Float",
		"(10^14.4) CPU-Generic 64bit Float",
		"(10^17.7) CPU-Generic 80bit Float",
		"(10^32.5) CPU-Generic 128bit Float",
		/* CPU SSE2 Rendering */
		"(10^5.7) CPU-SSE2 32bit Float",
		"(10^14.4) CPU-SSE2 64bit Float",
		/* CPU AVX Rendering */
		"(10^5.7) CPU-AVX 32bit Float",
		"(10^14.4) CPU-AVX 64bit Float",
		/* CPU AVX512 Rendering */
		"(10^1.8) CPU-AVX512 16bit Float",
		"(10^5.7) CPU-AVX512 32bit Float",
		"(10^14.4) CPU-AVX512 64bit Float"
	};
};

// Returns the closest valid rendering method supported on the hardware
Rendering_Configuration::Rendering_Preset validate_Rendering_Preset(
	Rendering_Configuration::Rendering_Preset render_preset
);

/* Enum of Functions that can be activated by a KeyboardScancode */
namespace Key_Function {
	const char* const Key_Function_Text[] = {
		"NONE",
		"incRealPos","decRealPos","resetRealPos","incImagPos","decImagPos","resetImagPos","incZoom","decZoom","resetZoom","resetCoordinates",
		"COORDINATES",
		"clockwiseRot","counterclockwiseRot","resetRotation","clockwiseRot90","rotate180","counterclockwiseRot90","flipHorizontal","flipVertical","resetFlip","clockwiseRotStep","counterclockwiseRotStep","clockwiseRotPower","counterclockwiseRotPower","incStretch","decStretch","resetStretch","resetTransformations",
		"TRANSFORMATIONS",
		"toggleJulia","toggleStartingZ","toggleCursorZValue","toggleRelativeZValue","incZReal","decZReal","resetZReal","incZImag","decZImag","resetZImag",
		"JULIA",
		"incMaxItr","decMaxItr","resetMaxItr","toggleAutoItr","incBreakout","decBreakout","resetBreakout","toggleAdjustZoomToPower",
		"PARAMETERS",
		"toggleABSandPolarMandelbrot","roundPower","floorPower","ceilingPower","toggleIntegerPower","lockToCardioid","flipCardioidSide",
		"POLAR",
		"inputFormula","resetFormula","incFormula","decFormula","incFamily","decFamily","inputPower","resetPower","incPower","decPower",
		"FORMULA",
		"toggleScreenSplit","nextScreenSplit","prevScreenSplit","clockwiseCornerSplit","counterClockwiseCornerSplit","toggleFloatingWindow","swapPrimaryAndSecondary",
		"SCREEN_SPLIT",
		"openFractalMenu","exportFractal","importFractal","takeScreenshot","takeSuperScreenshot","openRenderingMenu","openSettingsMenu","openKeyBindsMenu","abortRendering","exitApplication","toggleFullscreen","toggleMenuBar","toggleCordinateBar","toggleButtonBar",
		"FUNCTIONS",
		"refreshFractal","switchCPUandGPU","fp16GpuRendering","fp32GpuRendering","fp64GpuRendering","fp32CpuRendering","fp64CpuRendering","fp80CpuRendering","fp128CpuRendering","incSuperSample","decSuperSample","resetSuperSample","incSubSample","decSubSample","resetSubSample",
		"RENDERING",
		"Parameter_Function_Count"
	};
	enum Key_Function_Enum {
		NONE,
		/* Cordinates */
			incRealPos,decRealPos,resetRealPos,
			incImagPos,decImagPos,resetImagPos,
			incZoom,decZoom,resetZoom,
			resetCoordinates,
			COORDINATES,
		/* Transformations */
			clockwiseRot,counterclockwiseRot,resetRotation,
			clockwiseRot90,rotate180,counterclockwiseRot90,
			flipHorizontal,flipVertical,resetFlip,
			clockwiseRotStep,counterclockwiseRotStep,
			clockwiseRotPower,counterclockwiseRotPower,
			incStretch,decStretch,resetStretch,
			resetTransformations,
			TRANSFORMATIONS,
		/* Julia */
			toggleJulia,
			toggleStartingZ,
			toggleCursorZValue,
			toggleRelativeZValue,
			incZReal,decZReal,resetZReal,
			incZImag,decZImag,resetZImag,
			JULIA,
		/* Parameters */
			incMaxItr,decMaxItr,resetMaxItr,
			toggleAutoItr,
			incBreakout,decBreakout,resetBreakout,
			toggleAdjustZoomToPower,
			PARAMETERS,
		/* Polar */
			toggleABSandPolarMandelbrot,
			roundPower,floorPower,ceilingPower,
			toggleIntegerPower,
			lockToCardioid,
			flipCardioidSide,
			POLAR,
		/* Formula */
			inputFormula,resetFormula,
			incFormula,decFormula,
			incFamily,decFamily,
			inputPower,resetPower,
			incPower,decPower,
			FORMULA,
		/* Screen Split */
			toggleScreenSplit,nextScreenSplit,prevScreenSplit,
			clockwiseCornerSplit,counterClockwiseCornerSplit,
			toggleFloatingWindow,
			swapPrimaryAndSecondary,
			SCREEN_SPLIT,
		/* Functions */
			openFractalMenu,
			exportFractal,
			importFractal,
			takeScreenshot,
			takeSuperScreenshot,
			openRenderingMenu,
			openSettingsMenu,
			openKeyBindsMenu,
			abortRendering,
			exitApplication,
			toggleFullscreen,
			toggleMenuBar,toggleCordinateBar,toggleButtonBar,
			FUNCTIONS,
		/* Rendering */
			refreshFractal,
			switchCPUandGPU,
			fp16GpuRendering,fp32GpuRendering,fp64GpuRendering,
			fp32CpuRendering,fp64CpuRendering,fp80CpuRendering,fp128CpuRendering,
			incSuperSample,decSuperSample,resetSuperSample,
			incSubSample,decSubSample,resetSubSample,
			RENDERING,
		Parameter_Function_Count
	};
	const size_t Key_Function_Map[] = {
		NONE,COORDINATES,TRANSFORMATIONS,JULIA,PARAMETERS,POLAR,FORMULA,SCREEN_SPLIT,FUNCTIONS,RENDERING
	};

	const char* const Key_Function_Category_Text[] = {
		"NONE","COORDINATES","TRANSFORMATIONS","JULIA","PARAMETERS","POLAR","FORMULA","SCREEN_SPLIT","FUNCTIONS","RENDERING"
	};
	enum Key_Function_Category {
		Category_NONE,Category_COORDINATES,Category_TRANSFORMATIONS,Category_JULIA,Category_PARAMETERS,Category_POLAR,Category_FORMULA,Category_SCREEN_SPLIT,Category_FUNCTIONS,Category_RENDERING
	};
};
/* String of Functions that can be activated by a KeyboardScancode */
//const char*

#endif /* ENGINE_H */