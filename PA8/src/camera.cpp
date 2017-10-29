#include "camera.h"

Camera::Camera(Menu& menu) : m_menu(menu)
{
  cameraMode = CAMERA_MODE_FOLLOW;
  Object::viewMatrix = &view;
  Object::projectionMatrix = &projection;
}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
  //--Init the view and projection matrices
  //  if you will be having a moving camera the view matrix will need to more dynamic
  //  ...Like you should update it before you render more dynamic 
  //  for this project having them static will be fine
  view = glm::lookAt( glm::vec3(0.0, 8.0, -50.0), //Eye Position
                      glm::vec3(0.0, 0.0, 0.0), //Focus point
                      glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

  projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                 float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                 0.001f, //Distance to the near plane, normally a small value like this
                                 FAR_FRUSTRUM); //Distance to the far plane,
  return true;
}

void Camera::calculateCamera(glm::vec3 offsetChange) {
  
  if(cameraMode == CAMERA_MODE_FOLLOW) {
    //What we're looking at
    glm::vec3 lookVec = {0,0,0};
    //What should be in the background (whatever we're orbiting)
    glm::vec3 backgroundVec;
//    Object* lookingAt = m_menu.getPlanet(m_menu.options.lookingAt);
    Object* parent = nullptr; //lookingAt->parent;
    

    //Find the coordinates of whatever the thing we're looking at is and whatever it is orbiting
//    lookVec = lookingAt->position - *Object::globalOffset;
    
    //If we're orbiting something, put that something in the background of the camera
    if (parent != nullptr) {
      backgroundVec = parent->position - *Object::globalOffset;
    } else {
      backgroundVec = lookVec + glm::vec3(0.0, 0.0, -5.0) - *Object::globalOffset;
    }
    
    //Now do some math to find where to place the camera
    //First find the direction pointing from what we're orbiting to what we're looking at
    backgroundVec = glm::normalize(lookVec - backgroundVec);
    
    glm::vec3 crossVec = glm::normalize(
        glm::cross(glm::vec3(backgroundVec.x, backgroundVec.y, backgroundVec.z), glm::vec3(0.0, 1.0, 0.0)));
    
    float angle = m_menu.options.rotation * (float) M_PI / 180.0f;
    backgroundVec = cos(angle) * backgroundVec + sin(angle) * glm::vec3(crossVec.x, crossVec.y, crossVec.z);
    //Then scale it depending on how large what we're looking at is
    //We don't want to be to far away from a small object or too close to a large object
    backgroundVec *= m_menu.options.zoom;
    //Then add it back to the location of whatever we were looking at to angle the camera in front of what we're looking at AND what it's orbiting
    backgroundVec += lookVec;
    
    eyePos = glm::vec3(backgroundVec.x, backgroundVec.y + 0.5 * m_menu.options.zoom * m_menu.options.zoom,
                       backgroundVec.z);
    lookAt = lookVec;
  } else if(cameraMode == CAMERA_MODE_FREE) {
    eyePos += offsetChange;
    lookAt += offsetChange;
  }
  
  //Also let's try and look down from above what we're looking at
  view = glm::lookAt( eyePos, //Eye Position
                        lookAt, //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}

CameraDirection *Camera::getCameraDirection()
{
  return gCameraDirections;
}
