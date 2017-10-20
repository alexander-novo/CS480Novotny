#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "shader.h"
#include "object.h"
#include "Menu.h"

#define CAMERA_MODE_FOLLOW 1
#define CAMERA_MODE_FREE   2

#define FAR_FRUSTRUM 1000000.0f

class Graphics {
	public:
		Graphics(Object* sun, float lightPower, Menu& menu, const int& w, const int& h);
		
		~Graphics();
		
		//Initialise OpenGL
		bool Initialize(int width, int height);
		
		//Update physics and models
		void Update(unsigned int dt);
		
		//Render models
		void Render();
		
		//Return pointer to main Object (sun)
		Object* getCube();
		
		//Get the projection matrix
		glm::mat4& getProjection();
		
		//Keeps track of where the camera is and what it's looking at
		glm::vec3 lookAt;
		glm::vec3 eyePos;
		
		//Camera mode - see macros above
		int cameraMode;
	
	private:
		struct CameraDirection {
			GLenum CubemapFace;
			glm::vec3 Target;
			glm::vec3 up;
		};
		
		CameraDirection gCameraDirections[4] =
				{
						{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f) },
						{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
						//{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, -1.0f) },
						//{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
						{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },
						{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
				};
		std::string ErrorString(GLenum error);
		
		//Calculate where the camera should be, and the corresponding View matrix
		void calculateCamera(glm::vec3 offsetChange);
		
		//Render the skybox background
		void renderSkybox();
		
		void renderShadowMap();
		
		Menu& m_menu;
		
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 lprojection;
		glm::mat4 lshadowTrans[6];
		
		//How much light the sun should give off
		float lightPower;
		
		GLuint depthBuffer;
		GLuint renderBuffer;
		GLuint shadowMap;
		
		//The sun
		Object* m_cube;
		
		const int& windowWidth;
		const int& windowHeight;
		
		//Skybox render stuff
		Model* skybox;
		Shader* skyShader;
		Texture* skyTexture;
		
		Shader* shadowShader;
};

#endif /* GRAPHICS_H */
