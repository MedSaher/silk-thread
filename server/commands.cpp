#include <cstring>
#include <iostream>
#include <map>
#include "header.hpp"
std::map<std::string, bool>* SetCommands() {
    auto* commands = new std::map<std::string, bool>({
        {"ls", true}, // works
        {"cd", true}, // works
        {"rm", true}, // works
        {"pwd", true}, // works
        {"grep", true}, // works
        {"find", true}, // works
        {"touch", true}, // works
        {"mkdir", true}, // works
        {"echo", true}, // works
        {"dir", true},
        {"del", true},
        {"findstr", true},
        {"where", true},
        {"type nul > filename", true},
        {"clear", true}
    });
    return commands;
}