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
    bool AddShader(GLenum ShaderType);
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);

  private:
    //Separate class, for a static constructor
    static class ShaderLoader {
        public: 
            ShaderLoader();
    } shaderLoader;
    friend class ShaderLoader;

    static std::map<std::string,std::string>& getShaderList();

    GLuint m_shaderProg;    
    std::vector<GLuint> m_shaderObjList;
};

#endif  /* SHADER_H */
