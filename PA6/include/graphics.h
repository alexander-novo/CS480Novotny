#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "shader.h"
#include "object.h"
#include "Menu.h"

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
		
		glm::mat4& getProjection();
	
	private:
		std::string ErrorString(GLenum error);
		
		void calculateCamera();
		
		Menu& m_menu;
		
		glm::mat4 projection;
		glm::mat4 view;
		
		float lightPower;
		
		//The sun
		Object *m_cube;
};

#endif /* GRAPHICS_H */
