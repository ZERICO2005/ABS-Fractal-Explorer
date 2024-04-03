/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAME_PACER_HPP
#define FRAME_PACER_HPP

#include "Common_Def.h"

class Frame_Pacer {
	public:
	/* Constructors */
		Frame_Pacer();
		Frame_Pacer(nano64_t frameTime, nano64_t pollTime = SECONDS_TO_NANO(1.0 / 6.0));
		Frame_Pacer(fp64 frameRate, fp64 pollRate = (1.0 / 6.0));
	
	/* Variables */
		nano64_t get_FrameTime() const;
		fp64 get_FrameRate() const;

		nano64_t get_FrameDeltaTime() const;
		fp64 get_FrameDeltaSeconds() const;

		void set_FrameTime(nano64_t frameTime);
		void set_FrameRate(fp64 frameRate);

	/* Functions */
		// Force starts the next frame
		void forceNewFrame();
		// Checks if the next frame is ready
		bool checkNewFrame() const;
		// Checks and starts the next frame
		bool startNewFrame();

		nano64_t timeUntilNextFrame() const;
		fp64 secondsUntilNextFrame() const;
		// Will yeild/sleep until the next frame
		void waitForNextFrame(nano64_t threshold = SECONDS_TO_NANO(1.0 * 1.0e-3));
		// Will yeild/sleep until the next frame
		void waitForNextFrame(fp64 threshold = (1.0 * 1.0e-3));


	/* Statistics */
		nano64_t get_MaximumFrameTime() const;
		fp64 get_MaximumFrameRate() const;
		
	private:
		nano64_t FrameTime;
		nano64_t PollTime;

		nano64_t Frame_Start;
		nano64_t Next_Frame;
		nano64_t Delta_Time;

		nano64_t Display_Maximum_FrameTime;
		nano64_t Maximum_FrameTime;
		nano64_t Maximum_Timer;
		void init_Frame_Pacer(nano64_t frameTime, nano64_t pollTime);
		void update_Maximum_FrameTime(nano64_t frameTime);
};

#endif /* FRAME_PACER_HPP */