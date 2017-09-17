#include "graphics.h"

Graphics::Graphics() {

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
	
	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height)) {
		printf("Camera Failed to Initialize\n");
		return false;
	}
	
	// Create the object
	Object::Context sunCtx;
	sunCtx.spinScale = 0.25f;
	sunCtx.spinDir = 1;
	sunCtx.moveScale = 0.0f;
	sunCtx.orbitDistance = 0;
	sunCtx.name = "Sun";
	m_cube = new Object(sunCtx, NULL);
	
	Object::Context earthCtx;
	earthCtx.scale = 0.25;
	earthCtx.orbitDistance = 10.0;
	earthCtx.moveScale = 0.25;
	earthCtx.name = "Earth";
	
	m_cube->addChild(earthCtx);
	
	Object::Context moonCtx;
	moonCtx.scale = 0.1;
	moonCtx.moveScale = 3.0;
	moonCtx.moveDir = 1;
	moonCtx.orbitDistance = 1.0;
	moonCtx.name = "The Moon";
	
	(*m_cube)[0].addChild(moonCtx);
	
	Object::Context marsCtx;
	marsCtx.scale = 0.15;
	marsCtx.orbitDistance = 15.0;
	marsCtx.moveScale = 0.125;
	marsCtx.name = "Mars";
	
	m_cube->addChild(marsCtx);
	
	Object::Context potatoCtx;
	potatoCtx.scale = 0.025;
	potatoCtx.moveScale = 2.75;
	potatoCtx.moveDir = 1;
	potatoCtx.orbitDistance = .3;
	potatoCtx.name = "Potato Moon";
	
	(*m_cube)[1].addChild(potatoCtx);
	
	Object::Context almostSphereCtx;
	almostSphereCtx.scale = 0.05;
	almostSphereCtx.moveScale = 2;
	almostSphereCtx.spinDir = 1;
	almostSphereCtx.orbitDistance = .4;
	almostSphereCtx.name = "Almost a Sphere";
	
	(*m_cube)[1].addChild(almostSphereCtx);
	
	
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
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return true;
}

void Graphics::Update(unsigned int dt) {
	// Update the object
	m_cube->Update(dt, glm::mat4(1.0f));
}

void Graphics::Render(const Menu& menu) {
	//clear the screen
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Start the correct program
	m_shader->Enable();
	
	if(menu.options.lookingAt != -1) {
		//What we're looking at
		glm::vec4 lookVec(0.0, 0.0, 0.0, 1.0);
		//What should be in the background (whatever we're orbiting)
		glm::vec4 backgroundVec(0.0, 0.0, 0.0, 1.0);
		Object* lookingAt = menu.getPlanet(menu.options.lookingAt);
		Object* parent = lookingAt->parent;
		
		//If we're orbiting something, put that something in the background of the camera
		//Otherwise, default camera view (i.e. the sun)
		if(parent != NULL) {
			float scale = lookingAt->ctx.scale * 25;
			
			lookVec = lookingAt->GetModel() * lookVec;
			backgroundVec = parent->GetModel() * backgroundVec;
			
			backgroundVec = lookVec - backgroundVec;
			backgroundVec = glm::normalize(backgroundVec);
			backgroundVec *= scale;
			backgroundVec += lookVec;
			
			m_camera->GetView() = glm::lookAt( glm::vec3(backgroundVec.x, backgroundVec.y + 0.5 * scale, backgroundVec.z), //Eye Position
			                                   glm::vec3(lookVec.x, lookVec.y, lookVec.z), //Focus point
			                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
		} else {
			m_camera->GetView() = glm::lookAt( glm::vec3(0.0, 8.0, -16.0),
			                                   glm::vec3(0.0, 0.0, 0.0),
			                                   glm::vec3(0.0, 1.0, 0.0));
		}
		
	}
	
	
	
	// Send in the projection and view to the shader
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
	
	// Render the object
	//glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
	m_cube->Render(m_modelMatrix);
	
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
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

Object *Graphics::getCube() {
	return m_cube;
}
