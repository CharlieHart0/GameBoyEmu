#include "CPU.h"
#include "Instructions.h"
#include "Arithmetic.h"

void CPU_step(CPU& cpu)
{
	uint8_t instruction_byte = MemoryBus_read_byte(cpu.bus, cpu.pc);
	
	if (instruction_byte == 0xCB) {
		cpu.pc += 1;
		instruction_byte = MemoryBus_read_byte(cpu.bus, cpu.pc);

		if (prefixedInstructions[instruction_byte].instruction != UNINITALISED) {
			CPU_excecute(cpu, prefixedInstructions[instruction_byte]);
		}
		else {
			char exceptionMessage[45];
			snprintf(exceptionMessage, 45, "Invalid prefixed instruction byte: 0xCB %X", instruction_byte);
			throw std::invalid_argument(exceptionMessage);
		}
	}

	if (eightBitInstructions[instruction_byte].instruction != UNINITALISED) {
		CPU_excecute(cpu, eightBitInstructions[instruction_byte]);
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
	case PUSH:
		CPU_PUSH(cpu, fullInstruction.op1);
		break;
	case POP:
		CPU_POP(cpu, fullInstruction.op1);
		break;
	case JR:
		CPU_JR(cpu, fullInstruction.op1);
		break;
	case JP:
		CPU_JP(cpu, fullInstruction.op1);
		break;
	case CALL:
		CPU_CALL(cpu, fullInstruction.op1);
		break;
	case RET:
		CPU_RET(cpu, fullInstruction.op1);
		break;
	case EI:
		CPU_EI(cpu);
		break;
	case DI:
		CPU_DI(cpu);
		break;
	case RST:
		CPU_RST(cpu, (int)fullInstruction.op1);
		break;
	case RETI:
		CPU_RETI(cpu);
		break;
	case RLCA:
		CPU_RLCA(cpu);
		break;
	case RLA:
		CPU_RLA(cpu);
		break;
	case RRCA:
		CPU_RRCA(cpu);
		break;
	case RRA:
		CPU_RRA(cpu);
		break;
	case DAA:
		CPU_DAA(cpu);
		break;
	case SET:
		CPU_SET(cpu, fullInstruction.op1, (int) fullInstruction.op2);
		break;
	case RES:
		CPU_RES(cpu, fullInstruction.op1, (int)fullInstruction.op2);
		break;
	case BIT:
		CPU_BIT(cpu, (int)fullInstruction.op1, fullInstruction.op2);
		break;


	default:
		throw std::invalid_argument("Invalid 8 bit instruction!");
		break;
	}
}



#pragma region CPU_INSTRUCTIONS


void CPU_ADD(CPU& cpu, ArithmeticTarget target, ArithmeticTarget t2)
{
	if (target == SP && t2 == s8) {
		CPU_ADD_0xE8(cpu);
		return;
	}

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
	arithmeticResultu8 result = add_u8(registers->a, operand);
	

	//set flag registers
	registers->f.zero = result.wrappedValue == 0;
	registers->f.subtract = false;
	registers->f.carry = result.didCarry;

	// if two lower nibbles of numbers being added add to more than 0xF
	registers->f.half_carry = (registers->a & 0xF) + (operand & 0xF) > 0xF;

	// set register A to the result at the end, as to not mess with half carry flag if targetRegister is also A
	registers->a = result.wrappedValue;
}

