
#include "engine.h"

Engine::Engine(const Context &a) : _ctx(a), ctx(_ctx), windowWidth(_ctx.width), windowHeight(_ctx.height), mouseDown(false) {}

Engine::~Engine() {
	if(m_window != nullptr)
	{
		delete m_window;
		m_window = nullptr;
	}
	if(m_graphics != nullptr)
	{
		delete m_graphics;
		m_graphics = nullptr;
	}
}

bool Engine::Initialize() {
	// Start a window
	m_window = new Window();
	if (!m_window->Initialize(_ctx.name, &_ctx.width, &_ctx.height)) {
		printf("The window failed to initialize.\n");
		return false;
	}

	//Start the menu and connect it to the window
	m_menu = new Menu(*m_window);

	// Start the graphics
	m_graphics = new Graphics(*m_menu, _ctx.width, _ctx.height, _ctx.gameWorldCtx);
	if(_ctx.lights != nullptr){
		for(auto& i : *_ctx.lights) {
			m_graphics->addLight(i);
		}
		delete _ctx.lights;
		_ctx.lights = nullptr;
	}

	if (!m_graphics->Initialize(_ctx.width, _ctx.height)) {
		printf("The graphics failed to initialize.\n");
		return false;
	}

	Object::menu = m_menu;

	PhysicsWorld::game = _ctx.gameWorldCtx;

	// Set the time
	m_currentTimeMillis = GetCurrentTimeMillis();

	// No errors
	return true;
}

void Engine::Run() {
	m_running = true;

	bool newGame = false;

	while (m_running) {
		// Update the DT
		m_DT = getDT();

		while (SDL_PollEvent(&m_event) != 0) {
			eventHandler(m_DT);
			ImGui_ImplSdlGL3_ProcessEvent(&m_event);
		}

		// Check the keyboard input
		Keyboard(m_DT);

		if(!newGame)
		{
			// Start new game
			NewGame();
			newGame = true;
		}
		m_graphics->Update(m_DT);
		if(!m_menu->options.paused) _ctx.physWorld->update(m_DT);

		// Update menu options and labels
		m_menu->update(m_DT, _ctx.width, _ctx.height);

		if(m_menu->options.shouldClose) m_running = false;
		if(m_menu->options.shouldSwapShaders) {
			for(const auto& i : _ctx.gameWorldCtx->worldObjects) {
				std::swap(i->ctx.shader, i->ctx.altShader);
			}
		}
		if(m_menu->options.shouldStartNewGame) {
			NewGame();
		}

		//Render everything
		m_graphics->Render();
		m_menu->render();

		// Swap to the Window
		m_window->Swap();
	}

	ImGui_ImplSdlGL3_Shutdown();
}

void Engine::Keyboard(unsigned dt) {
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	float cameraSpeed = glm::length(m_graphics->getCamView()->lookAt - m_graphics->getCamView()->eyePos) * dt / 1000;

	if(SDL_GetModState() & KMOD_SHIFT) {
		//Go Up
		glm::vec3 moveDir = {0.0, 1.0, 0.0};
		moveDir *= cameraSpeed;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}
	if(SDL_GetModState() & KMOD_CTRL) {
		//Go Down
		glm::vec3 moveDir = {0.0, -1.0, 0.0};
		moveDir *= cameraSpeed;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}
	if(keyState[SDL_SCANCODE_W]) {
		//Go forward
		glm::vec3 moveDir = glm::normalize(m_graphics->getCamView()->lookAt - m_graphics->getCamView()->eyePos);
		moveDir *= cameraSpeed;
		moveDir.y = 0;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}
	if(keyState[SDL_SCANCODE_A]) {
		//Go left
		glm::vec3 moveDir = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), m_graphics->getCamView()->lookAt - m_graphics->getCamView()->eyePos));
		moveDir *= cameraSpeed;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}
	if(keyState[SDL_SCANCODE_S]) {
		//Go back
		glm::vec3 moveDir = glm::normalize(m_graphics->getCamView()->eyePos - m_graphics->getCamView()->lookAt);
		moveDir.y = 0;
		moveDir *= cameraSpeed;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}
	if(keyState[SDL_SCANCODE_D]) {
		//Go right
		glm::vec3 moveDir = glm::normalize(glm::cross(m_graphics->getCamView()->lookAt - m_graphics->getCamView()->eyePos, glm::vec3(0.0, 1.0, 0.0)));
		moveDir *= cameraSpeed;
		m_graphics->getCamView()->lookAt += moveDir;
		m_graphics->getCamView()->eyePos += moveDir;
	}

	if(keyState[SDL_SCANCODE_N]) {
		// New Game
		NewGame();
	}


	//Width of spotlight
	if( keyState[SDL_SCANCODE_UP]) {
		if(m_graphics->spotLights.size() >= 1 && m_graphics->spotLights[0]->angle < M_PI / 2) {
			m_graphics->spotLights[0]->angle += M_PI / 180;
			if(m_graphics->spotLights[0]->angle > M_PI / 2) {
				m_graphics->spotLights[0]->angle = M_PI / 2;
			}
		}
	}
	if(keyState[SDL_SCANCODE_DOWN]) {
		if(m_graphics->spotLights.size() >= 1 && m_graphics->spotLights[0]->angle > 0) {
			m_graphics->spotLights[0]->angle -= M_PI / 180;
			if(m_graphics->spotLights[0]->angle < 0) {
				m_graphics->spotLights[0]->angle = 0;
			}
		}
	}
}

