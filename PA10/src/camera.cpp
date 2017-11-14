#include "camera.h"

Camera::Camera(Menu& menu) : m_menu(menu) {
	cameraMode = CAMERA_MODE_FOLLOW;
	Object::viewMatrix = &view;
	Object::projectionMatrix = &projection;
}

Camera::~Camera() {

}

bool Camera::Initialize(int w, int h) {
	//--Init the view and projection matrices
	//  if you will be having a moving camera the view matrix will need to more dynamic
	//  ...Like you should update it before you render more dynamic
	//  for this project having them static will be fine
	view = glm::lookAt(glm::vec3(0.0, 8.0, -5.0), //Eye Position
	                   glm::vec3(0.0, 0.0, 0.0), //Focus point
	                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
	
	projection = glm::perspective(45.0f, //the FoV typically 90 degrees is good which is what this is set to
	                              float(w) / float(h), //Aspect Ratio, so Circles stay Circular
	                              NEAR_FRUSTRUM, //Distance to the near plane, normally a small value like this
	                              FAR_FRUSTRUM); //Distance to the far plane,
	return true;
}

void Camera::calculateCamera() {
	
	//What we're looking at
	glm::vec3 lookVec = lookAt;
	//What should be in the background (whatever we're orbiting)
	glm::vec3 backgroundVec = glm::vec3(0.0, 0.0, -1.0);
	float angle = m_menu.options.rotation * (float) M_PI / 180.0f;
	float elev = m_menu.options.elevation * (float) M_PI / 180.0f;
	
	backgroundVec = glm::rotate(backgroundVec, angle, glm::vec3(0.0, 1.0, 0.0));
	backgroundVec = glm::rotate(backgroundVec, elev, glm::cross(glm::vec3(0.0, -1.0, 0.0), backgroundVec));
	backgroundVec = glm::normalize(backgroundVec);
	backgroundVec *= m_menu.options.zoom;
	//Then add it back to the location of whatever we were looking at to angle the camera in front of what we're looking at AND what it's orbiting
	backgroundVec += lookVec;
	
	eyePos = backgroundVec;
	lookAt = lookVec;
	
	//Also let's try and look down from above what we're looking at
	view = glm::lookAt(eyePos, //Eye Position
	                   lookAt, //Focus point
	                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
}

glm::mat4 Camera::GetProjection() {
	return projection;
}

glm::mat4 Camera::GetView() {
	return view;
}

CameraDirection* Camera::getCameraDirection() {
	return gCameraDirections;
}
