#pragma once
#include "config.h"
#include "Ray.h"

Ray::Ray() {

}

Ray::Ray(glm::vec3 startPos, glm::vec3 endPos) {
	CreateRayBetween(startPos, endPos);
}

Ray::~Ray() {

}

void Ray::CreateRayBetween(glm::vec3 startPos, glm::vec3 endPos) {
	this->startPos = startPos;
	this->direction = glm::normalize(endPos - startPos);
}

//Returns distance between ray and plane
float Ray::PlaneIntersectionDistance(Plane plane) {
	float distance;

	glm::vec3 rayToPlane = plane.basePoint - this->startPos;
	distance = (glm::dot(rayToPlane, plane.normal)) / (glm::dot(plane.normal, this->direction));

	return distance;
}

//Returns point where ray intersects provided plane
glm::vec3 Ray::PlaneIntersectionPoint(Plane plane) {
	float distance = PlaneIntersectionDistance(plane);
	assert(distance >= 0); //Throw error if ray won't intersect
	
	return this->startPos + this->direction * distance;
}

