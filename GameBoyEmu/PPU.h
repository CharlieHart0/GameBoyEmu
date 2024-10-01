#pragma once
#include <cstdint>
#include <chrono>
#include "CPU.h"


// the PPU (pixel processing unit) is in charge of rendering to the display of the gameboy.

//TODO put these in their own header with the other addresses from other files to avoid repetition
#define GB_PPU_STAT_ADDR 0xFF41
#define GB_PPU_LY_ADDR 0xFF44

extern CPU cpu;

namespace gbppu
{
	// used to pause thread excecution until a certain amount of time has passed, in dots (238.435 nanoseconds)
	class PPUDotTimer
	{
	protected:
		//A “dot” = one 222 Hz (≅ 4.194 MHz) time unit
		const std::chrono::duration<double, std::nano> dot{238.435};
		std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
		std::chrono::duration<double, std::nano> minimumDurationRequired{0};

	public:
		void Start(unsigned int numDots);
		// pauses thread excecution until minimumDurationRequired has passed from startTimePoint
		void Wait();

	};
	

	class PPU
	{
	protected:
		// https://gbdev.io/pandocs/Rendering.html#ppu-modes
		
		PPUDotTimer dotTimer;


		// 0xFF41 .1 and .0
		// 0 Horizontal Blank  1 Vertical Blank  2 OAM Scan  3 Drawing Pixels
		uint8_t getPPUMode();
		void setPPUMode(uint8_t t_mode);

		uint8_t getLY();
		// increments LY, sets to 0 if 153 or more. Does not touch PPU mode.
		void incrementLY();

		

		
	public:
		// completes one 456 dot line, either (OAM scan, draw, hblank) OR vblank.
		void line();
		void Start();
	};
}


