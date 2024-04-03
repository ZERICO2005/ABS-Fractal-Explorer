/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "framePacer.hpp"

/* class Frame_Pacer */
	/* public */
	/* Constructors */
		Frame_Pacer::Frame_Pacer() {
			init_Frame_Pacer(0,0);
		}

		Frame_Pacer::Frame_Pacer(nano64_t frameTime, nano64_t pollTime) {
			init_Frame_Pacer(frameTime, pollTime);
		}

		Frame_Pacer::Frame_Pacer(fp64 frameRate, fp64 pollRate) {
			init_Frame_Pacer(FRAMERATE_TO_NANO(frameRate), FRAMERATE_TO_NANO(pollRate));
		}
		
	
	/* Variables */
		nano64_t Frame_Pacer::get_FrameTime() const {
			return FrameTime;
		}
		fp64 Frame_Pacer::get_FrameRate() const {
			return NANO_TO_FRAMERATE(FrameTime);
		}
		nano64_t Frame_Pacer::get_FrameDeltaTime() const {
			return Delta_Time;
		}
		fp64 Frame_Pacer::get_FrameDeltaSeconds() const {
			return NANO_TO_SECONDS(Delta_Time);
		}

		void Frame_Pacer::set_FrameTime(nano64_t frameTime) {
			FrameTime = frameTime;
		}
		void Frame_Pacer::set_FrameRate(fp64 frameRate) {
			set_FrameTime(FRAMERATE_TO_NANO(frameRate));
		}
	
	/* Functions */
		// Force starts the next frame
		void Frame_Pacer::forceNewFrame() {
			nano64_t currentTime = getNanoTime();
			Frame_Start = currentTime;
			Next_Frame = Frame_Start + FrameTime;
			
			nano64_t frameTime = Frame_Start - currentTime;
			update_Maximum_FrameTime(frameTime);
		}
		// Checks if the next frame is ready
		bool Frame_Pacer::checkNewFrame() const {
			return (timeUntilNextFrame() <= 0) ? true : false;
		}
		// Checks and starts the next frame
		bool Frame_Pacer::startNewFrame() {
			if (checkNewFrame() == true) {
				forceNewFrame();
				return true;
			}
			return false;
		}

		nano64_t Frame_Pacer::timeUntilNextFrame() const {
			return Next_Frame - getNanoTime();
		}
		fp64 Frame_Pacer::secondsUntilNextFrame() const {
			return NANO_TO_SECONDS(timeUntilNextFrame());
		}

		// Will yeild/sleep until the next frame
		void Frame_Pacer::waitForNextFrame(nano64_t threshold) {
			if (checkNewFrame() == true) { return; }
			while (timeUntilNextFrame() > threshold) {
				std::this_thread::yield();
			}
			while (checkNewFrame() == false) { /* Spin-Lock */ }
			forceNewFrame();
		}
		void Frame_Pacer::waitForNextFrame(fp64 threshold) {
			waitForNextFrame(SECONDS_TO_NANO(threshold));
		}

	/* Statistics */
		nano64_t Frame_Pacer::get_MaximumFrameTime() const {
			return Display_Maximum_FrameTime;
		}
		fp64 Frame_Pacer::get_MaximumFrameRate() const {
			return NANO_TO_FRAMERATE(Display_Maximum_FrameTime);
		}
		
	/* private */
		void Frame_Pacer::init_Frame_Pacer(nano64_t frameTime, nano64_t pollTime) {
			FrameTime = frameTime;
			PollTime = pollTime;

			Frame_Start = getNanoTime();
			Next_Frame = Frame_Start + frameTime;
			Delta_Time = 0;

			Display_Maximum_FrameTime = 0;
			Maximum_FrameTime = 0;
			Maximum_Timer = getNanoTime();
		}
		void Frame_Pacer::update_Maximum_FrameTime(nano64_t frameTime) {
			if (frameTime > Maximum_FrameTime) {
				Maximum_FrameTime = frameTime;
			}
			if (getNanoTime() - Maximum_Timer > PollTime) {
				Maximum_Timer = getNanoTime();
				Display_Maximum_FrameTime = Maximum_FrameTime;
				Maximum_FrameTime = 0;
			}
		}