void CPU_ADD_0xE8(CPU& cpu)
{
	int8_t signedVal = MemoryBus_read_byte(cpu.bus, cpu.pc + 1);
	cpu.pc += 2;
	
	uint16_t result = 0;
	result += signedVal;
	result += cpu.sp;

	cpu.registers.f.subtract = false;
	cpu.registers.f.zero = 0;

	cpu.registers.f.half_carry = ((cpu.sp & 0x0F) + (signedVal & 0x0F)) > 0xF;
	cpu.registers.f.carry = ((cpu.sp & 0xFF) + signedVal) > 0xFF;

	cpu.sp = result;
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

	arithmeticResultu16 result = add_u16(hlValue, operand);

	
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
	
	arithmeticResultu8 result = add_u8(registers->a, operand);


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

	arithmeticResultu8 result = add_u8(registers->a, operand);


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
	arithmeticResultu8 result = add_u8(operand, 0x01);


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
	arithmeticResultu16 result = add_u16(operand, 0x01);


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

	arithmeticResultu8 result = sub_u8(operand, 1);


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
	arithmeticResultu16 result = sub_u16(operand, 0x01);


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

	arithmeticResultu8 result = add_u8_3vals(registers->a, operand,carry_uint8);


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

	arithmeticResultu8 result = sub_u8_3vals(registers->a, operand, carry_uint8);


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

	// let the other function handle the program counter

	switch (t1) {
	case BC:
	case DE:
	case HL:
	case SP:
	case a16:
		CPU_LD_16BIT(cpu, t1, t2);
		break;
	default:
		// must be a valid pair of 8 bit operands, else CPU_LD_VALIDATION would have failed
		CPU_LD_8BIT(cpu, t1, t2);
		break;
	}

}

void CPU_LD_8BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2)
{
	uint8_t* p1 = nullptr,
		* p2 = nullptr;

	p1 = (uint8_t*)ArTarget_To_Ptr(cpu, t1);
	p2 = (uint8_t*)ArTarget_To_Ptr(cpu, t2);

	*p1 = *p2;

	cpu.pc += 1;

	if (t2 == d8 || t2 == a8 || t1 == a8) {
		cpu.pc += 1;
	}

	if (t2 == HL_POS || t1 == HL_POS) {
		Reg_set_16bit(cpu.registers, hl, Reg_get_16bit(cpu.registers, hl) + 1);
	}

	if (t2 == HL_NEG || t1 == HL_NEG) {
		Reg_set_16bit(cpu.registers, hl, Reg_get_16bit(cpu.registers, hl) - 1);
	}
}

void CPU_LD_16BIT(CPU& cpu, ArithmeticTarget t1, ArithmeticTarget t2)
{
	sixteenBitValuePtrs source = sixteenBitValuePtrs();
	sixteenBitValuePtrs dest = sixteenBitValuePtrs();
	
	// only used if t2 == SP+s8, as intermediate stage is required
	uint16_t sp_plus_s8 = 0;

	if (t2 != SP_PLUS_s8) {
		source = ArTarget_To_Ptr_16b(cpu, t2);
	}
	else {
		sp_plus_s8 = cpu.sp;
		sp_plus_s8 += *(int8_t*)ArTarget_To_Ptr(cpu, s8);
		source = uint16_to_uint8_ptrs(&sp_plus_s8);
	}
	dest = ArTarget_To_Ptr_16b(cpu, t1);


	

	if (t1 == SP && t2 == HL) {
		cpu.pc += 1;
	}
	else if (t1 == HL && t2 == SP_PLUS_s8) {
		cpu.pc += 2;
	}
	else {
		cpu.pc += 3;
	}


	if (dest.lower == nullptr) {
		throw std::logic_error("CPU_LD_16BIT lower destination pointer was null!");
	}
	if (source.lower == nullptr) {
		throw std::logic_error("CPU_LD_16BIT lower source pointer was null!");
	}
	if (dest.higher == nullptr) {
		throw std::logic_error("CPU_LD_16BIT higher destination pointer was null!");
	}
	if (source.higher == nullptr) {
		throw std::logic_error("CPU_LD_16BIT higher source pointer was null!");
	}

	*dest.lower = *source.lower;
	*dest.higher = *source.higher;
}

void CPU_PUSH(CPU& cpu, ArithmeticTarget t1)
{
	cpu.pc += 1;
	uint8_t* sourceLower = nullptr;
	uint8_t* sourceHigher = nullptr;
	switch (t1) {
	case BC:
		sourceLower = &cpu.registers.b;
		sourceHigher = &cpu.registers.c;
		break;
	case DE:
		sourceLower = &cpu.registers.d;
		sourceHigher = &cpu.registers.e;
		break;
	case HL:
		sourceLower = &cpu.registers.h;
		sourceHigher = &cpu.registers.l;
		break;
	case AF:
		sourceLower = &cpu.registers.a;
		sourceHigher = (uint8_t*)&cpu.registers.f;
		break;
	default:
		throw std::invalid_argument("CPU_PUSH recieved invalid target register pair!!");
		return;
	}

	cpu.sp -= 2;

	MemoryBus_write_byte(cpu.bus,cpu.sp+1 , *sourceHigher);
	MemoryBus_write_byte(cpu.bus, cpu.sp, *sourceLower);
}

