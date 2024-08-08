#pragma once
#include "config.h"
#include "core/physicscourse/Ray.h"

class Ray;

class Plane {
public:
	Plane();
	Plane(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
	~Plane();
	
	glm::vec3 normal;
	double d; //Equation variable d
	glm::vec3 basePoint; //Just a point, any point
	
	bool IsPointOnPlane(glm::vec3 point);
	float DistanceToPoint(glm::vec3 point);
	glm::vec3 ProjectPointOntoPlane(glm::vec3 point);

private:
};