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
    }
}