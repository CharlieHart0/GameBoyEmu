#include "Instructions.h"
#include <string>
#include <sstream>
#include <iomanip>

FullInstruction eightBitInstructions[0xFF]{};
FullInstruction sixteenBitInstructions[0xFF]{};

void initInstructionSet()
{

	// instruction set guide https://meganesu.github.io/generate-gb-opcodes/

	// this is how the instruction byte is decoded into a full instruction 
	// (Instruction and 0-2 operands)

#pragma region ADD

	eightBitInstructions[0x80].instruction = ADD;
	eightBitInstructions[0x80].op1 = A;
	eightBitInstructions[0x80].op2 = B;

	eightBitInstructions[0x81].instruction = ADD;
	eightBitInstructions[0x81].op1 = A;
	eightBitInstructions[0x81].op2 = C;

	eightBitInstructions[0x82].instruction = ADD;
	eightBitInstructions[0x82].op1 = A;
	eightBitInstructions[0x82].op2 = D;

	eightBitInstructions[0x83].instruction = ADD;
	eightBitInstructions[0x83].op1 = A;
	eightBitInstructions[0x83].op2 = E;

	eightBitInstructions[0x84].instruction = ADD;
	eightBitInstructions[0x84].op1 = A;
	eightBitInstructions[0x84].op2 = H;

	eightBitInstructions[0x85].instruction = ADD;
	eightBitInstructions[0x85].op1 = A;
	eightBitInstructions[0x85].op2 = L;

	eightBitInstructions[0x86].instruction = ADD;
	eightBitInstructions[0x86].op1 = A;
	eightBitInstructions[0x86].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0x87].instruction = ADD;
	eightBitInstructions[0x87].op1 = A;
	eightBitInstructions[0x87].op2 = A;

	eightBitInstructions[0xC6].instruction = ADD;
	eightBitInstructions[0xC6].op1 = A;
	eightBitInstructions[0xC6].op2 = d8;

#pragma endregion

#pragma region SUB

	eightBitInstructions[0x90].instruction = SUB;
	eightBitInstructions[0x90].op2 = B;

	eightBitInstructions[0x91].instruction = SUB;
	eightBitInstructions[0x91].op2 = C;

	eightBitInstructions[0x92].instruction = SUB;
	eightBitInstructions[0x92].op2 = D;

	eightBitInstructions[0x93].instruction = SUB;
	eightBitInstructions[0x93].op2 = E;

	eightBitInstructions[0x94].instruction = SUB;
	eightBitInstructions[0x94].op2 = H;

	eightBitInstructions[0x95].instruction = SUB;
	eightBitInstructions[0x95].op2 = L;

	eightBitInstructions[0x96].instruction = SUB;
	eightBitInstructions[0x96].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0x97].instruction = SUB;
	eightBitInstructions[0x97].op2 = A;

	eightBitInstructions[0xD6].instruction = SUB;
	eightBitInstructions[0xD6].op2 = d8;

#pragma endregion

#pragma region CP

	eightBitInstructions[0xB8].instruction = CP;
	eightBitInstructions[0xB8].op2 = B;

	eightBitInstructions[0xB9].instruction = CP;
	eightBitInstructions[0xB9].op2 = C;

	eightBitInstructions[0xBA].instruction = CP;
	eightBitInstructions[0xBA].op2 = D;

	eightBitInstructions[0xBB].instruction = CP;
	eightBitInstructions[0xBB].op2 = E;

	eightBitInstructions[0xBC].instruction = CP;
	eightBitInstructions[0xBC].op2 = H;

	eightBitInstructions[0xBD].instruction = CP;
	eightBitInstructions[0xBD].op2 = L;

	eightBitInstructions[0xBE].instruction = CP;
	eightBitInstructions[0xBE].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0xBF].instruction = CP;
	eightBitInstructions[0xBF].op2 = A;

	eightBitInstructions[0xFE].instruction = CP;
	eightBitInstructions[0xFE].op2 = d8;

#pragma endregion

#pragma region ADDHL

	eightBitInstructions[0x09].instruction = ADDHL;
	eightBitInstructions[0x09].op1 = HL;
	eightBitInstructions[0x09].op2 = BC;

	eightBitInstructions[0x19].instruction = ADDHL;
	eightBitInstructions[0x19].op1 = HL;
	eightBitInstructions[0x19].op2 = DE;

	eightBitInstructions[0x29].instruction = ADDHL;
	eightBitInstructions[0x29].op1 = HL;
	eightBitInstructions[0x29].op2 = HL;

	eightBitInstructions[0x39].instruction = ADDHL;
	eightBitInstructions[0x39].op1 = HL;
	eightBitInstructions[0x39].op2 = SP;

