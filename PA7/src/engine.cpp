
#include "engine.h"

Engine::Engine(const Context &ctx, Object *sun) : m_cube(sun), windowWidth(m_WINDOW_WIDTH), windowHeight(m_WINDOW_HEIGHT) {
	m_WINDOW_NAME = ctx.name;
	m_WINDOW_WIDTH = ctx.width;
	m_WINDOW_HEIGHT = ctx.height;
	m_FULLSCREEN = ctx.fullscreen;
	
	m_light = ctx.lightStrength;
	
	mouseDown = false;
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
	
	//Start the menu and connect it to the window
	m_menu = new Menu(*m_window, *m_cube);
	
	// Start the graphics
	m_graphics = new Graphics(m_cube, m_light, *m_menu);
	if (!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT)) {
		printf("The graphics failed to initialize.\n");
		return false;
	}
	
	Object::menu = m_menu;
	
	// Set the time
	m_currentTimeMillis = GetCurrentTimeMillis();
	
	// No errors
	return true;
}

void Engine::Run() {
	m_running = true;
	
	while (m_running) {
		// Update the DT
		m_DT = getDT();
		
		// Check the keyboard input
		while (SDL_PollEvent(&m_event) != 0) {
			Keyboard();
			ImGui_ImplSdlGL3_ProcessEvent(&m_event);
		}
		
		//Make certain whatever we're looking at is at origin
		Object::globalOffset = &m_menu->getPlanet(m_menu->options.lookingAt)->position;
		
		// Update planet positions
		m_graphics->Update(m_DT);
		
		// Update menu options and labels
		m_menu->update(m_DT, m_WINDOW_WIDTH, m_WINDOW_HEIGHT);
		
		//Render everything
		m_graphics->Render();
		m_menu->render();
		
		// Swap to the Window
		m_window->Swap();
	}
	
	ImGui_ImplSdlGL3_Shutdown();
}

void Engine::Keyboard() {
	float *scaleHandler;
	float minimum = MIN_MOVE_SCALE, maximum = MAX_MOVE_SCALE;
	
	//Use keyboard mods to determine what property to change
	if (SDL_GetModState() & KMOD_SHIFT) {
		scaleHandler = &(m_menu->getPlanet(m_menu->options.planetSelector)->ctx.moveScale);
	} else if (SDL_GetModState() & KMOD_CTRL) {
		scaleHandler = &(m_menu->getPlanet(m_menu->options.planetSelector)->ctx.spinScale);
	} else {
		scaleHandler = &(m_menu->getPlanet(m_menu->options.planetSelector)->ctx.timeScale);
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
					m_menu->getPlanet(m_menu->options.planetSelector)->ctx.moveDir *= -1;
				} else if (SDL_GetModState() & KMOD_CTRL) {
					m_menu->getPlanet(m_menu->options.planetSelector)->ctx.spinDir *= -1;
				} else {
					m_menu->getPlanet(m_menu->options.planetSelector)->ctx.timeScale *= -1;
				}
				break;
				
				//Stop object
			case SDLK_s:
				*scaleHandler = 0.0;
				break;
		}
		
	}
		
		//Mouse - click and drag rotation
	
	else if (m_event.type == SDL_MOUSEBUTTONDOWN && !ImGui::GetIO().WantCaptureMouse) {
		switch (m_event.button.button) {
			case SDL_BUTTON_LEFT:
				mouseDown = true;
				break;
		}
	} else if (m_event.type == SDL_MOUSEBUTTONUP) {
		switch (m_event.button.button) {
			case SDL_BUTTON_LEFT:
				mouseDown = false;
				break;
		}
	} else if (m_event.type == SDL_MOUSEMOTION && mouseDown) {
		float scale = 360.0f / m_WINDOW_WIDTH;
		
		m_menu->setRotation(m_menu->options.rotation + m_event.motion.xrel * scale);
	}
	
	else if (m_event.type == SDL_MOUSEWHEEL && !ImGui::GetIO().WantCaptureMouse) {
		float step = 0.05 * m_menu->options.zoom;
		//Scroll down
		if(m_event.wheel.y > 0) {
			step *= -1;
		}
		
		m_menu->setZoom(m_menu->options.zoom + step);
	}
	
	else if(m_event.type == SDL_WINDOWEVENT) {
		switch(m_event.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				m_WINDOW_WIDTH = m_event.window.data1;
				m_WINDOW_HEIGHT = m_event.window.data2;
				
				//Update our projection matrix in case the aspect ratio is different now
				m_graphics->getProjection() = glm::perspective( 45.0f,
				                               float(windowWidth)/float(windowHeight),
				                               0.001f,
				                               10000000.0f);
				
				//Tell OpenGL how large our window is now
				//SUPER IMPORTANT
				glViewport(0, 0, windowWidth, windowHeight);
				break;
		}
	}
	
	if (*scaleHandler < minimum) *scaleHandler = minimum;
	if (*scaleHandler > maximum) *scaleHandler = maximum;
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
