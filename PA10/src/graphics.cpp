#include "graphics.h"

Graphics::Graphics(Menu& menu, const int& w, const int& h, PhysicsWorld* pW, GameWorld::ctx* gwc) : windowWidth(w),
                                                                                                    windowHeight(h),
                                                                                                    m_menu(menu),
                                                                                                    physWorld(pW),
                                                                                                    gameWorldCtx(gwc) {
	camView = new Camera(menu);
	
	pickShader = Shader::load("shaders/vert_pick", "shaders/frag_pick");
}

Graphics::~Graphics() {
	if (camView != nullptr) {
		delete camView;
		camView = NULL;
	}
}

void Graphics::addLight(const LightContext& light) {
	if (light.type == LIGHT_SPOT) {
		spotLights.push_back(light);
	} else if (light.type == LIGHT_POINT) {
		//TODO do this
	}
}

bool Graphics::Initialize(int width, int height) {
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
	// cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;
	
	auto status = glewInit();
	
	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();
	
	//Check for error
	if (status != GLEW_OK) {
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif
	// Initialize Camera
	camView->Initialize(width, height);
	
	// For OpenGL 3
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	std::unordered_map<std::string, std::string> dictionary;
	dictionary["NUM_SPOT_LIGHTS"] = std::to_string(spotLights.size());
	
	for (int i = 0; i < gameWorldCtx->worldObjects.size(); i++) {
		gameWorldCtx->worldObjects[i]->Init_GL(&dictionary);
	}
	
	pickShader->Initialize();
	char str[256];
	int p2Score = 1;
	sprintf( str, "Player 1:  %d", p2Score);
	glGenFramebuffers(1, &pickBuffer);
	glGenRenderbuffers(1, &pickDepthBuffer);
	glGenTextures(1, &pickTexture);
	
	glBindTexture(GL_TEXTURE_2D, pickTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, pickBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, pickDepthBuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickTexture, 0);
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickDepthBuffer);
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return true;
}

void Graphics::updateScreenSize(int width, int height) {
	glBindTexture(GL_TEXTURE_2D, pickTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, pickBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, pickDepthBuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickTexture, 0);
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickDepthBuffer);
}

glm::vec3 hsv2rgb(glm::vec3 in) {
	float hh, p, q, t, ff;
	long i;
	glm::vec3 out;
	
	if (in.r <= 0.0) {
		out.r = in.b;
		out.g = in.b;
		out.b = in.b;
		return out;
	}
	hh = in.r;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long) hh;
	ff = hh - i;
	p = in.b * (1.0 - in.g);
	q = in.b * (1.0 - (in.g * ff));
	t = in.b * (1.0 - (in.g * (1.0 - ff)));
	
	switch (i) {
		case 0:
			out.r = in.b;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.b;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.b;
			out.b = t;
			break;
		
		case 3:
			out.r = p;
			out.g = q;
			out.b = in.b;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.b;
			break;
		case 5:
		default:
			out.r = in.b;
			out.g = p;
			out.b = q;
			break;
	}
	return out;
}

void Graphics::Update(unsigned int dt) {
	glm::vec3 offsetChange = {0, 0, 0};
	
	// Update the object
	for (int i = 0; i < gameWorldCtx->worldObjects.size(); i++) {
		gameWorldCtx->worldObjects[i]->Update(dt);
	}
	
	for (auto& i : spotLights) {
		if (i.isRainbow) {
			i.timer += dt / 25;
			i.color = hsv2rgb(glm::vec3(i.timer, 1.0, 1.0));
		}
	}
	
	//Calculate where our camera should be and update the View matrix
	camView->calculateCamera();
}

Object* Graphics::getObjectOnScreen(int x, int y, glm::vec3* location) {
	renderPick();
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pickBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	PixelInfo pixel;
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	
	int id = pixel.a;
	
	Object* re = nullptr;
	for (int i = 0; i < gameWorldCtx->worldObjects.size(); i++) {
		if (gameWorldCtx->worldObjects[i]->ctx.id == id) {
			re = gameWorldCtx->worldObjects[i];
			break;
		}
	}
	
	if (location != nullptr) {
		location->x = pixel.r;
		location->y = pixel.g;
		location->z = pixel.b;
	}
	
	return re;
	
}

void Graphics::Render() {
	//Switch to rendering on the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	vector<float> spotLightPositions(spotLights.size() * 3);
	vector<float> spotLightDirections(spotLights.size() * 3);
	vector<float> spotLightColors(spotLights.size() * 3);
	vector<float> spotLightAngles(spotLights.size());
	vector<float> spotLightStrengths(spotLights.size());
	
	glm::vec3 normalLightPoint;
	for (unsigned i = 0; i < spotLights.size(); i++) {
		spotLightPositions[i * 3 + 0] = spotLights[i].position.x;
		spotLightPositions[i * 3 + 1] = spotLights[i].position.y;
		spotLightPositions[i * 3 + 2] = spotLights[i].position.z;
		
		normalLightPoint = glm::normalize(*spotLights[i].pointing - spotLights[i].position);
		
		spotLightDirections[i * 3 + 0] = normalLightPoint.x;
		spotLightDirections[i * 3 + 1] = normalLightPoint.y;
		spotLightDirections[i * 3 + 2] = normalLightPoint.z;
		
		spotLightColors[i * 3 + 0] = spotLights[i].color.x;
		spotLightColors[i * 3 + 1] = spotLights[i].color.y;
		spotLightColors[i * 3 + 2] = spotLights[i].color.z;
		
		spotLightAngles[i] = cos(spotLights[i].angle);
		spotLightStrengths[i] = spotLights[i].strength;
	}
	
	//Render planets
	// Update the object
	Shader* shader = nullptr;
	for (int i = 0; i < gameWorldCtx->worldObjects.size(); i++) {
		if (shader != gameWorldCtx->worldObjects[i]->ctx.shader) {
			shader = gameWorldCtx->worldObjects[i]->ctx.shader;
			shader->Enable();
			
			shader->uniform3fv("spotLightPositions", spotLightStrengths.size(), &spotLightPositions[0]);
			shader->uniform3fv("spotLightDirections", spotLightStrengths.size(), &spotLightDirections[0]);
			shader->uniform3fv("spotLightColors", spotLightStrengths.size(), &spotLightColors[0]);
			shader->uniform1fv("spotLightStrengths", spotLightStrengths.size(), &spotLightStrengths[0]);
			shader->uniform1fv("spotLightAngles", spotLightStrengths.size(), &spotLightAngles[0]);
		}
		
		gameWorldCtx->worldObjects[i]->Render();
	}
	
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

void Graphics::renderPick() {
	pickShader->Enable();
	
	glBindFramebuffer(GL_FRAMEBUFFER, pickBuffer);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	for (int i = 0; i < gameWorldCtx->worldObjects.size(); i++) {
		gameWorldCtx->worldObjects[i]->RenderID(pickShader);
	}
}

std::string Graphics::ErrorString(GLenum error) {
	if (error == GL_INVALID_ENUM) {
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	} else if (error == GL_INVALID_VALUE) {
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	} else if (error == GL_INVALID_OPERATION) {
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	} else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	} else if (error == GL_OUT_OF_MEMORY) {
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	} else {
		return "None";
	}
}

vector<Object*>* Graphics::getObject() {
	return &gameWorldCtx->worldObjects;
}

Camera* Graphics::getCamView() {
	return camView;
}