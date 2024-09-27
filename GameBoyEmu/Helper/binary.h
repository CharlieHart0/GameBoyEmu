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
			return pos > (sizeof(T)-1) ? false : (val >> pos) & 1;
		}
	}
}