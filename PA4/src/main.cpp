#include "main.h"

int main(int argc, char **argv) {
	//Stores the properties of our engine, such as window name/size, fullscreen, and shader info
	Engine::Context ctx;
	Object* sun;
	
	//Do command line arguments
	int exit = processConfig(argc, argv, ctx, sun);
	
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
	delete engine;
	engine = NULL;
	delete sun;
	sun = NULL;
	
	return 0;
}

//Takes argc and argv from main and stuffs all the necessary information into ctx
int processConfig(int argc, char **argv, Engine::Context &ctx, Object*& sun) {
	
	//If no arguments, tell user to look at help menu
	if (argc == 1) {
		std::cout << "No arguments found" << std::endl
		          << "Please run with --help for command reference" << std::endl;
		return 1;
	}
	
	int error = -1;
	
	if (!strcmp(argv[1], "--help")) {
		helpMenu();
		return 0;
	} else {
		ifstream configFile(argv[1]);
		if(!configFile.is_open()) {
			std::cout << "Could not open config file '" << argv[1] << "'" << std::endl;
			return 1;
		}
		
		json config;
		config << configFile;
		
		std::cout << config["shaders"]["vertex"];
		error = loadShader(config["shaders"]["vertex"], ctx.vertex);
		if (error != -1) return error;
		error = loadShader(config["shaders"]["fragment"], ctx.fragment);
		if (error != -1) return error;
		
		ctx.height = config["window"]["height"];
		ctx.width  = config["window"]["width"];
		ctx.fullscreen = config["window"]["fullscreen"];
		
		Object::Context sunCtx;
		
		error = loadPlanetContext(config["sun"], sunCtx, config["scale"]["distance"], config["scale"]["time"]);
		if (error != -1) return error;
		
		sun = new Object(sunCtx, NULL);
		
		error = loadPlanets(config["sun"], *sun, config["scale"]["distance"], config["scale"]["time"]);
		if(error != -1) return error;
	}
	
	if (ctx.vertex == "") {
		std::cout << "No -v option included" << std::endl;
		return 1;
	}
	if (ctx.fragment == "") {
		std::cout << "No -f option included" << std::endl;
		return 1;
	}
	
	return -1;
}

//Accepts name of file to load shader from and string reference to store into
int loadShader(const std::string &filename, std::string &shader) {
	std::ifstream shaderFile(filename);
	
	if (!shaderFile.is_open()) {
		std::cerr << "Could not find shader file: " << filename << std::endl;
		return 1;
	}
	
	shader = std::string(std::istreambuf_iterator<char>(shaderFile), {});
	
	return -1;
}

int loadPlanets(json& config, Object& sun, float spaceScale, float timeScale) {
	Object::Context ctx;
	int error;
	
	for(auto& i : config["satellites"]) {
		error = loadPlanetContext(i, ctx, spaceScale, timeScale);
		if(error != -1) return error;
		
		Object& newPlanet = sun.addChild(ctx);
		error = loadPlanets(i, newPlanet, spaceScale, timeScale);
		if(error != -1) return error;
	}
	
	return -1;
}

int loadPlanetContext(json& config, Object::Context& ctx, float spaceScale, float timeScale) {
	ctx.moveScale = ((float) config["orbit"]["year"]) / timeScale;
	ctx.spinScale = ((float) config["day"]) / timeScale;
	ctx.orbitDistance = ((float) config["orbit"]["distance"]) / spaceScale;
	ctx.scale = ((float) config["radius"]) / spaceScale;
	
	if(config["spins"] == "ccw") ctx.spinDir = 1;
	else ctx.spinDir = -1;
	
	ctx.name = config["name"];
	
	ctx.model = Model::load(config["model"]);
	if(ctx.model == NULL) {
		std::cout << "Could not load model file \"" << config["model"] << "\"" << std::endl;
		return 1;
	}
	
	return -1;
}


//Displays command usage information to standard output
void helpMenu() {
	std::cout << "Command Usage:" << std::endl << std::endl
	          << "    " << PROGRAM_NAME << " --help" << std::endl
	          << "    " << PROGRAM_NAME << " [options] <-f filename> <-v filename>" << std::endl << std::endl
	          << "Options" << std::endl
	          << "    --help                       Show help menu and command usage" << std::endl
	          << "    -v, --vertex <filename>      Specify where to load vertex shader from" << std::endl
	          << "    -f, --fragment <filename>    Specify where to load fragment shader from" << std::endl
	          << "    -h, --height <number>        Choose the window height" << std::endl
	          << "    -w, --width <number>         Choose the window width" << std::endl
	          << "    -n, --name <name>            Choose the window name" << std::endl;
}