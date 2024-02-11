#include "CPU.h"
#include "Instructions.h"

void CPU_step(CPU& cpu)
{
	uint8_t instruction_byte = MemoryBus_read_byte(cpu.bus, cpu.pc);

	//TODO 16 bit instrucitons
	if (eightBitInstructions[instruction_byte].instruction != UNINITALISED) {
		CPU_excecute(cpu, eightBitInstructions[instruction_byte].instruction);
	}
	else {
		char exceptionMessage[37];
		snprintf(exceptionMessage, 37, "Invalid 8 bit instruction byte: %X", instruction_byte);
		throw std::invalid_argument(exceptionMessage);
	}
}

void CPU_excecute(CPU &cpu, FullInstruction fullInstruction)
{
	Registers* registers = &cpu.registers;

	switch (fullInstruction.instruction){
	case ADD:
		CPU_ADD(cpu, fullInstruction.op2);
		break;
	case ADDHL:
		CPU_ADDHL(cpu, fullInstruction.op2);
		break;
	case AND:
		CPU_AND(cpu, fullInstruction.op2);
		break;
	case OR:
		CPU_OR(cpu, fullInstruction.op2);
		break;
	case XOR:
		CPU_XOR(cpu, fullInstruction.op2);
		break;
	case CPL:
		CPU_CPL(cpu);
		break;
	case SUB:
		CPU_SUB(cpu, fullInstruction.op2);
		break;
	case CP:
		CPU_CP(cpu, fullInstruction.op2);
		break;
	case INC:
		CPU_INC(cpu, fullInstruction.op1);
		break;
	case INC16:
		CPU_INC16(cpu, fullInstruction.op1);
		break;
	case DEC16:
		CPU_DEC16(cpu, fullInstruction.op1);
		break;
	case CCF:
		CPU_CCF(cpu);
		break;
	case NOP:
		CPU_NOP(cpu);
		break;
	case ADC:
		CPU_ADC(cpu, fullInstruction.op2);
		break;
	case SBC:
		CPU_SBC(cpu, fullInstruction.op2);
		break;
	case SCF:
		CPU_SCF(cpu);
		break;
	case LD:
		CPU_LD(cpu, fullInstruction.op1, fullInstruction.op2);
		break;



	default:
		throw std::invalid_argument("Invalid 8 bit instruction!");
		break;
	}
}



#pragma region CPU_INSTRUCTIONS


void CPU_ADD(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	switch (target) {
	case A:
		operand = registers->a;
		break;
	case B:
		operand = registers->b;
		break;
	case C:
		operand = registers->c;
		break;
	case D:
		operand = registers->d;
		break;
	case E:
		operand = registers->e;
		break;
	case H:
		operand = registers->h;
		break;
	case L:
		operand = registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		operand = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_ADD recieved invalid target register!!");
		return;
	}
	arithmeticResultu8 result = overflowing_add_u8(registers->a, operand);
	

	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = false;
	registers->f.carry = result.didCarry;

	// if two lower nibbles of numbers being added add to more than 0xF
	registers->f.half_carry = (registers->a & 0xF) + (operand & 0xF) > 0xF;

	// set register A to the result at the end, as to not mess with half carry flag if targetRegister is also A
	registers->a = result.wrappedValue;
}

void CPU_ADDHL(CPU& cpu, ArithmeticTarget target)
{
	uint16_t operand;
	cpu.pc += 1;
	switch (target) {
	case HL:
		operand = Reg_get_16bit(cpu.registers, hl);
		break;
	case BC:
		operand = Reg_get_16bit(cpu.registers, bc);
		break;
	case DE:
		operand = Reg_get_16bit(cpu.registers, de);
		break;
	case SP:
		operand = cpu.sp;
		break;
	default:
		throw std::invalid_argument("CPU_ADDHL recieved invalid target register!!");
		return;
	}

	uint16_t hlValue = Reg_get_16bit(cpu.registers, hl);

	arithmeticResultu16 result = overflowing_add_u16(hlValue, operand);

	
	//set flags
	cpu.registers.f.subtract = 0;
	cpu.registers.f.carry = result.didCarry;
	cpu.registers.f.half_carry = ((hlValue & 0x0FFF) + (operand & 0x0FFF)) > 0x0FFF;


	Reg_set_16bit(cpu.registers, hl, result.wrappedValue);
}

