#ifndef GRAPHICS_HEADERS_H
#define GRAPHICS_HEADERS_H

#include <iostream>

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#include <OpenGL/GLU.h>
#else //linux as default

#include <GL/glew.h>
//#include <GL/glu.h>
#endif


//Assimp
#include <assimp/Importer.hpp>      // Importer that loads obj file
#include <assimp/scene.h>           // For the aiScene object
#include <assimp/postprocess.h>    // Postprocessing variables for Importer
#include <assimp/color4.h>          // aiColor4 object - handles colors from mesh objects

//ImageMagick
#include <Magick++.h>

// GLM for matricies
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#define INVALID_UNIFORM_LOCATION 0x7fffffff

struct Vertex {
	glm::vec3 vertex;
	glm::vec3 color;
	glm::vec3 normal;
	
	Vertex(glm::vec3 v, glm::vec3 c) : vertex(v), normal(c) {}
};

#endif /* GRAPHICS_HEADERS_H */
