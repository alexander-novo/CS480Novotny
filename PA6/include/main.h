#include <iostream>
#include <unordered_map>

#include "imgui.h"
#include "json.h"

#include "engine.h"
#include "model.h"

using json = nlohmann::json;

#define PROGRAM_NAME "Tutorial"

int processConfig(int argc, char** argv, json& config, Engine::Context&, Object*& sun);
int loadPlanets(json& planetConfig, Object& sun, float spaceScale, float timeScale, Shader* defaultShader);
int loadPlanetContext(json& config, Object::Context& ctx, float spaceScale, float timeScale, float scaleMultiplier, Shader* defaultShader);
void helpMenu();