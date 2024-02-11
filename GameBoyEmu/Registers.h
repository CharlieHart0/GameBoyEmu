#pragma once
#include <cstdint>
#include <stdexcept>

// Flag Registers


const uint8_t ZERO_FLAG_BYTE_POS = 7;
const uint8_t SUBTRACT_FLAG_BYTE_POS = 6;
const uint8_t HALF_CARRY_FLAG_BYTE_POS = 5;
const uint8_t CARRY_FLAG_BYTE_POS = 4;

struct FlagsRegister {
	bool zero;
	bool subtract;
	bool half_carry;
	bool carry;

	FlagsRegister(uint8_t value) {
		zero = ((value >> ZERO_FLAG_BYTE_POS) & 0b1);
		subtract = ((value >> SUBTRACT_FLAG_BYTE_POS) & 0b1);
		carry = ((value >> CARRY_FLAG_BYTE_POS) & 0b1);
		half_carry = ((value >> HALF_CARRY_FLAG_BYTE_POS) & 0b1);

	}

	operator uint8_t() {
		uint8_t value = 0;
		if (zero) { value |= (1 << ZERO_FLAG_BYTE_POS); }
		if (subtract) { value |= (1 << SUBTRACT_FLAG_BYTE_POS); }
		if (half_carry) { value |= (1 << HALF_CARRY_FLAG_BYTE_POS); }
		if (carry) { value |= (1 << CARRY_FLAG_BYTE_POS); }
		return value;
	}
};


struct sixteenBitValuePtrs {
	uint8_t* lower;
	uint8_t* higher;
};

// Main Registers

struct Registers {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	FlagsRegister f;
	uint8_t h;
	uint8_t l;
};

enum Reg_pair {
	af,
	bc,
	de,
	hl,
	sp
};


uint16_t Reg_get_16bit(Registers& registers, Reg_pair pair);
void Reg_set_16bit(Registers& registers, Reg_pair pair, uint16_t value);






