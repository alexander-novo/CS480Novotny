#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"

class Object
{
  public:
    struct Context {
        double timeScale = 1.0;
        double moveScale = 1.0;
        double spinScale = 1.0;
        double orbitDistance = 10.0;

        double scale = 1.0;

        enum Direction {
            cw,
            ccw
        };

        Direction moveDir = cw;
        Direction spinDir = ccw;
    };
    
    Object(const Context& ctx);
    ~Object();
    void Update(unsigned int dt, const glm::mat4& parentModel);
    void Render(GLint& modelLocation);

    glm::mat4 GetModel();
    Context& getContext();

    std::vector<Object> children;

  private:
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    Context ctx;

    struct Time {
        float spin, move;
    } time;
};

#endif /* OBJECT_H */
