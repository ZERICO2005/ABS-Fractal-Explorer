/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "render.h"

int main() {
	initLCDcontroller();
	renderLoop();
	terminateLCDcontroller();
	return 0;
}