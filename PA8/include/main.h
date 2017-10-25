#include <iostream>
#include <unordered_map>

#include <btBulletDynamicsCommon.h>

#include "imgui.h"
#include "json.h"

#include "engine.h"
#include "physics_model.h"
#include "physics_world.h"

using json = nlohmann::json;

#define PROGRAM_NAME "Tutorial"

//Take all the information in the config file, and stuff it into where it needs to go
int processConfig(int argc, char** argv, json& config, Engine::Context&, Object*& sun);
//Start loading planets from the config file
int loadPlanets(json& planetConfig, Object& sun, float spaceScale, float timeScale, Shader* defaultShader, PhysicsWorld *physWorld);
//Load an individual planet's data
int loadPlanetContext(json& config, Object::Context& ctx, float spaceScale, float timeScale, float scaleMultiplier, Shader* defaultShader, PhysicsWorld *physWorld);
//Display help menu
void helpMenu();