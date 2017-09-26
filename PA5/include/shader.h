#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <fstream>
#include <map>

#include "graphics_headers.h"

#define SHADER_DIR "shaders/"
#define SHADER_FILE "shaders/shaderList"

class Shader
{
  public:
    Shader();
    ~Shader();
    bool Initialize();
    void Enable();
    bool AddShader(GLenum ShaderType, const std::string& shader);
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);

  private:
    GLuint m_shaderProg;    
    std::vector<GLuint> m_shaderObjList;
};

#endif  /* SHADER_H */
