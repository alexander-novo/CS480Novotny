#include "camera.h"

Camera::Camera(Menu& menu) : m_menu(menu) {}

Camera::~Camera() {}

bool Camera::Initialize(int w, int h) {
	//--Init the view and projection matrices
	//  if you will be having a moving camera the view matrix will need to more dynamic
	//  ...Like you should update it before you render more dynamic
	//  for this project having them static will be fine
	view = glm::lookAt(glm::vec3(0.0, 8.0, -5.0), //Eye Position
	                   glm::vec3(0.0, 0.0, 0.0), //Focus point
	                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
	
	projection = glm::perspective(FOV, //the FoV typically 90 degrees is good which is what this is set to
	                              float(w) / float(h), //Aspect Ratio, so Circles stay Circular
	                              NEAR_FRUSTRUM, //Distance to the near plane, normally a small value like this
	                              FAR_FRUSTRUM); //Distance to the far plane,
	return true;
}

void Camera::calculateCamera(unsigned dt) {
	
	if(movingTime > 0) {
		float moveTime = dt > movingTime ? movingTime : dt;
		glm::vec3 moveVec = movingTowards * (moveTime / movingTime);
		lookAt += moveVec;
		movingTowards -= moveVec;
		movingTime -= moveTime;
	}
	
	//What we're looking at
	glm::vec3 lookVec = lookAt;
	glm::vec3 shakeFactor = glm::vec3(0.0, 0.0, 0.0);
	//What should be in the background (whatever we're orbiting)
	glm::vec3 backgroundVec = glm::vec3(0.0, 0.0, -1.0);
	float angle = m_menu.options.rotation * (float) M_PI / 180.0f;
	float elev = m_menu.options.elevation * (float) M_PI / 180.0f;
	
	backgroundVec = glm::rotate(backgroundVec, angle, glm::vec3(0.0, 1.0, 0.0));
	backgroundVec = glm::rotate(backgroundVec, elev, glm::cross(glm::vec3(0.0, -1.0, 0.0), backgroundVec));
	backgroundVec = glm::normalize(backgroundVec);
	backgroundVec *= m_menu.options.zoom * tempZoom;
	//Then add it back to the location of whatever we were looking at to angle the camera in front of what we're looking at AND what it's orbiting
	backgroundVec += lookVec;
	
	if(screenShake != glm::vec2(0.0, 0.0)) {
		glm::vec3 upVector = glm::vec3(0.0, 1.0, 0.0);
		glm::vec3 lookTowards = glm::normalize(lookVec - backgroundVec);
		glm::vec3 upVectorOrtho = glm::normalize(upVector - glm::dot(upVector, lookTowards) * lookTowards);
		glm::vec3 rightVector = glm::cross(lookTowards, upVectorOrtho);
		
		shakeFactor = (upVectorOrtho * screenShake.y + rightVector * screenShake.x) * m_menu.options.zoom * tempZoom * 0.1f;
		backgroundVec += shakeFactor;
		lookVec += shakeFactor;
		
		screenShake = glm::vec2(0.0, 0.0);
	}
	
	eyePos = backgroundVec;
	lookAt = lookVec;
	
	//Also let's try and look down from above what we're looking at
	view = glm::lookAt(eyePos, //Eye Position
	                   lookAt, //Focus point
	                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
	
	lookAt -= shakeFactor;
	
	tempZoom = 1;
}

void Camera::moveTowards(glm::vec3 towards, unsigned time) {
	movingTowards = towards - lookAt;
	movingTime = time;
}

glm::mat4& Camera::GetProjection() {
	return projection;
}

glm::mat4& Camera::GetView() {
	return view;
}

CameraDirection* Camera::getCameraDirection() {
	return gCameraDirections;
}