#pragma endregion

#pragma region AND

	eightBitInstructions[0xA0].instruction = AND;
	eightBitInstructions[0xA0].op2 = B;

	eightBitInstructions[0xA1].instruction = AND;
	eightBitInstructions[0xA1].op2 = C;

	eightBitInstructions[0xA2].instruction = AND;
	eightBitInstructions[0xA2].op2 = D;

	eightBitInstructions[0xA3].instruction = AND;
	eightBitInstructions[0xA3].op2 = E;

	eightBitInstructions[0xA4].instruction = AND;
	eightBitInstructions[0xA4].op2 = H;

	eightBitInstructions[0xA5].instruction = AND;
	eightBitInstructions[0xA5].op2 = L;

	eightBitInstructions[0xA6].instruction = AND;
	eightBitInstructions[0xA6].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0xA7].instruction = AND;
	eightBitInstructions[0xA7].op2 = A;

	eightBitInstructions[0xE6].instruction = AND;
	eightBitInstructions[0xE6].op2 = d8;

#pragma endregion

#pragma region OR

	eightBitInstructions[0xB0].instruction = OR;
	eightBitInstructions[0xB0].op2 = B;

	eightBitInstructions[0xB1].instruction = OR;
	eightBitInstructions[0xB1].op2 = C;

	eightBitInstructions[0xB2].instruction = OR;
	eightBitInstructions[0xB2].op2 = D;

	eightBitInstructions[0xB3].instruction = OR;
	eightBitInstructions[0xB3].op2 = E;

	eightBitInstructions[0xB4].instruction = OR;
	eightBitInstructions[0xB4].op2 = H;

	eightBitInstructions[0xB5].instruction = OR;
	eightBitInstructions[0xB5].op2 = L;

	eightBitInstructions[0xB6].instruction = OR;
	eightBitInstructions[0xB6].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0xB7].instruction = OR;
	eightBitInstructions[0xB7].op2 = A;

	eightBitInstructions[0xF6].instruction = OR;
	eightBitInstructions[0xF6].op2 = d8;

#pragma endregion

#pragma region XOR

	eightBitInstructions[0xA8].instruction = XOR;
	eightBitInstructions[0xA8].op2 = B;

	eightBitInstructions[0xA9].instruction = XOR;
	eightBitInstructions[0xA9].op2 = C;

	eightBitInstructions[0xAA].instruction = XOR;
	eightBitInstructions[0xAA].op2 = D;

	eightBitInstructions[0xAB].instruction = XOR;
	eightBitInstructions[0xAB].op2 = E;

	eightBitInstructions[0xAC].instruction = XOR;
	eightBitInstructions[0xAC].op2 = H;

	eightBitInstructions[0xAD].instruction = XOR;
	eightBitInstructions[0xAD].op2 = L;

	eightBitInstructions[0xAE].instruction = XOR;
	eightBitInstructions[0xAE].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0xAF].instruction = XOR;
	eightBitInstructions[0xAF].op2 = A;

	eightBitInstructions[0xEE].instruction = XOR;
	eightBitInstructions[0xEE].op2 = d8;

#pragma endregion

#pragma region CPL

	eightBitInstructions[0x2F].instruction = CPL;

#pragma endregion

#pragma region INC

	eightBitInstructions[0x04].instruction = INC;
	eightBitInstructions[0x04].op1 = B;

	eightBitInstructions[0x0C].instruction = INC;
	eightBitInstructions[0x0C].op1 = C;

	eightBitInstructions[0x14].instruction = INC;
	eightBitInstructions[0x14].op1 = D;

	eightBitInstructions[0x1C].instruction = INC;
	eightBitInstructions[0x1C].op1 = E;

	eightBitInstructions[0x24].instruction = INC;
	eightBitInstructions[0x24].op1 = H;

	eightBitInstructions[0x2C].instruction = INC;
	eightBitInstructions[0x2C].op1 = L;

	eightBitInstructions[0x34].instruction = INC;
	eightBitInstructions[0x34].op1 = HL_AS_ADDRESS;

	eightBitInstructions[0x3C].instruction = INC;
	eightBitInstructions[0x3C].op1 = A;

#pragma endregion

#pragma region INC16

	eightBitInstructions[0x03].instruction = INC16;
	eightBitInstructions[0x03].op1 = BC;

	eightBitInstructions[0x13].instruction = INC16;
	eightBitInstructions[0x13].op1 = DE;

	eightBitInstructions[0x23].instruction = INC16;
	eightBitInstructions[0x23].op1 = HL;

	eightBitInstructions[0x33].instruction = INC16;
	eightBitInstructions[0x33].op1 = SP;

#pragma endregion

