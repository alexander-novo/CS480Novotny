#include <iostream>
#include <vector>
#include <unordered_map>
#include <btBulletDynamicsCommon.h>
#include "physics_world.h"
#include "imgui.h"
#include "json.h"
#include "engine.h"
#include "model.h"
#include "gameworldctx.h"
#include "graphics_headers.h"

using json = nlohmann::json;

#define PROGRAM_NAME "Tutorial"

//Take all the information in the config file, and stuff it into where it needs to go
int processConfig(int argc, char **argv, json& config, Engine::Context &ctx);
//Load an object's data
int loadObjectContext(json &config, Object::Context &ctx, Shader* defaultShader, Shader* defaultAltShader, PhysicsWorld *physWorld);
int loadLightContext(json &config, Graphics::LightContext &ctx, const std::vector<Object*>& objects);
//Display help menu
void helpMenu();