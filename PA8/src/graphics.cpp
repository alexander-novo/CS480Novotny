#include "graphics.h"

Graphics::Graphics(float lightStrength, Menu& menu, const int& w, const int& h, PhysicsWorld *pW, GameWorld::ctx *gwc) : windowWidth(w), windowHeight(h), lightPower(lightStrength), m_menu(menu), physWorld(pW), gameWorldCtx(gwc) {
	camView = new Camera(menu);

	// camView->setMenu(*menu);
	
	//Load skybox stuff
	skybox = Model::load("models/Skybox.obj");
	skyShader = Shader::load("shaders/vert_sky", "shaders/frag_sky");
	skyTexture = Texture::load("textures/8k_stars_milky_way.jpg");
	
	shadowShader = Shader::load("shaders/vert_shadow", "shaders/frag_shadow");
}

Graphics::~Graphics() {
	if(camView != nullptr)
	{
		delete camView;
		camView = NULL;
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

	skybox->initGL();
	skyShader->Initialize();
	skyTexture->initGL();
	
	shadowShader->Initialize();

    for(int i = 0; i<gameWorldCtx->worldObjects.size(); i++)
    {
        gameWorldCtx->worldObjects[i]->Init_GL();
    }
	
	glGenFramebuffers(1, &depthBuffer);
	
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	for (uint i = 0 ; i < 6 ; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 5120, 5120, 0, GL_RGB, GL_FLOAT, NULL);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, shadowMap, 0);
	
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 5120, 5120);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return true;
}

void Graphics::Update(unsigned int dt) {
	glm::vec3 offsetChange = {0,0,0};
	
	// Update the object
    for(int i = 0; i<gameWorldCtx->worldObjects.size(); i++)
    {
        gameWorldCtx->worldObjects[i]->Update(dt);
    }

	//Calculate what our offset changed by, in case we need to move the camera
//	offsetChange -= *Object::globalOffset;
	
	//Calculate where our camera should be and update the View matrix
	camView->calculateCamera(offsetChange);
}

void Graphics::Render() {
	if(m_menu.options.drawShadows && m_menu.options.scale == CLOSE_SCALE) renderShadowMap();

	//Switch to rendering on the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float modifiedLight = pow(lightPower, m_menu.options.scale);

	//Render our background
	//Done first so that transparent objects (rings) work properly
	renderSkybox();

	if(m_menu.options.drawShadows) {
		glActiveTexture(GL_SHADOW_TEXTURE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	}

	//Render planets
    // Update the object
    for(int i = 0; i<gameWorldCtx->worldObjects.size(); i++)
    {
        gameWorldCtx->worldObjects[i]->Render(modifiedLight, shadowMap);
    }

	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

void Graphics::renderSkybox() {
	skyShader->Enable();
	
	//Make certain we can see the inside faces, because we will always be inside the skybox
	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
	
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	
	//Make certain the skybox is centered on the camera
	//We don't have to worry about making it big enough to cover everything
	//the shader will make certain it's always in the background
	glm::mat4 modelMat(1.0);
	modelMat = glm::translate(modelMat, camView->eyePos);
	modelMat = glm::scale(modelMat, glm::vec3(20, 20, 20));
	
	glm::mat4 MVPMatrix = camView->GetProjection() * camView->GetView() * modelMat;
	skyShader->uniformMatrix4fv("MVPMatrix", 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	
	skyTexture->bind(GL_COLOR_TEXTURE);
	skyShader->uniform1i("gSampler", GL_COLOR_TEXTURE_OFFSET);
	
	skybox->drawModel();
	
	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}

void Graphics::renderShadowMap() {
	shadowShader->Enable();
	
	glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
	glViewport(0,0,5120,5120);
	
	glCullFace(GL_FRONT);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	//glm::mat4 oldView = view;
	//glm::mat4 oldProj = projection;
	
	glm::vec3 offset = *Object::globalOffset;
	offset *= -1;
	
	lprojection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 100.0f);
	shadowShader->uniform3fv("lightW3", 1, &offset.x);
	
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < 4; i++) {
		lshadowTrans[i] = lprojection * glm::lookAt(offset, offset + camView->getCameraDirection()[i].Target, camView->getCameraDirection()[i].up);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, camView->getCameraDirection()[i].CubemapFace, shadowMap, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		shadowShader->uniformMatrix4fv("VP", 1, GL_FALSE, glm::value_ptr(lshadowTrans[i]));
        for(int j = 0; j<gameWorldCtx->worldObjects.size(); i++)
        {
            gameWorldCtx->worldObjects[i]->renderShadow(shadowShader);
        }
	}
	
	//view = oldView;
	//projection = oldProj;
	glViewport(0, 0, windowWidth, windowHeight);
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

vector <Object *> *Graphics::getObject() {
	return &gameWorldCtx->worldObjects;
}

Camera *Graphics::getCamView()
{
	return camView;
}