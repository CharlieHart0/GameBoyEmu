#pragma once
#include <cstdint>
#include <array>
#include <atomic>

struct MemoryBus {
	std::array<uint8_t, 0xFFFF + 1> memory{};

	//TODO make this work
	std::atomic<bool> wasLastReadOrWriteAllowed = true;

	// 0x0000 - 0x00FF  Boot ROM during boot

	// 0x0000 - 0x3FFF  Game ROM Bank 0

	// 0x4000 - 0x7FFF  Game ROM Bank N

	// 0x8000 - 0x97FF  TILE RAM

	// 0x9800 - 0x9FFF  Background Map

	// 0xA000 - 0xBFFF  Cart RAM

	// 0xC000 - 0xDFFF  Working RAM / Conventional RAM

	// 0xE000 - 0xFDFF  Echo RAM (discouraged from using)

	// 0xFE00 - 0xFE9F  Object Attribute Memory

	// 0xFEA0 - 0xFEFF  Unused

	// 0xFF00 - 0xFF7F  I/O Registers

	// 0xFF80 - 0xFFFE  High RAM Area

	// 0xFFFF  Interrupt Enabled Register
};

uint8_t MemoryBus_read_byte(MemoryBus& memoryBus, uint16_t address);

void* MemoryBus_get_ptr(MemoryBus& memoryBus, uint16_t address);

void MemoryBus_write_byte(MemoryBus& memoryBus, uint16_t address, uint8_t byte);

void MemoryBus_read_from_file(MemoryBus& memoryBus, const char* filepath);