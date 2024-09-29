#pragma once

#include <string>
#include <vector>


namespace chhelper
{
    namespace stringfuncs
    {

        std::vector<std::string> split(std::string s, std::string delim)
        {
            std::vector<std::string> strings;
            if (!s.empty())
            {
                int start = 0;
                do
                {
                    int i = s.find(delim, start);
                    if (i == std::string::npos) break;

                    int length = i - start;
                    strings.push_back(s.substr(start, length));
                    start += (length + delim.size());
                } while (true);
                strings.push_back(s.substr(start));
            }
            return strings;
        }

        // returns true if character is 0-9, a-f or A-F
        bool isHexChar(char c)
        {
            if ((48 <= c) && (57 >= c)) return true;
            if ((65 <= c) && (70 >= c)) return true;
            if ((97 <= c) && (102 >= c)) return true;
            return false;
        }

        bool hasIllegalHexChars(std::string& input)
        {
            for (const auto& c : input)
            {
                if (!isHexChar(c))
                {
                    return true;
                }
            }
            return false;
        }
    }
}