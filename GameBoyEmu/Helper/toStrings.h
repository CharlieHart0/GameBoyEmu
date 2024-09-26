#pragma once
#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace toStrings
{
	std::string hexToString(uint16_t value, bool use0x = true);
	std::string hexToString(uint8_t value, bool use0x = true);
	std::string boolToString(bool value);
}