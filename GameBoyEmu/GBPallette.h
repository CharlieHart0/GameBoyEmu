#pragma once
#include <cstdint>
#include <cstring>
#include <array>

class GBPalette
{
public:
	std::array<uint8_t, 4> colours[4];
	GBPalette(std::array<uint8_t,4> col1, std::array<uint8_t, 4> col2, std::array<uint8_t, 4> col3, std::array<uint8_t, 4> col4)
	{
		colours[0] = col1;
		colours[1] = col2;
		colours[2] = col3;
		colours[3] = col4;
	}
};

