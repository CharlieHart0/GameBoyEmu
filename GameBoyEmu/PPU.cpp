#include "PPU.h"
#include "Helper/binary.h"

using namespace chhelper::binary;

// TODO mutex with cpu
// 0xFF41 .1 and .0
uint8_t gbppu::PPU::getPPUMode()
{
	return MemoryBus_read_byte(cpu.bus, GB_PPU_STAT_ADDR,MemoryBusAccessSource::PPU) & 0b11;
}

// TODO mutex with cpu
void gbppu::PPU::setPPUMode(uint8_t t_mode)
{
	t_mode &= 0b11; // ignore any bits apart from bit 0 and 1
	uint8_t statValue = MemoryBus_read_byte(cpu.bus, GB_PPU_STAT_ADDR, MemoryBusAccessSource::PPU);
	// assign bits 0 and 1 of stat to bits 0 and 1 of mode
	for(int pos = 0; pos <= 1; pos++) assignBit<uint8_t>(statValue, pos, getBit<uint8_t>(t_mode, pos));
	MemoryBus_write_byte(cpu.bus, GB_PPU_STAT_ADDR, statValue, MemoryBusAccessSource::PPU);
}

uint8_t gbppu::PPU::getLY()
{
	return MemoryBus_read_byte(cpu.bus,GB_PPU_LY_ADDR, MemoryBusAccessSource::PPU);
}

void gbppu::PPU::incrementLY()
{
	uint8_t LY = getLY();
	if (LY >= 153) LY = 0; // should never be more than 153 though
	else LY++;
	MemoryBus_write_byte(cpu.bus, GB_PPU_LY_ADDR, LY, MemoryBusAccessSource::PPU);
}

// example blank test stuff here, just example timings and stuff
void gbppu::PPU::line()
{
	if (getLY() >= 144) // if in VBLANK
	{
		setPPUMode(1);
		dotTimer.Start(456);
		dotTimer.Wait();
	}
	else
	{
		setPPUMode(2);
		dotTimer.Start(80);
		//OAM Scan
		dotTimer.Wait();

		setPPUMode(3);
		dotTimer.Start(289);
		//drawing pixels
		dotTimer.Wait();

		setPPUMode(0);
		dotTimer.Start(87);
		// H blank
		dotTimer.Wait();
	}

	incrementLY();
}

void gbppu::PPU::Start()
{
	// unused yet
}

void gbppu::PPUDotTimer::Start(unsigned int numDots)
{
	minimumDurationRequired = dot * numDots;
}


void gbppu::PPUDotTimer::Wait()
{
	while ((std::chrono::steady_clock::now() - startTimePoint) < minimumDurationRequired)
	{
		//wait
	}
}
