#pragma once
#include "Registers.h"
#include "MemoryBus.h"
#include <stdexcept>

enum Instruction {
	UNINITALISED,
	ADD,
	ADDHL,
	AND,
	OR,
	XOR,
	CPL,
	SUB,
	CP,
	INC,
	INC16,
	DEC,
	DEC16,
	NOP,
	CCF,
	ADC,
	SBC,
	SCF,
	LD
};

enum ArithmeticTarget {
	INVALID,
	A,
	B,
	C,
	D,
	E,
	H,
	L,
	HL,
	BC,
	DE,
	SP,
	d8,
	HL_POS,
	HL_NEG,
	HL_AS_ADDRESS,
	DE_AS_ADDRESS,
	BC_AS_ADDRESS,
	C_AS_LOWER_ADDRESS,
	a8,
	a16,
	s8,
	SP_PLUS_s8,
	d16

};

struct FullInstruction {
	Instruction instruction;
	ArithmeticTarget op1;
	ArithmeticTarget op2;

	FullInstruction(Instruction i = UNINITALISED, 
		ArithmeticTarget one = INVALID,
		ArithmeticTarget two = INVALID) {
		instruction = i;
		op1 = one;
		op2 = two;
	}
};

struct CPU {
	Registers registers;
	uint16_t pc;
	uint16_t sp;
	MemoryBus bus;
};

#pragma region CPU_INSTRUCTIONS

void CPU_step(CPU& cpu);

void CPU_excecute(CPU& cpu, FullInstruction fullInstruction);

void CPU_ADD(CPU& cpu, ArithmeticTarget target);

void CPU_ADDHL(CPU& cpu, ArithmeticTarget target);

void CPU_AND(CPU& cpu, ArithmeticTarget target);

void CPU_OR(CPU& cpu, ArithmeticTarget target);

void CPU_XOR(CPU& cpu, ArithmeticTarget target);

void CPU_CPL(CPU& cpu);

void CPU_SUB(CPU& cpu, ArithmeticTarget target);

void CPU_CP(CPU& cpu, ArithmeticTarget target);

void CPU_INC(CPU& cpu, ArithmeticTarget target);

void CPU_INC16(CPU& cpu, ArithmeticTarget target);

void CPU_DEC(CPU& cpu, ArithmeticTarget target);

void CPU_DEC16(CPU& cpu, ArithmeticTarget target);

void CPU_CCF(CPU& cpu);

void CPU_NOP(CPU& cpu);

void CPU_ADC(CPU& cpu, ArithmeticTarget target);

void CPU_SBC(CPU& cpu, ArithmeticTarget target);

void CPU_SCF(CPU& cpu);

void CPU_LD(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2);

void CPU_LD_8BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2);

void CPU_LD_16BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2);

#pragma endregion

#pragma region CPU_HELPER_FUNCTIONS

bool CPU_LD_VALIDATION(ArithmeticTarget t1, ArithmeticTarget t2);

void* ArTarget_To_Ptr(CPU& cpu, ArithmeticTarget target);

sixteenBitValuePtrs ArTarget_To_Ptr_16b(CPU& cpu, ArithmeticTarget target);

#pragma endregion