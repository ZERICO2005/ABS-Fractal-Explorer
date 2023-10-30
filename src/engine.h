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

int init_Engine(std::atomic<bool>& QUIT_FLAG, std::mutex& Console_Mutex);

int terminate_Engine();

/* Enum of Functions that can be activated by a KeyboardScancode */
namespace Key_Function {
	enum Key_Function_Enum {
		none,
		/* Cordinates */
			incRealPos,decRealPos,resetRealPos,
			incImagPos,decImagPos,resetImagPos,
			incZoom,decZoom,resetZoom,
			resetCoordinates,
		/* Transformations */
			clockwiseRot,counterclockwiseRot,resetRotation,
			clockwiseRot90,rotate180,counterclockwiseRot90,
			flipHorizontal,flipVertical,resetFlip,
			clockwiseRotStep,counterclockwiseRotStep,
			clockwiseRotPower,counterclockwiseRotPower,
			incStretch,decStretch,resetStretch,
			resetTransformations,
		/* Julia */
			toggleJulia,
			toggleStartingZ,
			toggleCursorZValue,
			toggleRelativeZValue,
			incZReal,decZReal,resetZReal,
			incZImag,decZImag,resetZImag,
		/* Parameters */
			incMaxItr,decMaxItr,resetMaxItr,
			toggleAutoItr,
			incBreakout,decBreakout,resetBreakout,
			toggleAdjustZoomToPower,
		/* Polar */
			toggleABSandPolarMandelbrot,
			roundPower,floorPower,ceilingPower,
			toggleIntegerPower,
			lockToCardioid,
			flipCardioidSide,
		/* Formula */
			inputFormula,resetFormula,
			incFormula,decFormula,
			incFamily,decFamily,
			inputPower,resetPower,
			incPower,decPower,
		/* Screen Split */
			toggleScreenSplit,nextScreenSplit,prevScreenSplit,
			clockwiseCornerSplit,counterClockwiseCornerSplit,
			toggleFloatingWindow,
			swapPrimaryAndSecondary,
		/* Functions */
			openFractalMenu,
			exportFractal,
			importFractal,
			takeScreenshot,
			takeSuperScreenshot,
			openRenderingMenu,
			openSettingsMenu,
			openKeybindsMenu,
			abortRendering,
			exitApplication,
			toggleMenuBar,toggleCordinateBar,toggleButtonBar,
		/* Rendering */
			switchCPUandGPU,
			fp16GpuRendering,fp32GpuRendering,fp64GpuRendering,
			fp32CpuRendering,fp64CpuRendering,fp80CpuRendering,fp128CpuRendering,
			incSuperSample,decSuperSample,resetSuperSample,
			incSubSample,decSubSample,resetSubSample,
		Parameter_Function_Count
	};
};
/* String of Functions that can be activated by a KeyboardScancode */
//const char*

#endif /* ENGINE_H */