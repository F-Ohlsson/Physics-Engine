#pragma once
#include"config.h"
#include"Camera.h"

Camera::Camera(GLuint width, GLuint height, glm::vec3 position) {
	this->height = height;
	this->width = width;
	this->position = position;
}

glm::mat4 Camera::ProjectionMatrix(float FOVrad, float nearPlane, float farPlane) {
	float aspectRatio = (float)height / (float)width;

	//These calculations assume a symmetrical view-cone
	float top = cosf(FOVrad * 0.5f) / sinf(FOVrad * 0.5f) / 2;
	float bottom = -top;
	float h = top - bottom;
	float w = h * aspectRatio;

	glm::mat4 projMat = glm::mat4(
		{w,0,0,0},
		{0,h,0,0},
		{0,0,-((farPlane+nearPlane)/(farPlane-nearPlane)), -1 },
		{0,0,-((2 * farPlane * nearPlane) / (farPlane - nearPlane)),0}
	);


	return projMat;
}

glm::mat4 Camera::ViewMatrix(glm::vec3 lookPoint) {
	
	glm::vec3 forward = glm::normalize(this->position - lookPoint);
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));


	glm::mat4 viewMat = glm::mat4(
		{ right.x,right.y,right.z,0 },
		{ up.x,up.y,up.z,0 },
		{ forward.x,forward.y,forward.z,0 },
		{ position.x,position.y,position.z,1 }
	);

	return  viewMat;
}

glm::mat4 Camera::LookAt(glm::vec3 direction) {

	glm::vec3 forward = glm::normalize(direction);
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));

	if (isnan(right.x)) {
		if (direction.y > 0)
			right = { 1, 0, 0 };
		else
			right = { -1,0,0 };
	}

	glm::vec3 up = glm::normalize(glm::cross(forward, right));


	glm::mat4 viewMat = glm::mat4(
		{ right.x,right.y,right.z,0 },
		{ up.x,up.y,up.z,0 },
		{ forward.x,forward.y,forward.z,0 },
		{ 0,0,0,1 }
	);

	return  viewMat;
}
