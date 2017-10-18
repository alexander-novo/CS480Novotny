#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "shader.h"
#include "object.h"
#include "Menu.h"

#define CAMERA_MODE_FOLLOW 1
#define CAMERA_MODE_FREE   2

class Graphics {
	public:
		Graphics(Object *sun, float lightPower, Menu& menu);
		
		~Graphics();
		
		//Initialise OpenGL
		bool Initialize(int width, int height);
		//Update physics and models
		void Update(unsigned int dt);
		//Render models
		void Render();
		
		//Return pointer to main Object (sun)
		Object *getCube();
		//Get the projection matrix
		glm::mat4& getProjection();
		
		//Keeps track of where the camera is and what it's looking at
		glm::vec3 lookAt;
		glm::vec3 eyePos;
		
		//Camera mode - see macros above
		int cameraMode;
	
	private:
		std::string ErrorString(GLenum error);
		
		//Calculate where the camera should be, and the corresponding View matrix
		void calculateCamera(glm::vec3 offsetChange);
		//Render the skybox background
		void renderSkybox();
		
		Menu& m_menu;
		
		glm::mat4 projection;
		glm::mat4 view;
		
		//How much light the sun should give off
		float lightPower;
		
		//The sun
		Object *m_cube;
		
		//Skybox render stuff
		Model* skybox;
		Shader* skyShader;
		Texture* skyTexture;
};

#endif /* GRAPHICS_H */
