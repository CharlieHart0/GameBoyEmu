#include "MemoryBus.h"

uint8_t MemoryBus_read_byte(MemoryBus& memoryBus, uint16_t address) {
	return memoryBus.memory[address];
}

void* MemoryBus_get_ptr(MemoryBus& memoryBus, uint16_t address) {
	return &memoryBus.memory[address];
}

// This might need to be a little more complex but itll do for now i guess
void MemoryBus_write_byte(MemoryBus& memoryBus, uint16_t address, uint8_t byte) {
	memoryBus.memory[address] = byte;
}