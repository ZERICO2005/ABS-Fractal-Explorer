#include "gamma.h"
#include "render.h"

int main() {
	initLCDcontroller();
	renderLoop();
	terminateLCDcontroller();
	return 0;
}