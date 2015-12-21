#include "Camera.h";

Camera::Camera()
{
	cameraRotationSpeed = 0.5f;
	cameraMoveSpeed = 0.002f;

	cameraLocation = glm::vec3(0.0f,0.0f,0.0f);

	viewDirection = glm::vec3(0.0f,0.0f,-1.0f);
	up = glm::vec3(0.0f,1.0f,0.0f);

	cameraPerspectiveSettings = glm::vec4(60.0f,(1920.0f/1080.0f),0.1f,200.0f);
}

void Camera::roateX(float mouseDelta)
{
	viewDirection = glm::vec3(glm::rotate(glm::mat4(1), -1 * mouseDelta * cameraRotationSpeed, up) * glm::vec4(viewDirection,0));
}

void Camera::roateY(float mouseDelta)
{
		glm::vec3 tempVec = glm::vec3(glm::rotate(glm::mat4(1), mouseDelta * cameraRotationSpeed, glm::cross(up,viewDirection)) * glm::vec4(viewDirection,0));
		
		if(tempVec.y > -0.999f && tempVec.y < 0.999f)
		{
			viewDirection = tempVec;
		}

}