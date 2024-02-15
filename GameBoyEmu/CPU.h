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
	LD,
	PUSH,
	POP,
	JR,
	JP,
	CALL,
	RET,
	EI,
	DI,
	RST,
	RETI,
	RLCA,
	RLA,
	RRCA,
	RRA,
	DAA,
	SET,
	RES,
	BIT,
	SWAP
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
	d16,
	AF,
	NZ,
	Z,
	NC

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

	bool ime = true; //TODO should this be true by default? boot rom will probably make sure its correct anyway
};

#pragma region CPU_INSTRUCTIONS

void CPU_step(CPU& cpu);

void CPU_excecute(CPU& cpu, FullInstruction fullInstruction);

void CPU_ADD(CPU& cpu, ArithmeticTarget target, ArithmeticTarget t2 = INVALID);

void CPU_ADD_0xE8(CPU& cpu);

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

void CPU_PUSH(CPU& cpu, ArithmeticTarget t1);

void CPU_POP(CPU& cpu, ArithmeticTarget t1);

void CPU_JR(CPU& cpu, ArithmeticTarget target);

void CPU_JP(CPU& cpu, ArithmeticTarget target);

void CPU_CALL(CPU& cpu, ArithmeticTarget target);

void CPU_RET(CPU& cpu, ArithmeticTarget target = INVALID);

void CPU_EI(CPU& cpu);

void CPU_DI(CPU& cpu);

void CPU_RST(CPU& cpu, int value);

void CPU_RETI(CPU& cpu);

void CPU_RLCA(CPU& cpu);

void CPU_RLA(CPU& cpu);

void CPU_RRCA(CPU& cpu);

void CPU_RRA(CPU& cpu);

void CPU_DAA(CPU& cpu);

// prefixed instructions

void CPU_SET(CPU& cpu, ArithmeticTarget target, int value);

void CPU_RES(CPU& cpu, ArithmeticTarget target, int value);

void CPU_BIT(CPU& cpu, int value, ArithmeticTarget target);

void CPU_SWAP(CPU& cpu, ArithmeticTarget target);

#pragma endregion

#pragma region CPU_HELPER_FUNCTIONS

bool CPU_LD_VALIDATION(ArithmeticTarget t1, ArithmeticTarget t2);

void* ArTarget_To_Ptr(CPU& cpu, ArithmeticTarget target);

sixteenBitValuePtrs ArTarget_To_Ptr_16b(CPU& cpu, ArithmeticTarget target);

sixteenBitValuePtrs uint16_to_uint8_ptrs(uint16_t* ptrIn);

#pragma endregion