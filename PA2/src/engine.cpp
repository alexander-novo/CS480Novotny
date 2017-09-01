
#include "engine.h"

Engine::Engine(string name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;
  m_FULLSCREEN = false;
}

Engine::Engine(string name)
{
  m_WINDOW_NAME = name;
  m_WINDOW_HEIGHT = 0;
  m_WINDOW_WIDTH = 0;
  m_FULLSCREEN = true;
}

Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window();
  if(!m_window->Initialize(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT))
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  // Set the time
  m_currentTimeMillis = GetCurrentTimeMillis();

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while(m_running)
  {
    // Update the DT
    m_DT = getDT();

    // Check the keyboard input
    while(SDL_PollEvent(&m_event) != 0)
    {
      Keyboard();
    }

    // Update and render the graphics
    m_graphics->Update(m_DT);
    m_graphics->Render();

    // Swap to the Window
    m_window->Swap();
  }
}

void Engine::Keyboard()
{
  if(m_event.type == SDL_QUIT)
  {
    m_running = false;
  }
  else if (m_event.type == SDL_KEYDOWN)
  {
    double* scaleHandler;
    bool backwards;

    //Use keyboard mods to determine what property to change
    if(SDL_GetModState() & KMOD_SHIFT) {
      scaleHandler = &m_graphics->getCube()->moveScale;
    } else if(SDL_GetModState() & KMOD_CTRL) {
      scaleHandler = &m_graphics->getCube()->spinScale;
    } else {
      scaleHandler = &m_graphics->getCube()->timeScale;
    }

    backwards = *scaleHandler < 0.0;

    // handle key down events here
    switch(m_event.key.keysym.sym) {
      //Stop program
      case SDLK_ESCAPE:
        m_running = false;
        break;

      //Change orbit radius
      case SDLK_UP:
        m_graphics->getCube()->distance -= .5;
        if(m_graphics->getCube()->distance < 0.0)
          m_graphics->getCube()->distance = 0.0;
        break;
      case SDLK_DOWN:
        m_graphics->getCube()->distance += .5;
        if(m_graphics->getCube()->distance > 20.0)
          m_graphics->getCube()->distance = 20.0;
        break;

      //Change overall speed
      case SDLK_LEFT:
        *scaleHandler += .05 * (backwards ? 1 : -1);
        break;
      case SDLK_RIGHT:
        *scaleHandler += .05 * (backwards ? -1 : 1);
        break;

      //Reverse direction
      case SDLK_r:
        *scaleHandler *= -1.0;
        break;

      //Stop object
      case SDLK_s:
        *scaleHandler = 0.0;
        break;
    }

  }
}

unsigned int Engine::getDT()
{
  long long TimeNowMillis = GetCurrentTimeMillis();
  assert(TimeNowMillis >= m_currentTimeMillis);
  unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  m_currentTimeMillis = TimeNowMillis;
  return DeltaTimeMillis;
}

long long Engine::GetCurrentTimeMillis()
{
  timeval t;
  gettimeofday(&t, NULL);
  long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  return ret;
}
