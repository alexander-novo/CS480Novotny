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
		bool Initialize(int width, int height, std::string vertexShader, std::string fragmentShader);
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
		Shader *m_shader;
		
		//Where our matrices are in video memory
		GLint m_projectionMatrix;
		GLint m_viewMatrix;
		GLint m_modelMatrix;
		
		GLint m_diffuse;
		GLint m_ambient;
		GLint m_specular;
		
		GLint m_lightSource;
		GLint m_lightPower;
		
		Menu& m_menu;
		
		glm::mat4 projection;
		glm::mat4 view;
		
		float lightPower;
		
		//The sun
		Object *m_cube;
};

#endif /* GRAPHICS_H */
