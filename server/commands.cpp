#include <cstring>
#include <iostream>
#include <map>
#include "header.hpp"
std::map<std::string, bool>* SetCommands() {
    auto* commands = new std::map<std::string, bool>({
        {"ls", true},
        {"cd", true},
        {"rm", true},
        {"pwd", true},
        {"grep", true},
        {"find", true},
        {"touch", true},
        {"mkdir", true},
        {"dir", true},
        {"del", true},
        {"findstr", true},
        {"where", true},
        {"type nul > filename", true}
    });
    return commands;
}