#pragma once
#include "config.h"
#include "core/physicscourse/Plane.h"
#include "glm.hpp"

class Plane;

class Ray {
public:
	Ray();
	Ray(glm::vec3 startPos, glm::vec3 endPos);
	~Ray();

	glm::vec3 startPos = glm::vec3(0.f,0.f,0.f);
	glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f);

	void CreateRayBetween(glm::vec3 startPos, glm::vec3 endPos);
	float WillIntersectPlane(Plane plane);
	glm::vec3 PlaneIntersectionPoint(Plane plane);
	//Ray ConvertToModelSpace(glm::mat4 viewMatrix);
private:
};