void CPU_AND(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;

	uint8_t op2;

	switch (target) {
	case A:
		op2 = cpu.registers.a;
		break;
	case B:
		op2 = cpu.registers.b;
		break;
	case C:
		op2 = cpu.registers.c;
		break;
	case D:
		op2 = cpu.registers.d;
		break;
	case E:
		op2 = cpu.registers.e;
		break;
	case H:
		op2 = cpu.registers.h;
		break;
	case L:
		op2 = cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		op2 = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		op2 = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_AND recieved invalid target register!!");
		return;
	}

	uint8_t result = (cpu.registers.a & op2);

	// the values of half carry and carry seem like they shouldnt be constant here 
	// but eh idk this is what the internet told me to do
	cpu.registers.f.zero = result == 0x00;
	cpu.registers.f.subtract = 0;
	cpu.registers.f.half_carry = 1;
	cpu.registers.f.carry = 0;
}

void CPU_OR(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;

	uint8_t op2;

	switch (target) {
	case A:
		op2 = cpu.registers.a;
		break;
	case B:
		op2 = cpu.registers.b;
		break;
	case C:
		op2 = cpu.registers.c;
		break;
	case D:
		op2 = cpu.registers.d;
		break;
	case E:
		op2 = cpu.registers.e;
		break;
	case H:
		op2 = cpu.registers.h;
		break;
	case L:
		op2 = cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		op2 = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		op2 = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_OR recieved invalid target register!!");
		return;
	}

	uint8_t result = (cpu.registers.a | op2);

	// the values of half carry and carry seem like they shouldnt be constant here 
	// but eh idk this is what the internet told me to do
	cpu.registers.f.zero = result == 0x00;
	cpu.registers.f.subtract = 0;
	cpu.registers.f.half_carry = 0;
	cpu.registers.f.carry = 0;
}

void CPU_XOR(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;

	uint8_t op2;

	switch (target) {
	case A:
		op2 = cpu.registers.a;
		break;
	case B:
		op2 = cpu.registers.b;
		break;
	case C:
		op2 = cpu.registers.c;
		break;
	case D:
		op2 = cpu.registers.d;
		break;
	case E:
		op2 = cpu.registers.e;
		break;
	case H:
		op2 = cpu.registers.h;
		break;
	case L:
		op2 = cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		op2 = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		op2 = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_XOR recieved invalid target register!!");
		return;
	}

	uint8_t result = (cpu.registers.a ^ op2);

	// the values of half carry and carry seem like they shouldnt be constant here 
	// but eh idk this is what the internet told me to do
	cpu.registers.f.zero = result == 0x00;
	cpu.registers.f.subtract = 0;
	cpu.registers.f.half_carry = 0;
	cpu.registers.f.carry = 0;
}

void CPU_CPL(CPU& cpu)
{
	cpu.pc += 1;

	// flip all bits
	cpu.registers.a ^= 0xFF;

	cpu.registers.f.subtract = 0;
	cpu.registers.f.half_carry = 0;
}

void CPU_SUB(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	switch (target) {
	case A:
		operand = registers->a;
		break;
	case B:
		operand = registers->b;
		break;
	case C:
		operand = registers->c;
		break;
	case D:
		operand = registers->d;
		break;
	case E:
		operand = registers->e;
		break;
	case H:
		operand = registers->h;
		break;
	case L:
		operand = registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		operand = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_SUB recieved invalid target register!!");
		return;
	}
	
	arithmeticResultu8 result = overflowing_add_u8(registers->a, operand);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = true;
	registers->f.carry = result.didCarry;

	registers->f.half_carry = (uint16_t)(registers->a & 0xF) - (uint16_t)(operand & 0xF) < 0xF;

	registers->a = result.wrappedValue;
}

void CPU_CP(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	switch (target) {
	case A:
		operand = registers->a;
		break;
	case B:
		operand = registers->b;
		break;
	case C:
		operand = registers->c;
		break;
	case D:
		operand = registers->d;
		break;
	case E:
		operand = registers->e;
		break;
	case H:
		operand = registers->h;
		break;
	case L:
		operand = registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		operand = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_CP recieved invalid target register!!");
		return;
	}

	arithmeticResultu8 result = overflowing_add_u8(registers->a, operand);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = true;
	registers->f.carry = result.didCarry;

	registers->f.half_carry = (registers->a & 0xF) - (operand & 0xF) < 0xF;

}

