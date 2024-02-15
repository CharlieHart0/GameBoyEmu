#pragma once
#include <cstdint>
#include "CPU.h"


extern FullInstruction eightBitInstructions[0xFF];
extern FullInstruction prefixedInstructions[0xFF];

void initInstructionSet();

void setEightBitInstruction(uint8_t instruction_byte, Instruction instruction,
	ArithmeticTarget t1 = INVALID, ArithmeticTarget t2 = INVALID);

void setPrefixedInstruction(uint8_t instruction_byte, Instruction instruction, ArithmeticTarget t1 = INVALID , ArithmeticTarget t2 = INVALID);