void CPU_POP(CPU& cpu, ArithmeticTarget t1)
{
	cpu.pc += 1;
	uint8_t* destLower = nullptr;
	uint8_t* destHigher = nullptr;
	switch (t1) {
	case BC:
		destLower = &cpu.registers.b;
		destHigher = &cpu.registers.c;
		break;
	case DE:
		destLower = &cpu.registers.d;
		destHigher = &cpu.registers.e;
		break;
	case HL:
		destLower = &cpu.registers.h;
		destHigher = &cpu.registers.l;
		break;
	case AF:
		destLower = &cpu.registers.a;
		destHigher = (uint8_t*)&cpu.registers.f;
		break;
	default:
		throw std::invalid_argument("CPU_POP recieved invalid target register pair!!");
		return;
	}

	

	*destLower = MemoryBus_read_byte(cpu.bus, cpu.sp);
	cpu.sp += 1;
	*destHigher = MemoryBus_read_byte(cpu.bus, cpu.sp);
	cpu.sp += 1;
	
}

void CPU_JR(CPU& cpu, ArithmeticTarget target)
{
	bool doJump = false;

	switch (target) {
	case s8:
		doJump = true;
		break;
	case NZ:
		doJump = !cpu.registers.f.zero;
		break;
	case NC:
		doJump = !cpu.registers.f.carry;
		break;
	case C:
		doJump = cpu.registers.f.carry;
		break;
	case Z:
		doJump = cpu.registers.f.zero;
		break;
	default:
		throw std::invalid_argument("CPU_JR recieved invalid operand!!");
		return;
	}

	if (!doJump) {
		cpu.pc += 2;
		return;
	}

	int8_t signedVal = MemoryBus_read_byte(cpu.bus, cpu.pc + 1);
	cpu.pc += 2;
	cpu.pc += signedVal;
}

void CPU_JP(CPU& cpu, ArithmeticTarget target)
{
	bool doJump = false;
	uint8_t lower = 0;
	uint8_t higher = 0;

	switch (target) {
	case a16:
		doJump = true;
		break;
	case NZ:
		doJump = !cpu.registers.f.zero;
		break;
	case NC:
		doJump = !cpu.registers.f.carry;
		break;
	case C:
		doJump = cpu.registers.f.carry;
		break;
	case Z:
		doJump = cpu.registers.f.zero;
		break;
	case HL:
		doJump = true;
		break;
	default:
		throw std::invalid_argument("CPU_JP recieved invalid operand!!");
		return;
	}

	

	if (target == HL) {
		lower = cpu.registers.h;
		higher = cpu.registers.l;
		cpu.pc += 1;
	}
	else {
		lower = MemoryBus_read_byte(cpu.bus, cpu.pc + 1);
		higher = MemoryBus_read_byte(cpu.bus, cpu.pc + 2);
		cpu.pc += 3;
	}
	
	if (!doJump) { return; }

	cpu.sp = (uint16_t)lower + ((uint16_t)higher << 8);
}

void CPU_CALL(CPU& cpu, ArithmeticTarget target)
{
	bool doJump = false;

	uint16_t address =( ( (uint16_t)MemoryBus_read_byte(cpu.bus,cpu.pc + 2) ) << 8) 
		+(uint16_t)MemoryBus_read_byte(cpu.bus,cpu.pc + 1);

	switch (target) {
	case a16:
		doJump = true;
		break;
	case NZ:
		doJump = !cpu.registers.f.zero;
		break;
	case NC:
		doJump = !cpu.registers.f.carry;
		break;
	case C:
		doJump = cpu.registers.f.carry;
		break;
	case Z:
		doJump = cpu.registers.f.zero;
		break;
	default:
		throw std::invalid_argument("CPU_CALL recieved invalid operand!!");
		return;
	}

	cpu.pc += 3;

	if (!doJump) { return; }

	cpu.sp -= 2;

	MemoryBus_write_byte(cpu.bus, cpu.sp+1,(uint8_t)(cpu.pc >> 8));
	MemoryBus_write_byte(cpu.bus, cpu.sp, (uint8_t)(cpu.pc & 0xFF));

	cpu.pc = address;

}