void CPU_INC(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	uint8_t* operandLocation = nullptr;
	switch (target) {
	case A:
		operand = registers->a;
		operandLocation = &registers->a;
		break;
	case B:
		operand = registers->b;
		operandLocation = &registers->b;
		break;
	case C:
		operand = registers->c;
		operandLocation = &registers->c;
		break;
	case D:
		operand = registers->d;
		operandLocation = &registers->d;
		break;
	case E:
		operand = registers->e;
		operandLocation = &registers->e;
		break;
	case H:
		operand = registers->h;
		operandLocation = &registers->h;
		break;
	case L:
		operand = registers->l;
		operandLocation = &registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	default:
		throw std::invalid_argument("CPU_INC recieved invalid target register!!");
		return;
	}
	arithmeticResultu8 result = overflowing_add_u8(operand, 0x01);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = false;

	// if two lower nibbles of numbers being added add to more than 0xF
	registers->f.half_carry = (0x01& 0xF) + (operand & 0xF) > 0xF;

	if (target != HL_AS_ADDRESS) {	*operandLocation = result.wrappedValue;}
	else { MemoryBus_write_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl),result.wrappedValue); }
}

void CPU_INC16(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint16_t operand;
	Reg_pair location; // not used if target is SP
	switch (target) {
	case HL:
		operand = Reg_get_16bit(cpu.registers,hl);
		location = hl;
		break;
	case DE:
		operand = Reg_get_16bit(cpu.registers, de);
		location = de;
		break;
	case BC:
		operand = Reg_get_16bit(cpu.registers, bc);
		location = bc;
		break;
	case SP:
		operand = cpu.sp;
		break;
	default:
		throw std::invalid_argument("CPU_INC16 recieved invalid target register!!");
		return;
	}
	arithmeticResultu16 result = overflowing_add_u16(operand, 0x01);


	// flag registers are not set in this instruction

	if (target != SP) { Reg_set_16bit(cpu.registers, location, result.wrappedValue); }
	else { cpu.sp = result.wrappedValue; }
}

void CPU_DEC(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	uint8_t* operandLocation = nullptr;
	switch (target) {
	case A:
		operand = registers->a;
		operandLocation = &registers->a;
		break;
	case B:
		operand = registers->b;
		operandLocation = &registers->b;
		break;
	case C:
		operand = registers->c;
		operandLocation = &registers->c;
		break;
	case D:
		operand = registers->d;
		operandLocation = &registers->d;
		break;
	case E:
		operand = registers->e;
		operandLocation = &registers->e;
		break;
	case H:
		operand = registers->h;
		operandLocation = &registers->h;
		break;
	case L:
		operand = registers->l;
		operandLocation = &registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	default:
		throw std::invalid_argument("CPU_DEC recieved invalid target register!!");
		return;
	}

	arithmeticResultu8 result = underflowing_sub_u8(operand, 1);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = true;

	registers->f.half_carry = ((operand & 0xF) - 0x1) < 0xF;

	if (target != HL_AS_ADDRESS) { *operandLocation = result.wrappedValue; }
	else { MemoryBus_write_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl), result.wrappedValue); }
}

void CPU_DEC16(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint16_t operand;
	Reg_pair location; // not used if target is SP
	switch (target) {
	case HL:
		operand = Reg_get_16bit(cpu.registers, hl);
		location = hl;
		break;
	case DE:
		operand = Reg_get_16bit(cpu.registers, de);
		location = de;
		break;
	case BC:
		operand = Reg_get_16bit(cpu.registers, bc);
		location = bc;
		break;
	case SP:
		operand = cpu.sp;
		break;
	default:
		throw std::invalid_argument("CPU_DEC16 recieved invalid target register!!");
		return;
	}
	arithmeticResultu16 result = underflowing_sub_u16(operand, 0x01);


	// flag registers are not set in this instruction

	if (target != SP) { Reg_set_16bit(cpu.registers, location, result.wrappedValue); }
	else { cpu.sp = result.wrappedValue; }
}

void CPU_CCF(CPU& cpu)
{
	cpu.pc += 1;

	// flip carry flag register
	cpu.registers.f.carry ^= true;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
}

void CPU_NOP(CPU& cpu)
{
	cpu.pc += 1;
}

