#include <window.h>
#include <iostream>

Mix_Chunk * Window::bumperSound;
Mix_Chunk * Window::flipperSound;
Mix_Music * Window::bgMusicSound;
Mix_Chunk * Window::launcherSound;
Mix_Chunk * Window::explodeSound;

Window::Window()
{
  gWindow = NULL;
}

Window::~Window()
{
  SDL_StopTextInput();
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  Mix_FreeMusic(Window::bgMusicSound);
  Mix_FreeChunk(Window::flipperSound);
  Mix_FreeChunk(Window::launcherSound);
  Mix_FreeChunk(Window::bumperSound);
  Mix_FreeChunk(Window::explodeSound);
  SDL_Quit();
}

bool Window::Initialize(const string &name, int* width, int* height)
{
  // Start SDL
  // SDL_INIT_EVERYTHING
  // (Timer, Audio, Video, Joystick, Haptic, GameController, Events)
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
  {
    printf("SDL failed to initialize: %s\n", SDL_GetError());
    return false;
  }

  if (MIX_INIT_MP3 != (Mix_Init(MIX_INIT_MP3))) {
    printf("Could not initialize mixer (result:).\n");
    printf("Mix_Init: %s\n", Mix_GetError());
    return false;
  }

  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);


  Window::bumperSound = Mix_LoadWAV(BUMPER_SOUND);
  Window::flipperSound = Mix_LoadWAV(FLIPPER_SOUND);
  Window::bgMusicSound = Mix_LoadMUS(BGMUSIC_SOUND);
  Window::launcherSound = Mix_LoadWAV(LAUNCHER_SOUND);
  Window::explodeSound = Mix_LoadWAV(BUMPER_ALT_SOUND);

  if(!bumperSound) {
    printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
  }
	Mix_AllocateChannels(16);
  Mix_PlayMusic(bgMusicSound,-1);


  // Start OpenGL for SDL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );

  // Create window
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  
  //use for fullscreen
  if (*height == 0 && *width == 0)
  {
    *height = current.h;
    *width = current.w;
  }

  gWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, *width, *height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
  if(gWindow == NULL)
  {
    printf("Widow failed to create: %s\n", SDL_GetError());
    return false;
  }

  // Create context
  gContext = SDL_GL_CreateContext(gWindow);
  if(gContext == NULL) {
    printf("OpenGL context not created: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

void Window::Swap()
{
  SDL_GL_SwapWindow(gWindow);
}

SDL_Window* Window::getSDL_Window() const {
  return gWindow;
}