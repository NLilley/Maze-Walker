#pragma once
#include "middleware\glm\glm\glm.hpp"
#include "middleware\glm\glm\gtc\matrix_transform.hpp"

class Camera
{

public:
	float cameraRotationSpeed;
	float cameraMoveSpeed;

	glm::vec3 cameraLocation;

	glm::vec3 viewDirection;
	glm::vec3 up;

	glm::vec4 cameraPerspectiveSettings;

	Camera();

	void roateX(float mouseDelta);

	void roateY(float mouseDelta);
};