void CPU_ADC(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	switch (target) {
	case A:
		operand = registers->a;
		break;
	case B:
		operand = registers->b;
		break;
	case C:
		operand = registers->c;
		break;
	case D:
		operand = registers->d;
		break;
	case E:
		operand = registers->e;
		break;
	case H:
		operand = registers->h;
		break;
	case L:
		operand = registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		operand = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_ADC recieved invaild target register!!");
		return;
	}

	uint8_t carry_uint8 = (registers->f.carry) ? 1 : 0;

	arithmeticResultu8 result = overflowing_add_u8(registers->a, operand,carry_uint8);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = false;
	registers->f.carry = result.didCarry;

	// if two lower nibbles of numbers being added add to more than 0xF
	
	registers->f.half_carry = ((registers->a & 0xF) + (operand & 0xF) + carry_uint8 )> 0xF;

	// set register A to the result at the end, as to not mess with half carry flag if targetRegister is also A
	registers->a = result.wrappedValue;
}

void CPU_SBC(CPU& cpu, ArithmeticTarget target)
{
	cpu.pc += 1;
	Registers* registers = &cpu.registers;
	uint8_t operand;
	switch (target) {
	case A:
		operand = registers->a;
		break;
	case B:
		operand = registers->b;
		break;
	case C:
		operand = registers->c;
		break;
	case D:
		operand = registers->d;
		break;
	case E:
		operand = registers->e;
		break;
	case H:
		operand = registers->h;
		break;
	case L:
		operand = registers->l;
		break;
	case HL_AS_ADDRESS:
		operand = MemoryBus_read_byte(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;
	case d8:
		operand = MemoryBus_read_byte(cpu.bus, cpu.pc);
		cpu.pc += 1;
		break;
	default:
		throw std::invalid_argument("CPU_SBC recieved invalid target register!!");
		return;
	}

	uint8_t carry_uint8 = (registers->f.carry) ? 1 : 0;

	arithmeticResultu8 result = underflowing_sub_u8(registers->a, operand, carry_uint8);


	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = true;
	registers->f.carry = result.didCarry;

	// if two lower nibbles of numbers being added add to more than 0xF

	registers->f.half_carry = ((uint16_t)(registers->a & 0xF) - 
		(uint16_t)((operand & 0xF) + 
			(uint16_t)carry_uint8)) < 0x0;

	// set register A to the result at the end, as to not mess with half carry flag if targetRegister is also A
	registers->a = result.wrappedValue;
}

void CPU_SCF(CPU& cpu)
{
	cpu.pc += 1;
	cpu.registers.f.carry = true;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
}

void CPU_LD(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2)
{
	// copy contents of t2 into t1
	if (!CPU_LD_VALIDATION(t1, t2)) { 
		throw std::invalid_argument("CPU_LD recieved invalid target(s)!"); 
		return;
	}


}

void CPU_LD_8BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2)
{
	throw std::exception("CPU_LD_8BIT is not implemented yet.");
}

void CPU_LD_16BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2)
{
	throw std::exception("CPU_LD_16BIT is not implemented yet.");
}



#pragma endregion


bool CPU_LD_VALIDATION(ArithmeticTarget t1, ArithmeticTarget t2)
{
	switch (t1) {
	case A:
		if(t2 == a16 || 
			t2 == a8 || 
			t2 == C_AS_LOWER_ADDRESS || 
			t2 == BC_AS_ADDRESS ||
			t2 == DE_AS_ADDRESS ||
			t2 == HL_POS ||
			t2 == HL_NEG) {	return true;}
	case B:
	case C:
	case D:
	case E:
	case H:
	case L:
	case HL_AS_ADDRESS:
		if (t2 == A ||
			t2 == B ||
			t2 == C ||
			t2 == D ||
			t2 == E ||
			t2 == H ||
			t2 == L ||
			t2 == HL_AS_ADDRESS ||
			t2 == d8
			) {
			return !(t1 == HL_AS_ADDRESS && t2 == HL_AS_ADDRESS);
		}
		else {
			return false;
		}
		break;


	case a16:
		if (t2 == SP) { return true; }
	case a8:
	case C_AS_LOWER_ADDRESS:
		return t2 == A;
		break;


	case HL:
		if (t2 == SP_PLUS_s8 || t2 == d16) { return true; }
		break;


	case SP:
		if (t2 == HL || t2 == d16) { return true; }
		break;


	case DE:
	case BC:
		return t2 == d16;
		break;


	default:
		return false;
		break;
	}

	return false;
}
