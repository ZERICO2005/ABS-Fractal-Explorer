
#ifndef RENDER_H
#define RENDER_H

#include "gamma.h"

void renderFractal(frac* parameter);
void initLCDcontroller();
void terminateLCDcontroller();
void scanKeyboard();
uint8_t keyPressed(uint32_t key);

void renderLoop();

uint8_t status();

//Time Stuff
struct _Timer {
	uint64_t cycle;
	uint64_t reset;
}; typedef struct _Timer Timer;

uint8_t timerReady(Timer* timer);

void setTimer(Timer* timer, fp64 cycle); //Time in Seconds

fp64 readTimer(Timer* timer); //Time elasped in Seconds

#endif /* RENDER_H */