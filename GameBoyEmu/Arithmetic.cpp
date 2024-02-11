#include "Arithmetic.h"

arithmeticResultu8 overflowing_add_u8(uint8_t value1, uint8_t value2)
{
	uint16_t bigint = value1 + value2;

	arithmeticResultu8 result;
	result.didCarry = bigint > UINT8_MAX;

	if (result.didCarry) {
		bigint -= UINT8_MAX;
	}
	result.wrappedValue = (uint8_t)bigint;
	
	return result;
}

arithmeticResultu8 overflowing_add_u8(uint8_t value1, uint8_t value2, uint8_t value3)
{
	uint32_t bigint = value1 + value2 + value3;

	arithmeticResultu8 result;
	result.didCarry = bigint > UINT8_MAX;

	while(bigint > UINT8_MAX) {
		bigint -= UINT8_MAX;
	}
	result.wrappedValue = (uint8_t)bigint;

	return result;
}

arithmeticResultu16 overflowing_add_u16(uint16_t value1, uint16_t value2)
{
	uint32_t bigint = value1 + value2;

	arithmeticResultu16 result;
	result.didCarry = bigint > UINT16_MAX;

	if (result.didCarry) {
		bigint -= UINT16_MAX;
	}
	result.wrappedValue = (uint16_t)bigint;

	return result;
}

arithmeticResultu8 underflowing_sub_u8(uint8_t value1, uint8_t value2)
{
	arithmeticResultu8 result;
	result.wrappedValue  = value1 - value2;
	result.didCarry = value1 < value2;

	return result;
}

arithmeticResultu8 underflowing_sub_u8(uint8_t value1, uint8_t value2, uint8_t value3)
{
	arithmeticResultu8 result;
	result.wrappedValue = (value1 - value2)-value3;
	result.didCarry = value1 < ((uint16_t)value2+ (uint16_t)value3);

	return result;
}

arithmeticResultu16 underflowing_sub_u16(uint16_t value1, uint16_t value2)
{
	arithmeticResultu16 result;
	result.wrappedValue = value1 - value2;
	result.didCarry = value1 < value2;

	return result;
}
