#include "MemoryBus.h"
#include <fstream>
#include <imgui.h>

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

void MemoryBus_read_from_file(MemoryBus& memoryBus, const char* filepath)
{
    std::ifstream inputROM(filepath, std::ios::binary);
    inputROM.seekg(0, std::ios::end);
    std::streampos fileSize = inputROM.tellg();
    IM_ASSERT(fileSize != -1);
    inputROM.seekg(0, std::ios::beg);
    if (fileSize > 0xFFFF) { fileSize = 0xFFFF; }
    inputROM.read((char*) &(memoryBus.memory), fileSize);
}
