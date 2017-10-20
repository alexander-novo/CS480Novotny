//
// Created by alex on 9/20/17.
//

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "graphics_headers.h"

#ifndef TUTORIAL_MODEL_H
#define TUTORIAL_MODEL_H

//Set up which texture channels to use with which type of texture
#define GL_COLOR_TEXTURE    GL_TEXTURE0
#define GL_ALT_TEXTURE      GL_TEXTURE1
#define GL_ALT_TEXTURE2     GL_TEXTURE2
#define GL_NORMAL_TEXTURE   GL_TEXTURE3
#define GL_SPECULAR_TEXTURE GL_TEXTURE4
#define GL_SHADOW_TEXTURE   GL_TEXTURE5
//And the offset (for sending to the sampler)
#define GL_COLOR_TEXTURE_OFFSET    0
#define GL_ALT_TEXTURE_OFFSET      1
#define GL_ALT_TEXTURE2_OFFSET     2
#define GL_NORMAL_TEXTURE_OFFSET   3
#define GL_SPECULAR_TEXTURE_OFFSET 4
#define GL_SHADOW_TEXTURE_OFFSET   5

class Model {
	public:
		struct Material {
			glm::vec3 ambient = {0.2, 0.2, 0.2};  //Ka
			glm::vec3 diffuse = {0.8, 0.8, 0.8};  //Kd
			glm::vec3 specular = {0.0, 0.0, 0.0}; //Ks
			
			float shininess = 0.0f;               //Ns
		} material;
		
		//Load a model from a file
		static Model* load(std::string filename);
		
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		
		//Initalise OpenGL
		//Call after starting OpenGL, but before using drawModel()
		void initGL();
		//Draw the model to the screen
		void drawModel();
		
	private:
		Model();
		
		//OpenGL buffers
		GLuint VB;
		GLuint IB;
		
		//Keeps track of whether of not initGL() has been called yet
		bool initialised;
};

class Texture {
	public:
		//Load texture from file
		static Texture* load(std::string filename);
		
		//Initalise OpenGL
		//Call after starting OpenGL, but before using bind()
		void initGL();
		//Bind the texture for use
		void bind(GLenum textureTarget);
		
	private:
		Texture();
		
		//Keeps track of whether of not initGL() has been called yet
		bool initialised;
		
		//OpenGL texture location
		GLuint m_textureObj;
		
		//ImageMagick data
		Magick::Image* m_Image;
		Magick::Blob* m_Blob;
};
#endif //TUTORIAL_MODEL_H
