#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>

#include "graphics_headers.h"

#define SHADER_DIR "shaders/"
#define SHADER_FILE "shaders/shaderList"

class Shader {
	public:
		//Initialise Shader in OpenGL - call this after creating OpenGL context but before using shader
		bool Initialize();
		//Start using the shader for any render calls
		void Enable();
		//Get uniform location from shader
		GLint GetUniformLocation(const char *pUniformName);
		
		//Set uniforms within shader
		bool uniform1fv(const char* uniform, GLsizei size, const GLfloat* value);
		bool uniform3fv(const char* uniform, GLsizei size, const GLfloat* value);
		bool uniform1i(const char* uniform, GLint value);
		bool uniformMatrix4fv(const char* uniform, GLsizei size, GLboolean transpose, const GLfloat* value);
		
		//Load a shader from a file
		static Shader *load(std::string vertexFile, std::string fragmentFile);
	
	private:
		Shader();
		
		~Shader();
		
		bool AddShader(GLenum ShaderType, const std::string &shader);
		bool Finalize();
		
		bool initialised;
		
		GLuint m_shaderProg;
		std::vector<GLuint> m_shaderObjList;
		
		std::string vertexShader;
		std::string fragmentShader;
		
		static std::unordered_map<std::string, Shader*> loadedShaders;
};

#endif  /* SHADER_H */
