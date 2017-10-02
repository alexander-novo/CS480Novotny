#include "graphics.h"

Graphics::Graphics(Object* sun, float lightStrength, Menu& menu) : m_cube(sun), lightPower(lightStrength), m_menu(menu) {

}

Graphics::~Graphics() {

}

bool Graphics::Initialize(int width, int height, std::string vertexShader, std::string fragmentShader) {
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
	
	// For OpenGL 3
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Init BaseCamera
	m_camera = new BaseCamera();
	if (!m_camera->Initialize(width, height)) {
		printf("BaseCamera Failed to Initialize\n");
		return false;
	}
	
	m_cube->Init_GL();
	
	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize()) {
		printf("Shader Failed to Initialize\n");
		return false;
	}
	
	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER, vertexShader)) {
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}
	
	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER, fragmentShader)) {
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}
	
	// Connect the program
	if (!m_shader->Finalize()) {
		printf("Program to Finalize\n");
		return false;
	}
	
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_projectionMatrix not found\n");
		return false;
	}
	
	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_viewMatrix not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_modelMatrix not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_ambient = m_shader->GetUniformLocation("MaterialAmbientColor");
	if (m_ambient == INVALID_UNIFORM_LOCATION) {
		printf("m_ambient not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_diffuse = m_shader->GetUniformLocation("MaterialDiffuseColor");
	if (m_diffuse == INVALID_UNIFORM_LOCATION) {
		printf("m_diffuse not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_specular = m_shader->GetUniformLocation("MaterialSpecularColor");
	if (m_specular == INVALID_UNIFORM_LOCATION) {
		printf("m_specular not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_lightSource = m_shader->GetUniformLocation("isLightSource");
	if (m_lightSource == INVALID_UNIFORM_LOCATION) {
		printf("m_lightSource not found\n");
		return false;
	}
	
	// Locate the model matrix in the shader
	m_lightPower = m_shader->GetUniformLocation("lightPower");
	if (m_lightPower== INVALID_UNIFORM_LOCATION) {
		printf("m_lightPower not found\n");
		return false;
	}
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return true;
}

void Graphics::Update(unsigned int dt) {
	// Update the object
	m_cube->Update(dt, glm::mat4(1.0f), m_menu.options.scale);
}

void Graphics::Render() {
	//clear the screen
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Start the correct program
	m_shader->Enable();
	
	calculateCamera();
	
	// Send in the projection and view to the shader
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
	
	float modifiedLight = pow(lightPower, m_menu.options.scale);
	glUniform1fv(m_lightPower, 1, &modifiedLight);
	
	
	// Render the object
	//glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
	m_cube->Render(m_modelMatrix, m_ambient, m_diffuse, m_specular, m_lightSource);
	
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

void Graphics::calculateCamera() {
	//What we're looking at
	glm::vec4 lookVec(0.0, 0.0, 0.0, 1.0);
	//What should be in the background (whatever we're orbiting)
	glm::vec4 backgroundVec(0.0, 0.0, 0.0, 1.0);
	Object* lookingAt = m_menu.getPlanet(m_menu.options.lookingAt);
	Object* parent = lookingAt->parent;
	
	
	
	//Keep track of how large whatever we're looking at is
	//float scale = sqrt(lookingAt->ctx.scale) * 250;
	//float scale = lookingAt->ctx.scale * 15 * lookingAt->ctx.scaleMultiplier;
	float scale = lookingAt->ctx.scaleMultiplier / pow(lookingAt->ctx.scaleMultiplier, m_menu.options.scale) * pow(lookingAt->ctx.scale, m_menu.options.scale) * 15;
	
	//Find the coordinates of whatever the thing we're looking at is and whatever it is orbiting
	lookVec = lookingAt->GetModel() * lookVec;
	
	//If we're orbiting something, put that something in the background of the camera
	if(parent != NULL) {
		backgroundVec = parent->GetModel() * backgroundVec;
	} else {
		backgroundVec = lookVec + glm::vec4(0.0, 0.0, -5.0, 1.0);
	}
	
	
	//Now do some math to find where to place the camera
	//First find the direction pointing from what we're orbiting to what we're looking at
	backgroundVec = lookVec - backgroundVec;
	backgroundVec = glm::normalize(backgroundVec);
	
	glm::vec3 crossVec = glm::normalize(glm::cross(glm::vec3(backgroundVec.x, backgroundVec.y, backgroundVec.z), glm::vec3(0.0, 1.0, 0.0)));
	
	float angle = m_menu.options.rotation * M_PI / 180;
	backgroundVec = cos(angle) * backgroundVec + sin(angle) * glm::vec4(crossVec.x, crossVec.y, crossVec.z, 1.0);
	//Then scale it depending on how large what we're looking at is
	//We don't want to be to far away from a small object or too close to a large object
	backgroundVec *= scale * m_menu.options.zoom;
	//Then add it back to the location of whatever we were looking at to angle the camera in front of what we're looking at AND what it's orbiting
	backgroundVec += lookVec;
	
	//Also let's try and look down from above what we're looking at
	m_camera->GetView() = glm::lookAt( glm::vec3(backgroundVec.x, backgroundVec.y + 0.5 * scale * m_menu.options.zoom * m_menu.options.zoom, backgroundVec.z), //Eye Position
	                                   glm::vec3(lookVec.x, lookVec.y, lookVec.z), //Focus point
	                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
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

Object *Graphics::getCube() {
	return m_cube;
}
