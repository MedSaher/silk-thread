#ifndef LIBS
#define LIBS
#include <iostream>
#include <map>
#endif

#ifndef COMMANDS
#define COMMANDS

std::map<std::string, bool>* SetCommands();
bool is_valid_command(const std::map<std::string, bool>* commands, const std::string& input);

#endif