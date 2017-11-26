//
// Created by alex on 9/20/17.
//

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "graphics_headers.h"
#include <shader.h>

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

struct Material {
	glm::vec3 ambient = {0.2, 0.2, 0.2};  //Ka
	glm::vec3 diffuse = {0.8, 0.8, 0.8};  //Kd
	glm::vec3 specular = {0.0, 0.0, 0.0}; //Ks
	
	float shininess = 0.0f;               //Ns
};

struct Mesh {
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;
	Material material;
	
	//OpenGL buffers
	GLuint VB;
	GLuint IB;
};

class Model {
	public:
		//Load a model from a file
		static Model* load(std::string filename);
		
		std::vector<Mesh> meshes;
		
		//Initalise OpenGL
		//Call after starting OpenGL, but before using drawModel()
		void initGL();
		//Draw the model to the screen
		void drawModel(Shader* shader);
		
	protected:
		Model();
		
		static void loadVertices(aiMesh *mesh, Mesh *newModel);
		static void loadIndices(aiMesh *mesh, Mesh *newModel);
		static Material loadMaterials(const aiScene *scene, int meshIndex);
		
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

