#pragma once
#include"config.h"
#include "core/app.h"
//#include <memory>

class Camera {
public:
	Camera(GLuint width, GLuint height, glm::vec3 position);

	GLuint height;
	GLuint width;
	
	float speed = 1.0f;
	float rotation = 0.0f;
	
	glm::vec3 position = glm::vec3();
	glm::mat4 view = glm::mat4();
	glm::mat4 projection = glm::mat4();

	glm::mat4 ProjectionMatrix(float FOVrad, float nearPlane, float farPlane);
	glm::mat4 ViewMatrix(glm::vec3 direction);
	glm::mat4 LookAt(glm::vec3 direction);
};