#pragma region DEC

	eightBitInstructions[0x05].instruction = DEC;
	eightBitInstructions[0x05].op1 = B;

	eightBitInstructions[0x0D].instruction = DEC;
	eightBitInstructions[0x0D].op1 = C;

	eightBitInstructions[0x15].instruction = DEC;
	eightBitInstructions[0x15].op1 = D;

	eightBitInstructions[0x1D].instruction = DEC;
	eightBitInstructions[0x1D].op1 = E;

	eightBitInstructions[0x25].instruction = DEC;
	eightBitInstructions[0x25].op1 = H;

	eightBitInstructions[0x2D].instruction = DEC;
	eightBitInstructions[0x2D].op1 = L;

	eightBitInstructions[0x35].instruction = DEC;
	eightBitInstructions[0x35].op1 = HL_AS_ADDRESS;

	eightBitInstructions[0x3D].instruction = DEC;
	eightBitInstructions[0x3D].op1 = A;

#pragma endregion

#pragma region DEC16

	eightBitInstructions[0x0B].instruction = DEC16;
	eightBitInstructions[0x0B].op1 = BC;

	eightBitInstructions[0x1B].instruction = DEC16;
	eightBitInstructions[0x1B].op1 = DE;

	eightBitInstructions[0x2B].instruction = DEC16;
	eightBitInstructions[0x2B].op1 = HL;

	eightBitInstructions[0x3B].instruction = DEC16;
	eightBitInstructions[0x3B].op1 = SP;

#pragma endregion

#pragma region CCF

	eightBitInstructions[0x3F].instruction = CCF;

#pragma endregion

#pragma region NOP

	eightBitInstructions[0x00].instruction = NOP;

#pragma endregion

#pragma region ADC

	eightBitInstructions[0x88].instruction = ADC;
	eightBitInstructions[0x88].op2 = B;

	eightBitInstructions[0x89].instruction = ADC;
	eightBitInstructions[0x89].op2 = C;

	eightBitInstructions[0x8A].instruction = ADC;
	eightBitInstructions[0x8A].op2 = D;

	eightBitInstructions[0x8B].instruction = ADC;
	eightBitInstructions[0x8B].op2 = E;

	eightBitInstructions[0x8C].instruction = ADC;
	eightBitInstructions[0x8C].op2 = H;

	eightBitInstructions[0x8D].instruction = ADC;
	eightBitInstructions[0x8D].op2 = L;

	eightBitInstructions[0x8E].instruction = ADC;
	eightBitInstructions[0x8E].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0x8F].instruction = ADC;
	eightBitInstructions[0x8F].op2 = A;

	eightBitInstructions[0xCE].instruction = ADC;
	eightBitInstructions[0xCE].op2 = d8;

#pragma endregion

#pragma region SBC

	eightBitInstructions[0x98].instruction = SBC;
	eightBitInstructions[0x98].op2 = B;

	eightBitInstructions[0x99].instruction = SBC;
	eightBitInstructions[0x99].op2 = C;

	eightBitInstructions[0x9A].instruction = SBC;
	eightBitInstructions[0x9A].op2 = D;

	eightBitInstructions[0x9B].instruction = SBC;
	eightBitInstructions[0x9B].op2 = E;

	eightBitInstructions[0x9C].instruction = SBC;
	eightBitInstructions[0x9C].op2 = H;

	eightBitInstructions[0x9D].instruction = SBC;
	eightBitInstructions[0x9D].op2 = L;

	eightBitInstructions[0x9E].instruction = SBC;
	eightBitInstructions[0x9E].op2 = HL_AS_ADDRESS;

	eightBitInstructions[0x9F].instruction = SBC;
	eightBitInstructions[0x9F].op2 = A;

	eightBitInstructions[0xDE].instruction = SBC;
	eightBitInstructions[0xDE].op2 = d8;

#pragma endregion

#pragma region SCF

	eightBitInstructions[0x37].instruction = SCF;

#pragma endregion

