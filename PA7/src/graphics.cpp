#include "graphics.h"

Graphics::Graphics(Object* sun, float lightStrength, Menu& menu) : m_cube(sun), lightPower(lightStrength), m_menu(menu) {
	Object::viewMatrix = &view;
	Object::projectionMatrix = &projection;
	Object::globalOffset = &m_cube->position;
}

Graphics::~Graphics() {

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
	
	// For OpenGL 3
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	m_cube->Init_GL();
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
	                               float(width)/float(height), //Aspect Ratio, so Circles stay Circular
	                               0.01f, //Distance to the near plane, normally a small value like this
	                               1000000.0f); //Distance to the far plane,
	
	return true;
}

void Graphics::Update(unsigned int dt) {
	// Update the object
	m_cube->Update(dt, m_menu.options.scale, m_menu.options.drawOrbits);
}

void Graphics::Render() {
	//clear the screen
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	calculateCamera();
	
	float modifiedLight = pow(lightPower, m_menu.options.scale);
	
	// Render the object
	m_cube->Render(modifiedLight, m_menu.options.drawOrbits);
	
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

void Graphics::calculateCamera() {
	//What we're looking at
	glm::vec3 lookVec;
	//What should be in the background (whatever we're orbiting)
	glm::vec3 backgroundVec;
	Object* lookingAt = m_menu.getPlanet(m_menu.options.lookingAt);
	Object* parent = lookingAt->parent;
	
	//Keep track of how large whatever we're looking at is
	//float scale = sqrt(lookingAt->ctx.scale) * 250;
	//float scale = lookingAt->ctx.scale * 15 * lookingAt->ctx.scaleMultiplier;
	float scale = lookingAt->ctx.scaleMultiplier / pow(lookingAt->ctx.scaleMultiplier, m_menu.options.scale) * pow(lookingAt->ctx.scale, m_menu.options.scale) * 15;
	
	//Find the coordinates of whatever the thing we're looking at is and whatever it is orbiting
	lookVec = lookingAt->position;
	
	//If we're orbiting something, put that something in the background of the camera
	if(parent != NULL) {
		backgroundVec = parent->position;
	} else {
		backgroundVec = lookVec + glm::vec3(0.0, 0.0, -5.0);
	}
	
	//Now do some math to find where to place the camera
	//First find the direction pointing from what we're orbiting to what we're looking at
	backgroundVec = glm::normalize(lookVec - backgroundVec);
	
	glm::vec3 crossVec = glm::normalize(glm::cross(glm::vec3(backgroundVec.x, backgroundVec.y, backgroundVec.z), glm::vec3(0.0, 1.0, 0.0)));
	
	float angle = m_menu.options.rotation * M_PI / 180;
	backgroundVec = cos(angle) * backgroundVec + sin(angle) * glm::vec3(crossVec.x, crossVec.y, crossVec.z);
	//Then scale it depending on how large what we're looking at is
	//We don't want to be to far away from a small object or too close to a large object
	backgroundVec *= scale * m_menu.options.zoom;
	//Then add it back to the location of whatever we were looking at to angle the camera in front of what we're looking at AND what it's orbiting
	backgroundVec += lookVec;
	
	backgroundVec -= *Object::globalOffset;
	lookVec -= *Object::globalOffset;
	
	//Also let's try and look down from above what we're looking at
	view = glm::lookAt( glm::vec3(backgroundVec.x, backgroundVec.y + 0.5 * scale * m_menu.options.zoom * m_menu.options.zoom, backgroundVec.z), //Eye Position
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

glm::mat4& Graphics::getProjection() {
	return projection;
}