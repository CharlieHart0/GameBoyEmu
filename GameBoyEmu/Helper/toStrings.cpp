#include "toStrings.h"

namespace toStrings
{
    std::string hexToString(uint16_t value, bool use0x)
    {
        std::stringstream ss;
        if (use0x) ss << "0x";
        ss << std::setw(4) << std::setfill('0') << std::hex << (int) value;
        return ss.str();
    }

    std::string hexToString(uint8_t value, bool use0x)
    {
        std::stringstream ss;
        if (use0x) ss << "0x";
        ss << std::setw(2) << std::setfill('0') << std::hex << (int) value;
        return ss.str();
    }

    std::string boolToString(bool value)
    {
        if (value) return "TRUE";
        return "FALSE";
    }
}