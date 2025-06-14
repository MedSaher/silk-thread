#include <string>
#include "header.hpp"

bool is_valid_command(const std::map<std::string, bool>* commands, const std::string& input) {
    for (const auto& [key, value] : *commands) {
        if (input.rfind(key, 0) == 0 && value) {
            return true; // input starts with key
        }
    }
    return false;
}