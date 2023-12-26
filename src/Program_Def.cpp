/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

/* FP128 functions */
#ifdef enableFP80andFP128
	fp128 sin(fp128 x) { return sinq(x); }
	fp128 cos(fp128 x) { return cosq(x); }
	fp128 tan(fp128 x) { return tanq(x); }
	fp128 asin(fp128 x) { return asinq(x); }
	fp128 acos(fp128 x) { return acosq(x); }
	fp128 atan(fp128 x) { return atanq(x); }
	fp128 atan2(fp128 y, fp128 x) { return atan2q(y,x); }
	fp128 log(fp128 x) { return logq(x); }
	fp128 log2(fp128 x) { return log2q(x); }
	fp128 log10(fp128 x) { return log10q(x); }
	fp128 pow(fp128 x, fp128 y) { return powq(x,y); }
	fp128 floor(fp128 x) { return floorq(x); }
	fp128 ceil(fp128 x) { return ceilq(x); }
	fp128 fmax(fp128 x, fp128 y) { return fmaxq(x,y); }
	fp128 fmin(fp128 x, fp128 y) { return fminq(x,y); }
	fp128 fmod(fp128 x, fp128 y) { return fmodq(x,y); }
	fp128 fabs(fp128 x) { return fabsq(x); }
	fp128 copysign(fp128 x, fp128 y) { return copysignq(x,y); }
#endif

/* TimerBox */
	/* Constructors */
		TimerBox::TimerBox() {
			updateTime();
			setFreq((uint64_t)0);
			deltaTime = 0;
		}
		TimerBox::TimerBox(uint64_t freq) {
			updateTime();
			setFreq(freq);
			deltaTime = 0;
		}
		TimerBox::TimerBox(fp64 freq) {
			updateTime();
			setFreq(freq);
			deltaTime = 0;
		}
	/* resetTime */
		void TimerBox::updateTime() { resetTime = getNanoTime(); }
		void TimerBox::setTime(uint64_t time) { resetTime = time; }
		void TimerBox::setTime(fp64 time) { resetTime = SECONDS_TO_NANO(time); }
		uint64_t TimerBox::getResetTimeNano() { return resetTime; }
		fp64 TimerBox::getResetTime() { return NANO_TO_SECONDS(resetTime); }
	/* deltaTime */
		uint64_t TimerBox::getDeltaTimeNano() {
			return deltaTime;
		}
		fp64 TimerBox::getDeltaTime() {
			return NANO_TO_SECONDS(deltaTime);
		}
	/* freqTime */
		void TimerBox::setFreq(uint64_t freq) { freqTime = freq; }
		void TimerBox::setFreq(fp64 freq) { freqTime = SECONDS_TO_NANO(freq); }
		uint64_t TimerBox::getResetFreqNano() { return freqTime; }
		fp64 TimerBox::getResetFreq() { return NANO_TO_SECONDS(freqTime); }
	/* timeElapsed */
		uint64_t TimerBox::timeElapsedNano() {
			return getNanoTime() - resetTime;
		}
		fp64 TimerBox::timeElapsed() {
			return NANO_TO_SECONDS(getNanoTime() - resetTime);
		}
		uint64_t TimerBox::timeToTimerReadyNano() {
			return (timeElapsedNano() < freqTime) ? (freqTime - timeElapsedNano()) : 0;
		}
		fp64 TimerBox::timeToTimerReady() {
			return NANO_TO_SECONDS(timeToTimerReadyNano());
		}
	/* timerReset */
		bool TimerBox::timerReset(uint64_t freq) {
			uint64_t delta = getNanoTime() - resetTime;
			if (delta >= freq) {
				deltaTime = delta;
				resetTime = getNanoTime();
				return true;
			}
			return false;
		}
		bool TimerBox::timerReset() { return timerReset(freqTime); }
		bool TimerBox::timerReset(fp64 freq) { return timerReset(SECONDS_TO_NANO(freq)); }
	/* timerReady */
		bool TimerBox::timerReady(uint64_t freq) {
			if (getNanoTime() - resetTime >= freq) { return true; }
			return false;
		}
		bool TimerBox::timerReady() { return timerReady(freqTime); }
		bool TimerBox::timerReady(fp64 freq) { return timerReady(SECONDS_TO_NANO(freq)); }
/* TimerBox */