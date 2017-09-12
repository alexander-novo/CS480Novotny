
#include "engine.h"

Engine::Engine(const Context &ctx) {
    m_WINDOW_NAME = ctx.name;
    m_WINDOW_WIDTH = ctx.width;
    m_WINDOW_HEIGHT = ctx.height;
    m_FULLSCREEN = ctx.fullscreen;

    m_vertexShader = ctx.vertex;
    m_fragmentShader = ctx.fragment;
}

Engine::~Engine() {
    delete m_window;
    delete m_graphics;
    m_window = NULL;
    m_graphics = NULL;
}

bool Engine::Initialize() {
    // Start a window
    m_window = new Window();
    if (!m_window->Initialize(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT)) {
        printf("The window failed to initialize.\n");
        return false;
    }

    // Start the graphics
    m_graphics = new Graphics();
    if (!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_vertexShader, m_fragmentShader)) {
        printf("The graphics failed to initialize.\n");
        return false;
    }
	
	//Start the menu and connect it to the window
	m_menu = new Menu(*m_window, *m_graphics->getCube());

    // Set the time
    m_currentTimeMillis = GetCurrentTimeMillis();

    // No errors
    return true;
}

void Engine::Run() {
    m_running = true;

    ImVec4 clear_color = ImColor(114, 144, 154);

    while (m_running) {
        // Update the DT
        m_DT = getDT();
	
	    // Check the keyboard input
	    while (SDL_PollEvent(&m_event) != 0) {
		    Keyboard();
	    }
	    
	    //Update the menu and get user's input options
	    m_menu->update();

        // Update and render the graphics
        m_graphics->Update(m_DT);
        m_graphics->Render();

        //Render the GUI
        m_menu->render();

        // Swap to the Window
        m_window->Swap();
    }

    ImGui_ImplSdlGL3_Shutdown();
}

void Engine::Keyboard() {
    float *scaleHandler;
	float minimum = MIN_MOVE_SCALE, maximum = MAX_MOVE_SCALE;
    bool backwards;

    //Use keyboard mods to determine what property to change
    if (SDL_GetModState() & KMOD_SHIFT) {
        scaleHandler = &(m_menu->getCurrentPlanet()->ctx.moveScale);
    } else if (SDL_GetModState() & KMOD_CTRL) {
        scaleHandler = &(m_menu->getCurrentPlanet()->ctx.spinScale);
    } else {
        scaleHandler = &(m_menu->getCurrentPlanet()->ctx.timeScale);
	    minimum = MIN_TIME_SCALE;
	    maximum = MAX_TIME_SCALE;
    }

    if (m_event.type == SDL_QUIT) {
        m_running = false;
    }

        //Keyboard

    else if (m_event.type == SDL_KEYDOWN && !ImGui::GetIO().WantCaptureKeyboard) {
        // handle key down events here
        switch (m_event.key.keysym.sym) {
            //Stop program
            case SDLK_ESCAPE:
                m_running = false;
                break;

                //Change orbit radius
            case SDLK_UP:
                m_graphics->getCube()->ctx.orbitDistance -= .5;
                if (m_graphics->getCube()->ctx.orbitDistance < 0.0)
                    m_graphics->getCube()->ctx.orbitDistance = 0.0;
                break;
            case SDLK_DOWN:
                m_graphics->getCube()->ctx.orbitDistance += .5;
                if (m_graphics->getCube()->ctx.orbitDistance > 20.0)
                    m_graphics->getCube()->ctx.orbitDistance = 20.0;
                break;

                //Change overall speed
            case SDLK_LEFT:
                *scaleHandler -= .05;
                break;
            case SDLK_RIGHT:
                *scaleHandler += .05;
                break;

                //Reverse direction
            case SDLK_r:
	            if (SDL_GetModState() & KMOD_SHIFT) {
		            m_menu->getCurrentPlanet()->ctx.moveDir *= -1;
	            } else if (SDL_GetModState() & KMOD_CTRL) {
		            m_menu->getCurrentPlanet()->ctx.spinDir *= -1;
	            } else {
		            m_menu->getCurrentPlanet()->ctx.timeScale *= -1;
	            }
                break;

                //Stop object
            case SDLK_s:
                *scaleHandler = 0.0;
                break;
        }

    }

        //Mouse
	
    else if (m_event.type == SDL_MOUSEBUTTONDOWN && !ImGui::GetIO().WantCaptureMouse) {
        switch (m_event.button.button) {
            case SDL_BUTTON_LEFT:
                //Reverse object direction
                *scaleHandler *= -1.0;
                break;
            case SDL_BUTTON_RIGHT:
                //Stop object
                *scaleHandler = 0.0;
                break;
        }
    }
	
	if(*scaleHandler < minimum) *scaleHandler = minimum;
	if(*scaleHandler > maximum) *scaleHandler = maximum;
}

unsigned int Engine::getDT() {
    long long TimeNowMillis = GetCurrentTimeMillis();
    assert(TimeNowMillis >= m_currentTimeMillis);
    unsigned int DeltaTimeMillis = (unsigned int) (TimeNowMillis - m_currentTimeMillis);
    m_currentTimeMillis = TimeNowMillis;
    return DeltaTimeMillis;
}

long long Engine::GetCurrentTimeMillis() {
    timeval t;
    gettimeofday(&t, NULL);
    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
}
