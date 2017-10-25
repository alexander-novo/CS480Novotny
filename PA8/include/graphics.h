#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "shader.h"
#include "object.h"
#include "Menu.h"
#include "physics_world.h"
#include "camera.h"

class Graphics {
	public:
		Graphics(Object* sun, float lightPower, Menu& menu, const int& w, const int& h, PhysicsWorld *pW);
		~Graphics();
		
		//Initialise OpenGL
		bool Initialize(int width, int height);
		
		//Update physics and models
		void Update(unsigned int dt);
		
		//Render models
		void Render();
		
		//Return pointer to main Object (sun)
		Object* getObject();

		Camera * getCamView();
	
	private:

		// The camera view
		Camera *camView;
		std::string ErrorString(GLenum error);
		
		//Render the skybox background
		void renderSkybox();
		
		void renderShadowMap();
		
		Menu& m_menu;
		
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

		PhysicsWorld* physWorld;
};

#endif /* GRAPHICS_H */
