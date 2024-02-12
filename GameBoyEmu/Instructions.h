#pragma once
#include <cstdint>
#include "CPU.h"


extern FullInstruction eightBitInstructions[0xFF];
extern FullInstruction sixteenBitInstructions[0xFF];

void initInstructionSet();

void setEightBitInstruction(uint8_t instruction_byte, Instruction instruction,
	ArithmeticTarget t1 = INVALID, ArithmeticTarget t2 = INVALID);