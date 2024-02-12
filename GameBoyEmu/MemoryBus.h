#pragma once
#include <cstdint>

struct MemoryBus {
	uint8_t memory[0xFFFF]{};
};

uint8_t MemoryBus_read_byte(MemoryBus& memoryBus, uint16_t address);

void* MemoryBus_get_ptr(MemoryBus& memoryBus, uint16_t address);

void MemoryBus_write_byte(MemoryBus& memoryBus, uint16_t address, uint8_t byte);