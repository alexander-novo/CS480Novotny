#include "main.h"


// Holds Game Data
GameWorld::ctx *gameCtx = new GameWorld::ctx;

int main(int argc, char** argv) {
	
	//If no arguments, use default (basic) config file
	if (argc == 1) {
		std::string newArgv = "config.json";
		argv[1] = new char[newArgv.size() + 1];
		argv[1][newArgv.size()] = 0;
		strncpy(argv[1], newArgv.c_str(), newArgv.size());
	}
	
	srand(time(nullptr));
	
	//Stores the properties of our engine, such as window name/size, fullscreen, and shader info
	Engine::Context ctx;
	ctx.gameWorldCtx = gameCtx;

	PhysicsWorld::game = gameCtx;
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
		
		vertexLocation = config["default_alt_shaders"]["vertex"];
		fragLocation = config["default_alt_shaders"]["fragment"];
		Shader* defaultAltShader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
		
		//Load the gameworld's objects
		int j = 0;
		int k = 0;
		for (auto& i : config["game_objects"]) {
			Object::Context objCtx;
			error = loadObjectContext(i, objCtx, defaultShader, defaultAltShader, physWorld);
			if (error != -1) return error;
			Object* newObject = new Object(objCtx);
			gameCtx->worldObjects.push_back(newObject);

			// Ball Indices
			// Assumes ordered 1-15, cue-ball last (16)

            // Doesn't work??!
            std::string findStr = "all";
			//if(objCtx.name.find(findStr))
            if(j > 0)
			{

				if(k < 7)
				{
//                    std::cout << objCtx.name << std::endl;
					gameCtx->ballSolids.push_back(j);
				}
				else if (k == 7)
				{
					gameCtx->eightBall = j;
				}
				else if (k > 7 && k <= 14)
				{
					gameCtx->ballStripes.push_back(j);
				}
				else if (k == 15)
				{
					gameCtx->cueBall = j;
				}

//				std::cout << objCtx.name << std::endl;
				k++;
			}
			j++;
		}

		// Set Initial Balls to not sunk and not out of bounds
		for (int i = 0; i<7; i++) {
			gameCtx->oobSolids[i] = false;
			gameCtx->oobStripes[i] = false;
			gameCtx->sunkSolids[i] = false;
			gameCtx->sunkStripes[i] = false;
		}

		
		vector<Graphics::LightContext*>* lights = new vector<Graphics::LightContext*>();
		for(auto& i : config["lights"]) {
			Graphics::LightContext* newCtx = new Graphics::LightContext;
			error = loadLightContext(i, *newCtx, gameCtx->worldObjects);
			lights->push_back(newCtx);
			if(error != -1) return error;
		}
		if(lights->size() > 0) ctx.lights = lights;
		else                   ctx.lights = nullptr;
		
		if (error != -1) return error;
	}
	return -1;
}

int loadObjectContext(json& config, Object::Context& ctx, Shader* defaultShader, Shader* defaultAltShader, PhysicsWorld* physWorld) {
	
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
	else
	{
		ctx.shape = 0;
		objectPhysics.shape = 0;
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
		ctx.isBounceType = objectPhysics.isBounceType;
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

	// Scaling in directions for Non-Spheres
	if (config.find("scaleXYZ") != config.end()) {
		ctx.scale.x = config["scaleXYZ"]["z"];
		ctx.scale.y = config["scaleXYZ"]["x"];
		ctx.scale.z = config["scaleXYZ"]["y"];
		objectPhysics.scaleX = ctx.scale.x;
		objectPhysics.scaleY = ctx.scale.y;
		objectPhysics.scaleZ = ctx.scale.z;
	}

	// Scaling in directions for Non-Spheres
	if (config.find("rotation") != config.end()) {
		objectPhysics.rotationX = config["rotation"]["x"];
		objectPhysics.rotationY = config["rotation"]["y"];
		objectPhysics.rotationZ = config["rotation"]["z"];
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
		ctx.scale.x = ctx.scale.y = ctx.scale.z = objectPhysics.radius;
	}

	// Scaling for spheres
	if (config.find("scale") != config.end()) {
		objectPhysics.scale = config["scale"];
		ctx.scale.x = ctx.scale.y = ctx.scale.z = config["scale"];
	}

	if (config.find("model") != config.end()) {
		filename = config["model"];
		
		ctx.model = Model::load("models/" + filename);
		
		if (ctx.model == nullptr) {
			std::cout << ctx.name << " Could not load model file " << config["model"] << std::endl;
			return 1;
		}
		btTriangleMesh* objTriMesh = new btTriangleMesh();
		Model* collisionMesh = ctx.model;
		if(config.find("collision-mesh") != config.end()) {
			filename = config["collision-mesh"];
			
			collisionMesh = Model::load("models/" + filename);
			ctx.shape = 0;
		}
		
		if(ctx.shape > 4 || ctx.shape <= 0)
		{
			for(const auto& m : collisionMesh->meshes) {
				for(int i = 0; i < m._indices.size() / 3; i++) {
					btVector3 triArray[3];
					for(int j = 0; j < 3; j++) {
						glm::vec3 position = m._vertices[m._indices[3 * i + j]].vertex;
						triArray[j] = btVector3(position.x*ctx.scale.x, position.y*ctx.scale.y, position.z*ctx.scale.z);
					}
					
					objTriMesh->addTriangle(triArray[0], triArray[1], triArray[2]);
				}
			}
		}
		
		ctx.rigidBodyIndex = physWorld->createObject(ctx.name, objTriMesh, &objectPhysics);
		
		ctx.physicsBody = (*(physWorld->getLoadedBodies()))[ctx.rigidBodyIndex];
	} else {
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
	
	if (config.find("isAlt") != config.end()) {
		ctx.isAlt = config["isAlt"];
	} else {
		ctx.isAlt = false;
	}
	
	if (config.find("alt_shaders") != config.end()) {
		std::string vertexLocation = config["alt_shaders"]["vertex"];
		std::string fragLocation = config["alt_shaders"]["fragment"];
		ctx.altShader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
	} else {
		ctx.altShader = defaultAltShader;
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
	
	if(config["color"].is_string()) {
		if(config["color"] == "rainbow") {
			ctx.isRainbow = true;
			ctx.timer = rand() %  360;
		}
	} else {
		ctx.isRainbow = false;
		
		ctx.color.x = double(config["color"]["r"]) / 255.0;
		ctx.color.y = double(config["color"]["g"]) / 255.0;
		ctx.color.z = double(config["color"]["b"]) / 255.0;
	}
	
	if(ctx.type == LIGHT_SPOT) {
		if(config["pointingAt"].is_string()) {
			std::string name = config["pointingAt"];
			for(const auto& i : objects) {
				if(i->ctx.name == name) {
					ctx.pointing = &i->position;
					if(!ctx.isRainbow) {
						ctx.isBumperLight = true;
						i->ctx.bumperLight = &ctx.timer;
					}
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

std::ostream& operator<<(std::ostream& stream, const glm::vec3 & vector) {
	stream << "(" << vector.x << "," << vector.y << "," << vector.z << ")";
	return stream;
}