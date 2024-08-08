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

float Ray::WillIntersectPlane(Plane plane) {
	float distance;

	glm::vec3 rayToPlane = plane.basePoint - this->startPos;
	distance = (glm::dot(rayToPlane, plane.normal)) / (glm::dot(plane.normal, this->direction));

	return distance;
}

glm::vec3 Ray::PlaneIntersectionPoint(Plane plane) {
	float distance = WillIntersectPlane(plane);
	assert(distance >= 0); //Throw error if ray won't intersect
	
	return this->startPos + this->direction * distance;
}

