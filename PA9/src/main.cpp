#include "main.h"

int main(int argc, char** argv) {
	
	//If no arguments, use default (basic) config file
	if (argc == 1) {
		std::string newArgv = "config.json";
		argv[1] = new char[newArgv.size() + 1];
		argv[1][newArgv.size()] = 0;
		strncpy(argv[1], newArgv.c_str(), newArgv.size());
	}
	
	//Stores the properties of our engine, such as window name/size, fullscreen, and shader info
	Engine::Context ctx;
	ctx.gameWorldCtx = gameCtx;
	
	//Do command line arguments
	json config;
	int exit = processConfig(argc, argv, config, ctx);
	
	if (exit != -1) {
		return exit;
	}
	
	// Start an engine and run it then cleanup after
	Engine* engine = new Engine(ctx);
	if (!engine->Initialize()) {
		printf("The engine failed to start.\n");
		delete engine;
		engine = nullptr;
		return 1;
	}
	
	engine->Run();
	//If we changed window resolution, save it for next time
	if (config["window"]["width"] != engine->windowWidth || config["window"]["height"] != engine->windowHeight) {
		config["window"]["width"] = engine->windowWidth;
		config["window"]["height"] = engine->windowHeight;
		
		std::ofstream configFile(argv[1]);
		
		configFile << config.dump(4);
	}
	
	// If there were no arguments, delete the argument we created
	if (argc == 1) {
		delete[] argv[1];
		argv[1] = nullptr;
	}
	
	// memory clean-up
	for (int i = 0; (i < (ctx.gameWorldCtx->worldObjects.size())); i++) {
		delete ctx.gameWorldCtx->worldObjects[i];
		ctx.gameWorldCtx->worldObjects[i] = nullptr;
	}
	delete engine;
	engine = nullptr;
	delete ctx.physWorld;
	ctx.physWorld = nullptr;
	delete gameCtx;
	gameCtx = nullptr;
	
	return 0;
}

//Takes argc and argv from main and stuffs all the necessary information into ctx
int processConfig(int argc, char** argv, json& config, Engine::Context& ctx) {
	int error = -1;
	
	if (!strcmp(argv[1], "--help")) {
		helpMenu();
		return 0;
	} else {
		// Add the physics world (not defined in config)
		PhysicsWorld* physWorld = new PhysicsWorld();
		ctx.physWorld = physWorld;
		
		//Load and process config file
		ifstream configFile(argv[1]);
		if (!configFile.is_open()) {
			std::cout << "Could not open config file '" << argv[1] << "'" << std::endl;
			return 1;
		}
		
		config << configFile;
		
		//Window properties
		//I don't think fullscreen works yet - maybe eventually
		ctx.height = config["window"]["height"];
		ctx.width = config["window"]["width"];
		ctx.fullscreen = config["window"]["fullscreen"];
		ctx.name = "Pinball";
		
		//The configuration of the game world
		std::string vertexLocation = config["default_shaders"]["vertex"];
		std::string fragLocation = config["default_shaders"]["fragment"];
		Shader* defaultShader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
		
		//Load the gameworld's objects
		for (auto& i : config["game_objects"]) {
			Object::Context objCtx;
			error = loadObjectContext(i, objCtx, defaultShader, physWorld);
			if (error != -1) return error;
			Object* newObject = new Object(objCtx);
			gameCtx->worldObjects.push_back(newObject);
		}
		
		vector<Graphics::LightContext>* lights = new vector<Graphics::LightContext>();
		Graphics::LightContext lightCtx;
		for(auto& i : config["lights"]) {
			error = loadLightContext(i, lightCtx, gameCtx->worldObjects);
			if(error != -1) return error;
			lights->push_back(lightCtx);
		}
		if(lights->size() > 0) ctx.lights = lights;
		else                   ctx.lights = nullptr;
		
		if (error != -1) return error;
	}
	return -1;
}

