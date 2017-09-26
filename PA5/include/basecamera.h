#ifndef BASECAMERA_H
#define BASECAMERA_H

#include "graphics_headers.h"

class BaseCamera
{
  public:
    BaseCamera();
    ~BaseCamera();
    bool Initialize(int w, int h);
    glm::mat4& GetProjection();
    glm::mat4& GetView();
  
  private:
    glm::mat4 projection;
    glm::mat4 view;
};

#endif /* BASECAMERA_H */
