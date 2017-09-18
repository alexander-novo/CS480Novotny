#include <iostream>
#include <unordered_map>

#include "imgui.h"
#include "json.h"

#include "engine.h"

typedef std::unordered_map<std::string, std::vector<std::string>> ArgMap;

using json = nlohmann::json;

#define PROGRAM_NAME "Tutorial"

int processConfig(int argc, char** argv, Engine::Context&, Object*& sun);
int loadShader(const std::string&, std::string&);
int loadPlanets(json& planetConfig, Object& sun, float spaceScale, float timeScale);
int loadPlanetContext(json& config, Object::Context& ctx, float spaceScale, float timeScale);
void helpMenu();