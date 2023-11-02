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

int setup_fracExp(int argc, char* argv[]);

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Key_Function_Mutex);

int terminate_Engine();

/* Enum of Functions that can be activated by a KeyboardScancode */
namespace Key_Function {
	const char* const Key_Function_Text[] = {"NONE","incRealPos","decRealPos","resetRealPos","incImagPos","decImagPos","resetImagPos","incZoom","decZoom","resetZoom","resetCoordinates","COORDINATES","clockwiseRot","counterclockwiseRot","resetRotation","clockwiseRot90","rotate180","counterclockwiseRot90","flipHorizontal","flipVertical","resetFlip","clockwiseRotStep","counterclockwiseRotStep","clockwiseRotPower","counterclockwiseRotPower","incStretch","decStretch","resetStretch","resetTransformations","TRANSFORMATIONS","toggleJulia","toggleStartingZ","toggleCursorZValue","toggleRelativeZValue","incZReal","decZReal","resetZReal","incZImag","decZImag","resetZImag","JULIA","incMaxItr","decMaxItr","resetMaxItr","toggleAutoItr","incBreakout","decBreakout","resetBreakout","toggleAdjustZoomToPower","PARAMETERS","toggleABSandPolarMandelbrot","roundPower","floorPower","ceilingPower","toggleIntegerPower","lockToCardioid","flipCardioidSide","POLAR","inputFormula","resetFormula","incFormula","decFormula","incFamily","decFamily","inputPower","resetPower","incPower","decPower","FORMULA","toggleScreenSplit","nextScreenSplit","prevScreenSplit","clockwiseCornerSplit","counterClockwiseCornerSplit","toggleFloatingWindow","swapPrimaryAndSecondary","SCREEN_SPLIT","openFractalMenu","exportFractal","importFractal","takeScreenshot","takeSuperScreenshot","openRenderingMenu","openSettingsMenu","openKeyBindsMenu","abortRendering","exitApplication","toggleMenuBar","toggleCordinateBar","toggleButtonBar","FUNCTIONS","switchCPUandGPU","fp16GpuRendering","fp32GpuRendering","fp64GpuRendering","fp32CpuRendering","fp64CpuRendering","fp80CpuRendering","fp128CpuRendering","incSuperSample","decSuperSample","resetSuperSample","incSubSample","decSubSample","resetSubSample","RENDERING","Parameter_Function_Count"};
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
			toggleMenuBar,toggleCordinateBar,toggleButtonBar,
			FUNCTIONS,
		/* Rendering */
			switchCPUandGPU,
			fp16GpuRendering,fp32GpuRendering,fp64GpuRendering,
			fp32CpuRendering,fp64CpuRendering,fp80CpuRendering,fp128CpuRendering,
			incSuperSample,decSuperSample,resetSuperSample,
			incSubSample,decSubSample,resetSubSample,
			RENDERING,
		Parameter_Function_Count
	};
	const size_t Key_Function_Map[] = {NONE,COORDINATES,TRANSFORMATIONS,JULIA,PARAMETERS,POLAR,FORMULA,SCREEN_SPLIT,FUNCTIONS,RENDERING};
};
/* String of Functions that can be activated by a KeyboardScancode */
//const char*

#endif /* ENGINE_H */