void Engine::eventHandler(unsigned dt) {
	//Quit program
	if (m_event.type == SDL_QUIT
		|| m_event.type == SDL_KEYDOWN && m_event.key.keysym.sym == SDLK_ESCAPE) {
		m_running = false;
	}

		//Mouse - click and drag rotation
	else if (m_event.type == SDL_MOUSEBUTTONDOWN && !ImGui::GetIO().WantCaptureMouse) {
		switch (m_event.button.button) {
			case SDL_BUTTON_LEFT:
			{
				mouseDown = true;
				glm::vec3 pickedPosition;
				Object* picked = m_graphics->getObjectOnScreen(m_event.button.x, _ctx.height - m_event.button.y, &pickedPosition);
				if(picked != nullptr) {
					glm::vec3 glmImpVector = picked->position - m_graphics->getCamView()->eyePos;
					glmImpVector.y = 0;
					glmImpVector = glm::normalize(glmImpVector);
					glmImpVector *= picked->ctx.mass;

					btVector3 impVector(glmImpVector.x, glmImpVector.y, glmImpVector.z);
					btVector3 locVector(pickedPosition.x, pickedPosition.y, pickedPosition.z);
					picked->ctx.physicsBody->applyImpulse(impVector, locVector);
				}
				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				unsigned seed = GetCurrentTimeMillis();
				std::default_random_engine generator(seed);
				std::uniform_int_distribution<int> distribution(-50,50);
				btVector3 pushIt(0,0,2500);
				_ctx.gameWorldCtx->worldObjects[1]->ctx.physicsBody->applyCentralImpulse(pushIt);
				for(int i = 1; i < _ctx.gameWorldCtx->worldObjects.size(); i++)
				{
					btVector3 shootIt(distribution(generator),0,distribution(generator));
					_ctx.gameWorldCtx->worldObjects[i]->ctx.physicsBody->applyCentralImpulse(shootIt);
				}
			}
		}
	} else if (m_event.type == SDL_MOUSEBUTTONUP) {
		switch (m_event.button.button) {
			case SDL_BUTTON_LEFT:
				mouseDown = false;
				break;
		}
	}
	else if (m_event.type == SDL_MOUSEMOTION && mouseDown) {
		float xscale = 360.0f / _ctx.width;
		float yscale = 180.0f / _ctx.height;

		m_menu->setRotation(m_menu->options.rotation + m_event.motion.xrel * xscale);
		m_menu->setElevation(m_menu->options.elevation + m_event.motion.yrel * yscale);
	}

	else if (m_event.type == SDL_MOUSEWHEEL && !ImGui::GetIO().WantCaptureMouse) {
		float step = 0.05f * m_menu->options.zoom;
		//Scroll down
		if(m_event.wheel.y > 0) {
			step *= -1;
		}

		m_menu->setZoom(m_menu->options.zoom + step);
	}

		//Window resize
	else if(m_event.type == SDL_WINDOWEVENT) {
		switch(m_event.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				_ctx.width = m_event.window.data1;
				_ctx.height = m_event.window.data2;

				//Update our projection matrix in case the aspect ratio is different now
				m_graphics->getCamView()->GetProjection() = glm::perspective( 45.0f,
																			  float(windowWidth)/float(windowHeight),
																			  NEAR_FRUSTRUM,
																			  FAR_FRUSTRUM);

				m_graphics->updateScreenSize(windowWidth, windowHeight);

				//Tell OpenGL how large our window is now
				//SUPER IMPORTANT
				glViewport(0, 0, windowWidth, windowHeight);
				break;
		}
	}
		//For single-press keyboard events
		//For long-hold keyboard events, see keyboard()
	else if(m_event.type == SDL_KEYDOWN) {
		switch(m_event.key.keysym.sym) {
			//Switch shaders
			case SDLK_r:
				m_menu->swapShaderType();
				break;
			case SDLK_o:
				m_menu->toggleOptionsMenu();
				break;
			case SDLK_p:
				// Pause Game
				m_menu->pause();
				break;
			case SDLK_LEFT:
				if(m_menu->options.paused) break;
				Mix_PlayChannel(-1, Window::flipperSound, 0);
				break;
			case SDLK_RIGHT:
				if( m_menu->options.paused) break;
				Mix_PlayChannel(-1, Window::flipperSound, 0);
				break;

			case SDLK_SPACE:
			{
				break;
			}
		}
	}
	else if(m_event.type == SDL_KEYUP)
	{
		switch(m_event.key.keysym.sym)
		{
			case SDLK_SPACE:
			{
				break;
			}
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;

		}
	}
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
	gettimeofday(&t, nullptr);
	long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
	return ret;
}


void Engine::NewGame() {
	static float radius = 0.1; //todo maybe load from config?
	static float height = sqrt(5) * radius;
	static float yOrigin = radius;
	static float zOrigin = -0.25f;
	
	float xCoord;
	btTransform ballTransform;
	btRigidBody* body;
	
	for(int i = 0; i < 5; i++) {
		float width = i * radius * 2;
		for(int j = 0; j <= i; j++) {
			xCoord = i == 0 ? 0 : - width / 2 + width / i * j;
			
			ballTransform.setIdentity();
			ballTransform.setOrigin(btVector3(xCoord, yOrigin, i * height + zOrigin));
			
			body = (*(_ctx.physWorld->getLoadedBodies()))[_ctx.physWorld->ballIndices[i * (i + 1) / 2 + j]];
			body->setWorldTransform(ballTransform);
			body->setLinearVelocity(btVector3(0, 0, 0));
			body->setAngularVelocity(btVector3(0, 0, 0));
		}
	}

	//For testing purposes - uncomment to see ball placement without physics, then press P to turn physics on
	//_ctx.physWorld->update(20);
	//m_menu->pause();
}