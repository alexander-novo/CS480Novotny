#include "main.h"

int main(int argc, char **argv)
{
  //Stores the properties of our engine, such as window name/size, fullscreen, and shader info
  Engine::Context ctx;

  //Do command line arguments
  int exit = processArgs(argc, argv, ctx);

  if(exit != -1) {
    return exit;
  }

  // Start an engine and run it then cleanup after
  Engine *engine = new Engine(ctx);
  if(!engine->Initialize())
  {
    printf("The engine failed to start.\n");
    delete engine;
    engine = NULL;
    return 1;
  }
  engine->Run();
  delete engine;
  engine = NULL;
  return 0;
}

//Takes argc and argv from main and stuffs all the necessary information into ctx
int processArgs(int argc, char** argv, Engine::Context& ctx) {
  int argIndex = 1;
  int error;

  //If no arguments, tell user to look at help menu
  if(argc == 1) {
    std::cout << "No arguments found" << std::endl
              << "Please run with --help for command reference" << std::endl;
    return 1;
  }

  //Loop through arguments and process
  for(; argIndex < argc; argIndex++) {
    //HELP MENU
    if(!strcmp(argv[argIndex],"--help")) {
      helpMenu();
      return 0;
    } 
    //VERTEX SHADER FILE
    else if (!strcmp(argv[argIndex], "-v") || !strcmp(argv[argIndex], "--vertex")) {
      //Make certain this isn't the last argument
      //The next argument should be the filename
      if(++argIndex < argc) {
        error = loadShader(argv[argIndex], ctx.vertex);
        if(error != -1) return error;
      } else {
        std::cout << "Expected argument for -v";
        return 1;
      }
      continue;
    } 
    //FRAGMENT SHADER FILE
    else if (!strcmp(argv[argIndex], "-f") || !strcmp(argv[argIndex], "--fragment")) {
      //Make certain this isn't the last argument
      //The next argument should be the filename
      if(++argIndex < argc) {
        error = loadShader(argv[argIndex], ctx.fragment);
        if(error != -1) return error;
      } else {
        std::cout << "Expected argument for -f";
        return 1;
      }
      continue;
    }
    //WINDOW HEIGHT
    else if (!strcmp(argv[argIndex], "-h") || !strcmp(argv[argIndex], "--height")) {
      //Make certain this isn't the last argument
      //The next argument should be the window height
      if(++argIndex < argc) {
        
        ctx.height = strtol(argv[argIndex], nullptr, 10);

        if(ctx.height < 1) {
          std::cout << "Value '" << argv[argIndex] <<"' not a valid window height" << std::endl;
          return 1;
        }

      } else {
        std::cout << "Expected argument for -h";
        return 1;
      }
      continue;
    }
    //WINDOW WIDTH
    else if (!strcmp(argv[argIndex], "-w") || !strcmp(argv[argIndex], "--width")) {
      //Make certain this isn't the last argument
      //The next argument should be the window width
      if(++argIndex < argc) {

        ctx.width = strtol(argv[argIndex], nullptr, 10);

        if(ctx.height < 1) {
          std::cout << "Value '" << argv[argIndex] <<"' not a valid window width" << std::endl;
          return 1;
        }

      } else {
        std::cout << "Expected argument for -w";
        return 1;
      }
      continue;
    }
    //WINDOW NAME
    else if (!strcmp(argv[argIndex], "-n") || !strcmp(argv[argIndex], "--name")) {
      //Make certain this isn't the last argument
      //The next argument should be the window name
      if(++argIndex < argc) {

        ctx.name = argv[argIndex];

      } else {
        std::cout << "Expected argument for -n";
        return 1;
      }
      continue;
    }

    std::cout << "Unexpected argument: " << argv[argIndex] << std::endl;
  }

  if(ctx.vertex == "") {
    std::cout << "No -v option included" << std::endl;
    return 1;
  }
  if(ctx.fragment == "") {
    std::cout << "No -f option included" << std::endl;
    return 1;
  }

  return -1;
}

//Accepts name of file to load shader from and string reference to store into
int loadShader(const std::string& filename, std::string& shader) {
  std::ifstream shaderFile(filename);
    
  if(!shaderFile.is_open()) {
      std::cerr << "Could not find shader file: " << filename << std::endl;
      return 1;
  }

  shader = std::string(std::istreambuf_iterator<char>(shaderFile), {});

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