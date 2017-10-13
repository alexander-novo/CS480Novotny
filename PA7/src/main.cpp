#include "main.h"

int main(int argc, char **argv) {
	
	//If no arguments, use default (basic) config file
	if (argc == 1) {
		std::string newArgv = "config.json";
		argv[1] = new char[newArgv.size()+1]; 
		argv[1][newArgv.size()] = 0; 
		strncpy(argv[1], newArgv.c_str(), newArgv.size());	
	}

	//Stores the properties of our engine, such as window name/size, fullscreen, and shader info
	Engine::Context ctx;
	Object *sun;
	
	//Do command line arguments
	json config;
	int exit = processConfig(argc, argv, config, ctx, sun);
	
	if (exit != -1) {
		return exit;
	}
	
	// Start an engine and run it then cleanup after
	Engine *engine = new Engine(ctx, sun);
	if (!engine->Initialize()) {
		printf("The engine failed to start.\n");
		delete engine;
		engine = NULL;
		return 1;
	}
	
	engine->Run();
	
	//If we changed window resolution, save it for next time
	if(config["window"]["width"] != engine->windowWidth || config["window"]["height"] != engine->windowHeight) {
		config["window"]["width"] = engine->windowWidth;
		config["window"]["height"] = engine->windowHeight;
		
		std::ofstream configFile(argv[1]);
		
		config >> configFile;
	}

	// If there were no arguments, delete the argument we created
	if (argc == 1)
	{
		delete [] argv[1];
		argv[1] = NULL;
	}
	
	delete engine;
	engine = NULL;
	delete sun;
	sun = NULL;
	
	return 0;
}

//Takes argc and argv from main and stuffs all the necessary information into ctx
int processConfig(int argc, char **argv, json& config, Engine::Context &ctx, Object *&sun) {
	
	int error = -1;
	
	if (!strcmp(argv[1], "--help")) {
		helpMenu();
		return 0;
	} else {
		//Load and process config file
		ifstream configFile(argv[1]);
		if (!configFile.is_open()) {
			std::cout << "Could not open config file '" << argv[1] << "'" << std::endl;
			return 1;
		}
		
		config << configFile;
		
		std::string vertexLocation = config["shaders"]["vertex"];
		std::string fragLocation = config["shaders"]["fragment"];
		Shader* defaultShader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
		
		//Window properties
		//I don't think fullscreen works yet - maybe eventually
		ctx.height = config["window"]["height"];
		ctx.width = config["window"]["width"];
		ctx.fullscreen = config["window"]["fullscreen"];
		
		ctx.lightStrength = config["sunlight"];
		
		Object::orbitShader = Shader::load("shaders/vert_orbits", "shaders/frag_orbits");
		
		Object::Context sunCtx;
		
		//Load the sun's properties
		error = loadPlanetContext(config["sun"], sunCtx, config["scale"]["distance"], config["scale"]["time"], 0, defaultShader);
		if (error != -1) return error;
		
		//make certain we set it to a light source
		sunCtx.isLightSource = true;
		sunCtx.scaleMultiplier = sunCtx.scale;
		
		sun = new Object(sunCtx, NULL);
		
		//Now load all the rest of the planets
		error = loadPlanets(config["sun"], *sun, config["scale"]["distance"], config["scale"]["time"], defaultShader);
		if (error != -1) return error;

	}
	
	return -1;
}

int loadPlanets(json &config, Object &sun, float spaceScale, float timeScale, Shader* defaultShader) {
	Object::Context ctx;
	int error;
	
	for (auto &i : config["satellites"]) {
		error = loadPlanetContext(i, ctx, spaceScale, timeScale, sun.ctx.scaleMultiplier, defaultShader);
		if (error != -1) return error;
		
		Object &newPlanet = sun.addChild(ctx);
		error = loadPlanets(i, newPlanet, spaceScale, timeScale, defaultShader);
		if (error != -1) return error;
	}
	
	return -1;
}

int loadPlanetContext(json &config, Object::Context &ctx, float spaceScale, float timeScale, float scaleMultiplier, Shader* defaultShader) {
	ctx.name = config["name"];

	if(config["orbit"]["year"] != 0) ctx.moveScale = timeScale / ((float) config["orbit"]["year"]);
	else ctx.moveScale = 0;
	if(config["day"] != 0) ctx.spinScale = timeScale / ((float) config["day"]);
	else ctx.spinScale = 0;
	
	ctx.orbitTilt = tan(M_PI * ((float) config["orbit"]["tilt"]) / 180);
	ctx.axisTilt = M_PI * ((float) config["axial-tilt"]) / 180;
	
	ctx.orbitDistance = ((float) config["orbit"]["distance"]) / spaceScale;
	ctx.scale = ((float) config["radius"]) / spaceScale;
	
	ctx.scaleMultiplier = scaleMultiplier;
	
	if (config["spins"] == "ccw") ctx.spinDir = 1;
	else ctx.spinDir = -1;
	
	std::string filename = config["model"];
	ctx.model = Model::load("models/" + filename);
	if (ctx.model == NULL) {
		std::cout << "Could not load model file " << config["model"] << std::endl;
		return 1;
	}
	
	//Check if the planet has a texture
	if(config.find("texture") != config.end()) {
		filename = config["texture"];
		ctx.texture = Texture::load("textures/" + filename);
	} else {
		ctx.texture = nullptr;
	}
	
	if(config.find("alt-texture") != config.end()) {
		filename = config["alt-texture"];
		ctx.altTexture = Texture::load("textures/" + filename);
	} else {
		ctx.altTexture = nullptr;
	}
	
	if(config.find("normal-texture") != config.end()) {
		filename = config["normal-texture"];
		ctx.normalMap = Texture::load("textures/" + filename);
	} else {
		ctx.normalMap = nullptr;
	}
	
	if(config.find("specular-texture") != config.end()) {
		filename = config["specular-texture"];
		ctx.specularMap = Texture::load("textures/" + filename);
	} else {
		ctx.specularMap = nullptr;
	}
	
	//Check if the planet has a special shader
	if(config.find("shaders") != config.end()) {
		std::string vertexLocation = config["shaders"]["vertex"];
		std::string fragLocation = config["shaders"]["fragment"];
		ctx.shader = Shader::load("shaders/" + vertexLocation, "shaders/" + fragLocation);
	} else {
		ctx.shader = defaultShader;
	}

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