#pragma region LD

	#pragma region LD_MAIN

		// Instructions 0x40 to 0x7F
		ArithmeticTarget ops[8] = { B,C,D,E,H,L,HL_AS_ADDRESS,A };


		// this saved me literally half an hour of typing setEightBitInstruction(0x40, LD, B, B);
		for (int op1 = 0; op1 < 8; op1++) {
			for (int op2 = 0; op2 < 8; op2++) {
				if (!(ops[op1] == HL_AS_ADDRESS && ops[op2] == HL_AS_ADDRESS)) {
					setEightBitInstruction( (0x40 + (op1 * 8) + op2), LD, ops[op1], ops[op2]);
				}
			}
		}

	#pragma endregion

	#pragma region LD_16BIT

		#pragma region LD 16 bit pair, d16

			eightBitInstructions[0x01].instruction = LD;
			eightBitInstructions[0x01].op1 = BC;
			eightBitInstructions[0x01].op2 = d16;

			eightBitInstructions[0x11].instruction = LD;
			eightBitInstructions[0x11].op1 = DE;
			eightBitInstructions[0x11].op2 = d16;

			eightBitInstructions[0x21].instruction = LD;
			eightBitInstructions[0x21].op1 = HL;
			eightBitInstructions[0x21].op2 = d16;

			eightBitInstructions[0x31].instruction = LD;
			eightBitInstructions[0x31].op1 = SP;
			eightBitInstructions[0x31].op2 = d16;

		#pragma endregion

		#pragma region LD a16, SP

			setEightBitInstruction(0x08, LD, a16, SP);

		#pragma endregion

		#pragma region LD a16, A and A, a16

			setEightBitInstruction(0xEA, LD, a16, A);
			setEightBitInstruction(0xFA, LD, A, a16);

		#pragma endregion
		
		#pragma region LD HL, SP_PLUS_s8  and SP, HL

			setEightBitInstruction(0xF8, LD, HL, SP_PLUS_s8);
			setEightBitInstruction(0xF9, LD, SP, HL);

		#pragma endregion

	#pragma endregion

	#pragma region LD_8BIT_EXTRA

		#pragma region LD C_AS_ADDRESS, A and A, C_AS_ADDRESS

			setEightBitInstruction(0xE2, LD, C_AS_LOWER_ADDRESS, A);
			setEightBitInstruction(0xF2, LD, A, C_AS_LOWER_ADDRESS);

		#pragma endregion

		#pragma region LD a8,A and A, a8

		setEightBitInstruction(0xF0, LD, a8, A);
		setEightBitInstruction(0xE0, LD, A, a8);

		#pragma endregion

		#pragma region LD (A-L / HL AS ADDRESS), d8   

		//Code 0x(0-3)6  and  0x(0-3)E
		setEightBitInstruction(0x06, LD, B, d8);
		setEightBitInstruction(0x16, LD, D, d8);
		setEightBitInstruction(0x26, LD, H, d8);
		setEightBitInstruction(0x36, LD, HL_AS_ADDRESS, d8);

		setEightBitInstruction(0x0E, LD, C, d8);
		setEightBitInstruction(0x1E, LD, E, d8);
		setEightBitInstruction(0x2E, LD, L, d8);
		setEightBitInstruction(0x3E, LD, A, d8);

		#pragma endregion

		#pragma region LD pair as address, A 

		// Code 0x(0-3)2 
		setEightBitInstruction(0x02, LD, BC_AS_ADDRESS, A);
		setEightBitInstruction(0x12, LD, DE_AS_ADDRESS, A);
		setEightBitInstruction(0x22, LD, HL_POS, A);
		setEightBitInstruction(0x32, LD, HL_NEG, A);

		#pragma endregion

		#pragma region LD A, pair as address 
		
		// Code 0x(0-3)A
		setEightBitInstruction(0x0A, LD, A, BC_AS_ADDRESS);
		setEightBitInstruction(0x1A, LD, A, DE_AS_ADDRESS);
		setEightBitInstruction(0x2A, LD, A, HL_POS);
		setEightBitInstruction(0x3A, LD, A, HL_NEG);


		#pragma endregion

	#pragma endregion

#pragma endregion

#pragma region PUSH

		setEightBitInstruction(0xC5, PUSH, BC);
		setEightBitInstruction(0xD5, PUSH, DE);
		setEightBitInstruction(0xE5, PUSH, HL);
		setEightBitInstruction(0xF5, PUSH, AF);

#pragma endregion

#pragma region POP

		setEightBitInstruction(0xC1, POP, BC);
		setEightBitInstruction(0xD1, POP, DE);
		setEightBitInstruction(0xE1, POP, HL);
		setEightBitInstruction(0xF1, POP, AF);

#pragma endregion
}

void setEightBitInstruction(uint8_t instruction_byte, Instruction instruction, ArithmeticTarget t1, ArithmeticTarget t2)
{
	FullInstruction* fi_ptr = &eightBitInstructions[instruction_byte];
	if (fi_ptr->instruction != UNINITALISED || fi_ptr->op1 != INVALID || fi_ptr->op2 != INVALID) {
		std::stringstream errMsg;
		errMsg << "Tried to set an eight bit instruction at byte: " << std::hex << 
			(uint8_t)fi_ptr->instruction <<
			", which has already been set!";
		throw std::exception(errMsg.str().c_str());
	}
	fi_ptr->instruction = instruction;
	fi_ptr->op1 = t1;
	fi_ptr->op2 = t2;
}
