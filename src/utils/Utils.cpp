#include "Utils.h"
#include <algorithm>
#include <cctype>

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string Utils::unescape(const std::string& input) {
    std::string output = input;
    size_t pos = 0;
    while ((pos = output.find("&amp;", pos)) != std::string::npos) {
        output.replace(pos, 5, "&");
        pos += 1;
    }
    return output;
}