void CPU_RET(CPU& cpu, ArithmeticTarget target)
{
	bool doJump = false;

	switch (target) {
		// invalid is default operand  (for RET 0xC9, which has no operand)
	case INVALID:
		doJump = true;
		break;
	case NZ:
		doJump = !cpu.registers.f.zero;
		break;
	case NC:
		doJump = !cpu.registers.f.carry;
		break;
	case C:
		doJump = cpu.registers.f.carry;
		break;
	case Z:
		doJump = cpu.registers.f.zero;
		break;
	default:
		throw std::invalid_argument("CPU_RET recieved invalid operand!!");
		return;
	}

	if (!doJump) {
		cpu.pc += 1;
		return;
	}

	// pop from stack

	uint8_t lower = MemoryBus_read_byte(cpu.bus, cpu.sp);
	cpu.sp += 1;
	uint8_t higher = MemoryBus_read_byte(cpu.bus, cpu.sp);
	cpu.sp += 1;

	
	cpu.pc = ((uint16_t)higher << 8) + (uint16_t)lower;
}

void CPU_EI(CPU& cpu)
{
	cpu.pc += 1;
	cpu.ime = true;
}

void CPU_DI(CPU& cpu)
{
	cpu.pc += 1;
	cpu.ime = false;
}

void CPU_RST(CPU& cpu, int value)
{

	if (value < 0 || value > 7) {
		throw std::invalid_argument("CPU_RST recieved invalid value!");
		return;
	}
	cpu.pc += 1;
	cpu.sp -= 2;

	MemoryBus_write_byte(cpu.bus, cpu.sp + 1, (uint8_t)(cpu.pc >> 8));
	MemoryBus_write_byte(cpu.bus, cpu.sp, (uint8_t)(cpu.pc & 0xFF));

	cpu.pc = (uint16_t)(value * 8);
}

void CPU_RETI(CPU& cpu)
{
	cpu.pc += 1;
	cpu.ime = true;
	cpu.pc = (((uint16_t)MemoryBus_read_byte(cpu.bus, cpu.sp+1)) << 8) |
		(uint16_t)MemoryBus_read_byte(cpu.bus, cpu.sp);
	cpu.sp += 2;
}

void CPU_RLCA(CPU& cpu)
{
	uint8_t carried = 0;
	cpu.pc += 1;

	carried = cpu.registers.a & 0b10000000;
	carried >> 7;

	cpu.registers.a << 1;
	cpu.registers.a += carried;

	cpu.registers.f.zero = false;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
	cpu.registers.f.carry = (bool)carried;

}

void CPU_RLA(CPU& cpu)
{
	uint8_t carried = 0;
	cpu.pc += 1;

	carried = cpu.registers.a & 0b10000000;
	carried >> 7;

	cpu.registers.a << 1;
	if(cpu.registers.f.carry){ cpu.registers.a += 0x01; }
	

	cpu.registers.f.zero = false;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
	cpu.registers.f.carry = (bool)carried;
}

void CPU_RRCA(CPU& cpu)
{
	uint8_t carried = 0;
	cpu.pc += 1;

	carried = cpu.registers.a & 0b00000001;

	cpu.registers.a >> 1;
	cpu.registers.a += carried;

	cpu.registers.f.zero = false;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
	cpu.registers.f.carry = (bool)carried;
}

