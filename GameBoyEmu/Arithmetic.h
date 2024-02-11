#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


typedef struct {
	bool didCarry;
	uint8_t wrappedValue;
} arithmeticResultu8;

typedef struct  {
	bool didCarry;
	uint16_t wrappedValue;
} arithmeticResultu16;

#pragma region add_operations

arithmeticResultu8 add_u8(uint8_t value1, uint8_t value2);

arithmeticResultu8 add_u8_3vals(uint8_t value1, uint8_t value2, uint8_t value3);

arithmeticResultu16 add_u16(uint16_t value1, uint16_t value2);

#pragma endregion


#pragma region sub_operations

arithmeticResultu8 sub_u8(uint8_t value1, uint8_t value2);

arithmeticResultu8 sub_u8_3vals(uint8_t value1, uint8_t value2, uint8_t value3);

arithmeticResultu16 sub_u16(uint16_t value1, uint16_t value2);

#pragma endregion


#ifdef __cplusplus
}
#endif

