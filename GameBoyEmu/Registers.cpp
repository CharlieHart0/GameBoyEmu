#include "Registers.h"

// main registers

uint16_t Reg_get_16bit(Registers& registers,Reg_pair pair)
{
	uint8_t* reg1;
	uint8_t* reg2;
	switch (pair)
	{
	case af:
		reg1 = &registers.a;
		reg2 = (uint8_t*) &registers.f; //TODO: not sure if this cast is silly or not
		break;
	case bc:
		reg1 = &registers.b;
		reg2 = &registers.c;
		break;
	case de:
		reg1 = &registers.d;
		reg2 = &registers.e;
		break;
	case hl:
		reg1 = &registers.h;
		reg2 = &registers.l;
		break;
	default:
		throw std::invalid_argument("Reg_get_16bit recieved invald target register!!");
		break;
	}
	return (uint16_t)*reg1 << 8 
		| (uint16_t)*reg2;
}

void Reg_set_16bit(Registers& registers, Reg_pair pair, uint16_t value)
{
	uint8_t* reg1;
	uint8_t* reg2;
	switch (pair)
	{
	case af:
		reg1 = &registers.a;
		reg2 = (uint8_t*)&registers.f; //TODO: not sure if this cast is silly or not
		break;
	case bc:
		reg1 = &registers.b;
		reg2 = &registers.c;
		break;
	case de:
		reg1 = &registers.d;
		reg2 = &registers.e;
		break;
	case hl:
		reg1 = &registers.h;
		reg2 = &registers.l;
		break;
	default:
		throw std::invalid_argument("Reg_set_16bit recieved invald target register!!");
		break;
	}
	*reg1 = (uint8_t)((value & 0xFF00) >> 8);
	*reg2 = (uint8_t)(value & 0xFF);
}