int loadObjectContext(json& config, Object::Context& ctx, Shader* defaultShader, PhysicsWorld* physWorld) {
	
	PhysicsWorld::Context objectPhysics;
	if (config.find("name") != config.end()) {
		ctx.name = config["name"];
	}
	
	if (config.find("shape") != config.end()) {
		if (config["shape"] == "sphere") {
			ctx.shape = 1;
		} else if (config["shape"] == "box") {
			ctx.shape = 2;
		} else if (config["shape"] == "cylinder") {
			ctx.shape = 3;
		} else if (config["shape"] == "plane") {
			ctx.shape = 4;
		} else {
			ctx.shape = 0;
		}
		objectPhysics.shape = ctx.shape;
	}
	
	if (config.find("height") != config.end()) {
		objectPhysics.heightY = config["height"];
	}
	if (config.find("width") != config.end()) {
		objectPhysics.widthX = config["width"];
	}
	if (config.find("depth") != config.end()) {
		objectPhysics.lengthZ = config["depth"];
	}
	if (config.find("isKinematic") != config.end()) {
		objectPhysics.isKinematic = config["isKinematic"];
	}
	if (config.find("isBounceType") != config.end()) {
		objectPhysics.isBounceType = config["isBounceType"];
	}
	
	std::string filename;
	
	//Check if the object has a special starting location
	if (config.find("location") != config.end()) {
		ctx.zLoc = config["location"]["z"];
		ctx.xLoc = config["location"]["x"];
		ctx.yLoc = config["location"]["y"];
		objectPhysics.xLoc = ctx.xLoc;
		objectPhysics.yLoc = ctx.yLoc;
		objectPhysics.zLoc = ctx.zLoc;
	}
	
	if (config.find("mass") != config.end()) {
		ctx.mass = config["mass"];
		objectPhysics.mass = ctx.mass;
	}
	
	//Check if the object is a static or dynamic object
	if (config.find("isDynamic") != config.end()) {
		ctx.isDynamic = config["isDynamic"];
		objectPhysics.isDynamic = ctx.isDynamic;
	} else {
		ctx.isDynamic = false;
		objectPhysics.isDynamic = ctx.isDynamic;
	}
	
	if (config.find("radius") != config.end()) {
		objectPhysics.radius = config["radius"];
		ctx.scale = config["radius"];
	}
	
	if (config.find("scale") != config.end()) {
		objectPhysics.scale = config["scale"];
		ctx.scale = config["scale"];
	}
	
	if (config.find("isPaddle") != config.end()) {
		objectPhysics.isPaddle = config["isPaddle"];
	}
	
	if (config.find("model") != config.end()) {
		filename = config["model"];
		
		ctx.model = PhysicsModel::load("models/" + filename, physWorld, &objectPhysics);
		
		if (ctx.model == nullptr) {
			std::cout << ctx.name << " Could not load model file " << config["model"] << std::endl;
			return 1;
		}
		
		ctx.physicsBody = (*(physWorld->getLoadedBodies()))[ctx.model->getRigidBodyIndex()];
	} else if (ctx.name != "Game Surface") {
		std::cout << config["name"] << "Object has no model " << std::endl;
		return 1;
	}
	
	//Check if the object has a texture
	if (config.find("texture") != config.end()) {
		filename = config["texture"];
		ctx.texture = Texture::load("textures/" + filename);
	} else {
		ctx.texture = nullptr;
	}
	
	//Night-time/Alternative texture
	if (config.find("alt-texture") != config.end()) {
		filename = config["alt-texture"];
		ctx.altTexture = Texture::load("textures/" + filename);
	} else {
		ctx.altTexture = nullptr;
	}
	
	//Normal Map texture
	if (config.find("normal-texture") != config.end()) {
		filename = config["normal-texture"];
		ctx.normalMap = Texture::load("textures/" + filename);
	} else {
		ctx.normalMap = nullptr;
	}
	
	//Specular map texture
	if (config.find("specular-texture") != config.end()) {
		filename = config["specular-texture"];
		ctx.specularMap = Texture::load("textures/" + filename);
	} else {
		ctx.specularMap = nullptr;
	}
	
	//Check if the planet has a special shader
	if (config.find("shaders") != config.end()) {
		std::string vertexLocation = config["shaders"]["vertex"];
		std::string fragLocation = config["shaders"]["fragment"];
		ctx.shader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
	} else {
		ctx.shader = defaultShader;
	}
	
	
	if (config.find("mass") != config.end()) {
		ctx.mass = config["mass"];
	}
	
	return -1;
}

int loadLightContext(json &config, Graphics::LightContext &ctx, const std::vector<Object*>& objects) {
	if(config["type"] == "spot") {
		ctx.type = LIGHT_SPOT;
	} else if(config["type"] == "point"){
		ctx.type = LIGHT_POINT;
	} else {
		std::cerr << "Incorrect light in config file: Invalid Type \"" << config["type"] << "\"" << std::endl;
		return 1;
	}
	
	ctx.position.x = config["location"]["x"];
	ctx.position.y = config["location"]["y"];
	ctx.position.z = config["location"]["z"];
	
	if(ctx.type == LIGHT_SPOT) {
		if(config["pointingAt"].is_string()) {
			std::string name = config["pointingAt"];
			for(const auto& i : objects) {
				if(i->ctx.name == name) {
					ctx.pointing = &i->position;
					break;
				}
			}
		} else {
			glm::vec3* newPoint = new glm::vec3;
			newPoint->x = config["pointingAt"]["x"];
			newPoint->y = config["pointingAt"]["y"];
			newPoint->z = config["pointingAt"]["z"];
			
			ctx.pointing = newPoint;
		}
		
		ctx.angle = double(config["angle"]) * M_PI / 180;
	}
	
	ctx.strength = config["strength"];
	
	return -1;
}


//Displays command usage information to standard output
void helpMenu() {
	std::cout << "Command Usage:" << std::endl << std::endl
	          << "    " << PROGRAM_NAME << " --help" << std::endl
	          << "        Show help menu and command usage" << std::endl
	          << "    " << PROGRAM_NAME << " <filename>" << std::endl
	          << "        Run program with specified config file" << std::endl;
}