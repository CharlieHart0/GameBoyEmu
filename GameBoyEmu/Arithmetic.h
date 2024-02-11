#pragma once
#include <cstdint>

struct arithmeticResultu8 {
	bool didCarry;
	uint8_t wrappedValue;
};

struct arithmeticResultu16 {
	bool didCarry;
	uint16_t wrappedValue;
};

#pragma region add_operations
arithmeticResultu8 overflowing_add_u8(uint8_t value1, uint8_t value2);

arithmeticResultu8 overflowing_add_u8(uint8_t value1, uint8_t value2, uint8_t value3);

arithmeticResultu16 overflowing_add_u16(uint16_t value1, uint16_t value2);
#pragma endregion

#pragma region sub_operations
arithmeticResultu8 underflowing_sub_u8(uint8_t value1, uint8_t value2);

arithmeticResultu8 underflowing_sub_u8(uint8_t value1, uint8_t value2, uint8_t value3);

arithmeticResultu16 underflowing_sub_u16(uint16_t value1, uint16_t value2);
#pragma endregion

