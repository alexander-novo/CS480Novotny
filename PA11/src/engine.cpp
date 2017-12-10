
#include "engine.h"

Engine::Engine(const Context &a) : _ctx(a), ctx(_ctx), windowWidth(_ctx.width), windowHeight(_ctx.height) {}

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
		
		static Texture* billboardTex = Texture::load("textures/Green_Ring.png");
		static Texture* billboardTex2 = Texture::load("textures/Red_Ring.png");
		
		if(leftDown && ctx.mode == MODE_TAKE_SHOT) {
#define MAX_RADIUS 0.2f
#define MAX_ZOOM 0.2f
#define MAX_AMPLITUDE 0.15f
#define MAX_TIME 3.0f
			
			billboardTex->initGL();
			billboardTex2->initGL();
			
			float billRadius = min(mouseTimer / 1000.0f / (MAX_TIME / MAX_RADIUS), MAX_RADIUS);
			Texture* billTex = (billRadius == MAX_RADIUS) ? billboardTex2 : billboardTex;
			
			m_graphics->addGuiBillboard(glm::vec3(clickedLocation.x / _ctx.width * 2 - 1,
			                                      clickedLocation.y / _ctx.height * 2 - 1,
			                                      billRadius),
			                            billTex);
			m_graphics->getCamView()->tempZoom *= 1 - min(mouseTimer / 1000.0f / (MAX_TIME / MAX_ZOOM), MAX_ZOOM);
			
			float amplitude = min(float(pow(mouseTimer / 1000.0f / (MAX_TIME / sqrt(MAX_AMPLITUDE)), 2)), MAX_AMPLITUDE);
			float theta = mouseTimer / 50.0f;
			float shakeRadius = amplitude * sin(theta);
			
			m_graphics->getCamView()->screenShake = glm::vec2(shakeRadius * cos(theta), shakeRadius * sin(theta));
			mouseTimer += m_DT;
		}

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
				leftDown = true;
				clickedLocation.x = m_event.button.x;
				clickedLocation.y = _ctx.height - m_event.button.y;
				break;
			}
			case SDL_BUTTON_RIGHT:
				rightDown = true;
				break;
		}
	} else if (m_event.type == SDL_MOUSEBUTTONUP) {
		switch (m_event.button.button) {
			case SDL_BUTTON_LEFT:
				switch(ctx.mode) {
					case MODE_PLACE_CUE: {
						_ctx.mode = MODE_TAKE_SHOT;
						btVector3 trans = ctx.physWorld->getLoadedBodies()->operator[](
								ctx.gameWorldCtx->cueBall)->getWorldTransform().getOrigin();
						m_graphics->getCamView()->lookAt = glm::vec3(trans.x(), trans.y(), trans.z());
						break;
					}
					case MODE_TAKE_SHOT: {
						glm::vec3 pickedPosition;
						Object* picked = m_graphics->getObjectOnScreen(clickedLocation.x, clickedLocation.y,
						                                               &pickedPosition);
						if (picked != nullptr) {
							glm::vec3 glmImpVector = picked->position - m_graphics->getCamView()->eyePos;
							glmImpVector = glm::normalize(glmImpVector);
							glmImpVector *= 1 + mouseTimer / 200;
							
							btVector3 impVector(glmImpVector.x, glmImpVector.y, glmImpVector.z);
							btVector3 locVector(pickedPosition.x, pickedPosition.y, pickedPosition.z);
							picked->ctx.physicsBody->applyImpulse(impVector, locVector);
							
							ctx.gameWorldCtx->isNextShotOK = false;
							ctx.gameWorldCtx->turnSwapped = false;
						}
						break;
					}
				}
				leftDown = false;
				break;
			case SDL_BUTTON_RIGHT:
				rightDown = false;
				mouseTimer = 0;
				break;
		}
	}
	else if (m_event.type == SDL_MOUSEMOTION) {
		switch(ctx.mode) {
			case MODE_PLACE_CUE:
				float yPos = 0.1; //Radius - maybe load this from config?
				glm::vec3 upVector = glm::vec3(0.0, 1.0, 0.0);
				glm::vec3 cameraPos = m_graphics->getCamView()->eyePos;
				glm::vec3 lookTowards = glm::normalize(m_graphics->getCamView()->lookAt - cameraPos);
				glm::vec3 upVectorOrtho = glm::normalize(upVector - glm::dot(upVector, lookTowards) * lookTowards);
				glm::vec3 rightVector = glm::cross(lookTowards, upVectorOrtho);
				
				float near_frust_height = tan(FOV / 2) * NEAR_FRUSTRUM;
				float near_frust_width = near_frust_height * windowWidth / windowHeight;
				float clickedHeight = 1 - m_event.motion.y * 2.0f / windowHeight; //1 at top of window, -1 at bottom
				float clickedWidth  = m_event.motion.x * 2.0f / windowWidth - 1;  //1 at right of window, -1 at left
				
				glm::vec3 pointVector = NEAR_FRUSTRUM * lookTowards
				                        + clickedHeight * near_frust_height * upVectorOrtho
				                        + clickedWidth * near_frust_width * rightVector;
				
				float length = (yPos - cameraPos.y) / pointVector.y;
				float xPos = cameraPos.x + pointVector.x * length;
				float zPos = cameraPos.z + pointVector.z * length;
				
				static float xMax = 0.546552 * 5;//vertex position (from obj file) times table scale
				static float zMax = 1.07326 * 5;
				static float zMin = zMax / 2; //For the kitchen
				
				int kMod = ctx.gameWorldCtx->kMod;
				
				//Clamp kitchen
				xPos = (xPos > xMax) ? xMax : ((xPos < -1 * xMax) ? -1 * xMax : xPos);
				zPos = (zPos > kMod * zMin) ? kMod * zMin : ((zPos < kMod * zMax) ? kMod * zMax : zPos);
				
				btTransform ballTransform;
				ballTransform.setIdentity();
				ballTransform.setOrigin(btVector3(xPos, yPos, zPos));
				
				btRigidBody* ball = ctx.physWorld->getLoadedBodies()->operator[](ctx.gameWorldCtx->cueBall);
				ball->setWorldTransform(ballTransform);
				ball->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
				ball->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
				
				break;
		}
		
		if(rightDown) {
			float xscale = -360.0f / _ctx.width;
			float yscale = 180.0f / _ctx.height;
	
			m_menu->setRotation(m_menu->options.rotation + m_event.motion.xrel * xscale);
			m_menu->setElevation(m_menu->options.elevation + m_event.motion.yrel * yscale);
		}
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
				m_graphics->getCamView()->GetProjection() = glm::perspective( FOV,
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
	static float height = sqrt(3) * radius;
	static float yOrigin = radius;
	static float zOrigin = 1.07326 * 2.5; //Vertex from obj file times scale divided by 2
	
	float xCoord;
	btTransform ballTransform;
	btRigidBody* body;
	
	std::vector<int> tempBallIndices = _ctx.physWorld->ballIndices;
	std::vector<int> randBallIndices;
	int randIndex;
	
	while(tempBallIndices.size() > 0) {
		randIndex = rand() % tempBallIndices.size();
		randBallIndices.push_back(tempBallIndices[randIndex]);
		tempBallIndices.erase(tempBallIndices.begin() + randIndex);
	}
	
	for(int i = 0; i < 5; i++) {
		float width = i * radius * 2;
		for(int j = 0; j <= i; j++) {
			xCoord = i == 0 ? 0 : - width / 2 + width / i * j;
			
			ballTransform.setIdentity();
			ballTransform.setOrigin(btVector3(xCoord, yOrigin, i * height + zOrigin));
			
			body = (*(_ctx.physWorld->getLoadedBodies()))[randBallIndices[i * (i + 1) / 2 + j]];

			body->setWorldTransform(ballTransform);
			body->setLinearVelocity(btVector3(0, 0, 0));
			body->setAngularVelocity(btVector3(0, 0, 0));
		}
	}
	
	_ctx.mode = MODE_PLACE_CUE;
	_ctx.gameWorldCtx->kMod = -1;

	//For testing purposes - uncomment to see ball placement without physics, then press P to turn physics on
	//_ctx.physWorld->update(20);
	//m_menu->pause();
}