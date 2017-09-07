#include <iostream>
#include <unordered_map>

#include "engine.h"

typedef std::unordered_map<std::string, std::vector<std::string>> ArgMap;

#define PROGRAM_NAME "Tutorial"

int processArgs(int argc, char** argv, Engine::Context&);
int loadShader(const std::string&, std::string&);
void helpMenu();