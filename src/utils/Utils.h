#pragma once
#include <string>

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::string unescape(const std::string& input);
};