void CPU_RRA(CPU& cpu)
{
	uint8_t carried = 0;
	cpu.pc += 1;

	carried = cpu.registers.a & 0b00000001;

	cpu.registers.a >> 1;
	if (cpu.registers.f.carry) { cpu.registers.a += 0x01; }

	cpu.registers.f.zero = false;
	cpu.registers.f.subtract = false;
	cpu.registers.f.half_carry = false;
	cpu.registers.f.carry = (bool)carried;
}

void CPU_DAA(CPU& cpu)
{
	// oh boy this one is tricky

	// something something Binary Coded Decimal
	//  0101 (5) 1001 (9)   0101 1001 is 59 in BCD

	// 6 is added or subtracted to the higher and lower nibbles to turn a BCD value to binary? i think?
	// ngl this one kinda makes no sense but hey if it works it works right?

	cpu.pc += 1;

	uint16_t aTemp = (uint16_t)cpu.registers.a;


	if (cpu.registers.f.half_carry || (cpu.registers.a & 0x0F) > 0x09) {
		if (cpu.registers.f.subtract) {
			aTemp -= 0x06;
		}
		else {
			aTemp += 0x06;
		}
	}

	if (cpu.registers.f.carry || ((cpu.registers.a & 0xF0) >> 4) > 0x09) {
		if (cpu.registers.f.subtract) {
			aTemp -= 0x60;
		}
		else {
			aTemp += 0x60;
		}
	}


	cpu.registers.f.zero = aTemp == 0;
	cpu.registers.f.half_carry = 0;
	cpu.registers.f.carry = cpu.registers.f.carry || aTemp > 0xFF;

	cpu.registers.a = (uint8_t)aTemp & 0xFF;
}

#pragma region prefixed instructions

void CPU_SET(CPU& cpu, ArithmeticTarget target, int value)
{
	cpu.pc += 2;
	uint8_t* bytePointer = nullptr;

	if (value < 0 || value > 7) {
		throw std::invalid_argument("CPU_SET recieved invalid value!");
		return;
	}

	switch (target) {
	case A:
		bytePointer = &cpu.registers.a;
		break;
	case B:
		bytePointer = &cpu.registers.b;
		break;
	case C:
		bytePointer = &cpu.registers.c;
		break;
	case D:
		bytePointer = &cpu.registers.d;
		break;
	case E:
		bytePointer = &cpu.registers.e;
		break;
	case H:
		bytePointer = &cpu.registers.h;
		break;
	case L:
		bytePointer = &cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		bytePointer = (uint8_t*)MemoryBus_get_ptr(cpu.bus,Reg_get_16bit(cpu.registers,hl));
		break;


	default:
		throw std::invalid_argument("CPU_SET recieved invalid target!");
		return;
	}

	if (((*bytePointer >> value) & 0x01)) {
		return;
	}

	*bytePointer += (1 << value);
}

void CPU_RES(CPU& cpu, ArithmeticTarget target, int value)
{
	cpu.pc += 2;
	uint8_t* bytePointer = nullptr;

	if (value < 0 || value > 7) {
		throw std::invalid_argument("CPU_RES recieved invalid value!");
		return;
	}

	switch (target) {
	case A:
		bytePointer = &cpu.registers.a;
		break;
	case B:
		bytePointer = &cpu.registers.b;
		break;
	case C:
		bytePointer = &cpu.registers.c;
		break;
	case D:
		bytePointer = &cpu.registers.d;
		break;
	case E:
		bytePointer = &cpu.registers.e;
		break;
	case H:
		bytePointer = &cpu.registers.h;
		break;
	case L:
		bytePointer = &cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		bytePointer = (uint8_t*)MemoryBus_get_ptr(cpu.bus, Reg_get_16bit(cpu.registers, hl));
		break;


	default:
		throw std::invalid_argument("CPU_RES recieved invalid target!");
		return;
	}

	if (!((*bytePointer >> value) & 0x01)) {
		return;
	}

	*bytePointer -= (1 << value);
}

