#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"

class Object
{
  public:
    Object(double = 1.0, double = 1.0, double = 1.0, double = 10.0);
    ~Object();
    void Update(unsigned int dt);
    void Render();

    glm::mat4 GetModel();

    double timeScale, moveScale, spinScale, distance;

  private:
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    struct Time {
        float spin, move;
    } time;
};

#endif /* OBJECT_H */
