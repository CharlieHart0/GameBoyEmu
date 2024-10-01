#pragma once
#include <cstdint>

namespace chhelper
{
	namespace binary
	{
		// get bit from value. Returns false for pos > sizeof(T)-1
		template<typename T>
		bool getBit(T val, unsigned int pos)
		{
			// TODO should i throw an error here? not sure
			return (pos > (sizeof(T) * 8) - 1) ? false : (val >> pos) & 1;
		}

		// set bit to 1 at pos in value
		template<typename T>
		void setBit(T& val, unsigned int pos)
		{
			if (pos > (sizeof(T) * 8) - 1) throw std::exception("bit position invalid!");
			val = (val | (1 << (pos)));
		}

		// set bit to 0 at pos in value
		template<typename T>
		void clearBit(T& val, unsigned int pos)
		{
			if (pos > (sizeof(T) * 8) - 1) throw std::exception("bit position invalid!");
			val = (val & (~(1 << (pos))));
		}

		// set bit to given value at pos 
		template<typename T>
		void assignBit(T& val, unsigned int pos, bool bit)
		{
			if (pos > (sizeof(T)*8) - 1) throw std::exception("bit position invalid!");
			if (bit) setBit<T>(val, pos);
			else clearBit<T>(val, pos);
		}
	}
}