void CPU_BIT(CPU& cpu, int value, ArithmeticTarget target)
{
	cpu.pc += 2;
	uint8_t byte = 0;

	if (value < 0 || value > 7) {
		throw std::invalid_argument("CPU_BIT recieved invalid value!");
		return;
	}

	switch (target) {
	case A:
		byte = cpu.registers.a;
		break;
	case B:
		byte = cpu.registers.b;
		break;
	case C:
		byte = cpu.registers.c;
		break;
	case D:
		byte = cpu.registers.d;
		break;
	case E:
		byte = cpu.registers.e;
		break;
	case H:
		byte = cpu.registers.h;
		break;
	case L:
		byte = cpu.registers.l;
		break;
	case HL_AS_ADDRESS:
		byte = Reg_get_16bit(cpu.registers, hl);
		break;


	default:
		throw std::invalid_argument("CPU_BIT recieved invalid target!");
		return;
	}

	byte >> value;

	byte &= 0x01;

	cpu.registers.f.zero = byte == 0;
	cpu.registers.f.half_carry = 1;
	cpu.registers.f.subtract = 0;
}

#pragma endregion

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

void* ArTarget_To_Ptr(CPU& cpu, ArithmeticTarget target)
{
	switch (target) {
	case A:
		return &cpu.registers.a;
	case B:
		return &cpu.registers.b;
	case C:
		return &cpu.registers.c;
	case D:
		return &cpu.registers.d;
	case E:
		return &cpu.registers.e;
	case H:
		return &cpu.registers.h;
	case L:
		return &cpu.registers.l;
	case HL_NEG:
	case HL_POS:
	case HL_AS_ADDRESS:
		return MemoryBus_get_ptr(cpu.bus, Reg_get_16bit(cpu.registers, hl));
	case DE_AS_ADDRESS:
		return MemoryBus_get_ptr(cpu.bus, Reg_get_16bit(cpu.registers, de));
	case BC_AS_ADDRESS:
		return MemoryBus_get_ptr(cpu.bus, Reg_get_16bit(cpu.registers, bc));
	case C_AS_LOWER_ADDRESS:
		return MemoryBus_get_ptr(cpu.bus, 0xFF00 + cpu.registers.c);
	case a8:
		return MemoryBus_get_ptr(cpu.bus,  0xFF00 + MemoryBus_read_byte(cpu.bus, cpu.pc + 1));
	case d8:
		return (uint8_t*)MemoryBus_get_ptr(cpu.bus, cpu.pc + 1);
	case s8:
		return (int8_t*)MemoryBus_get_ptr(cpu.bus, cpu.pc + 1);
	default:
		throw std::invalid_argument("Cannot get pointer for invalid Arithmetic Target!");
	}
}

sixteenBitValuePtrs ArTarget_To_Ptr_16b(CPU& cpu, ArithmeticTarget target)
{
	sixteenBitValuePtrs result;
	switch (target) {
	case DE:
		result.lower = &cpu.registers.d;
		result.higher = &cpu.registers.e;
		break;
	case BC:
		result.lower = &cpu.registers.b;
		result.higher = &cpu.registers.c;
		break;
	case HL:
		result.lower = &cpu.registers.h;
		result.higher = &cpu.registers.l;
		break;
	case SP:
		result = uint16_to_uint8_ptrs(&cpu.sp);
		break;
	case a16:
		result.lower = (uint8_t*)MemoryBus_get_ptr(cpu.bus,
			MemoryBus_read_byte(cpu.bus, cpu.pc + 0x0001));
		result.higher = (uint8_t*)MemoryBus_get_ptr(cpu.bus,
			MemoryBus_read_byte(cpu.bus, cpu.pc + 0x0002));
		break;
	case d16:
		result.lower = (uint8_t*)MemoryBus_get_ptr(cpu.bus, cpu.pc + 0x0001);
		result.higher = (uint8_t*)MemoryBus_get_ptr(cpu.bus, cpu.pc + 0x0002);
		break;

	default:
		throw std::invalid_argument("ArTarget_To_Ptr16 recieved invalid target!");	
	}

	return result;
}

sixteenBitValuePtrs uint16_to_uint8_ptrs(uint16_t* ptrIn)
{
	sixteenBitValuePtrs result;
	result.lower = (uint8_t*)ptrIn; //TODO not sure if this actually works tbh
	result.higher = (uint8_t*)ptrIn + 1;
	return result;
}
