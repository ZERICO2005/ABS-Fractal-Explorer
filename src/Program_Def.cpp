/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

/* Relative File Path */
	static std::mutex mutex_RelativeFilePath;
	static std::string string_RelativeFilePath = "./";
	std::string get_RelativeFilePath() {
		std::lock_guard<std::mutex> lock(mutex_RelativeFilePath);
		return string_RelativeFilePath;
	}
	void set_RelativeFilePath(const std::string& path) {
		std::lock_guard<std::mutex> lock(mutex_RelativeFilePath);
		string_RelativeFilePath = path;
	}

/* TimerBox */
	/* Constructors */
		TimerBox::TimerBox() {
			updateTime();
			setFreq((nano64_t)0);
			deltaTime = 0;
		}
		TimerBox::TimerBox(nano64_t freq) {
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
		void TimerBox::setTime(nano64_t time) { resetTime = time; }
		void TimerBox::setTime(fp64 time) { resetTime = SECONDS_TO_NANO(time); }
		nano64_t TimerBox::getResetTimeNano() const { return resetTime; }
		fp64 TimerBox::getResetTime() const { return NANO_TO_SECONDS(resetTime); }
	/* deltaTime */
		nano64_t TimerBox::getDeltaTimeNano() const {
			return deltaTime;
		}
		fp64 TimerBox::getDeltaTime() const {
			return NANO_TO_SECONDS(deltaTime);
		}
	/* freqTime */
		void TimerBox::setFreq(nano64_t freq) { freqTime = freq; }
		void TimerBox::setFreq(fp64 freq) { freqTime = SECONDS_TO_NANO(freq); }
		nano64_t TimerBox::getResetFreqNano() const { return freqTime; }
		fp64 TimerBox::getResetFreq() const { return NANO_TO_SECONDS(freqTime); }
	/* timeElapsed */
		nano64_t TimerBox::timeElapsedNano() const {
			return getNanoTime() - resetTime;
		}
		fp64 TimerBox::timeElapsed() const {
			return NANO_TO_SECONDS(getNanoTime() - resetTime);
		}
		nano64_t TimerBox::timeToTimerReadyNano() const {
			return (timeElapsedNano() < freqTime) ? (freqTime - timeElapsedNano()) : 0;
		}
		fp64 TimerBox::timeToTimerReady() const {
			return NANO_TO_SECONDS(timeToTimerReadyNano());
		}
	/* timerReset */
		bool TimerBox::timerReset(nano64_t freq) {
			nano64_t delta = getNanoTime() - resetTime;
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
		bool TimerBox::timerReady(nano64_t freq) const {
			return (getNanoTime() - resetTime >= freq) ? true : false;
		}
		bool TimerBox::timerReady() const { return timerReady(freqTime); }
		bool TimerBox::timerReady(fp64 freq) const { return timerReady(SECONDS_TO_NANO(freq)); }
/* TimerBox */