#pragma once
#include "config.h"
#include "Plane.h"
#include <iostream>

//Plane equation ax+by+cz=d
// [a,b,c] = normal[x,y,z]

Plane::Plane() {

}

Plane::Plane(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3) {
	glm::vec3 edge1 = point2 - point1;
	glm::vec3 edge2 = point3 - point1;

	this->normal = glm::normalize(glm::cross(edge1, edge2));

	assert(this->normal != glm::vec3(0, 0, 0) && "COLINEAR VECTORS, NOT VALID PLANE");

	this->d = (normal.x * point1.x + normal.y * point1.y + normal.z * point1.z);
	this->basePoint = point1;

	//std::cout << this->normal.x << "; " << this->normal.y << "; " << this->normal.z << std::endl;
}

Plane::~Plane() {

}

//Calculate shortest distance between point and plane
float Plane::DistanceToPoint(glm::vec3 point) {
	glm::vec3 betweenPointAndBase = point - this->basePoint; //Hypothenuse of right angled triangle (vector B)
	//Plane normal (vector A)
	
	//Distance = B * cos(theta)
	//B * cos(theta) = (A dot B) / abs(A)
	//This is because A dot B = abs(A) * abs(B) * cos(theta)

	float numerator = abs(glm::dot(this->normal, betweenPointAndBase));
	float denominator = abs(glm::length(this->normal));

	return numerator/denominator;
}

bool Plane::IsPointOnPlane(glm::vec3 point) {
	float result = this->normal.x * point.x + this->normal.y * point.y + this->normal.z * point.z;
	float diff = result - this->d;
	//std::cout << diff << std::endl;
	return std::abs(diff) < 0.000001f ? true : false;
}

glm::vec3 Plane::ProjectPointOntoPlane(glm::vec3 point) {
	float distance = DistanceToPoint(point);
	return point - (this->normal * distance);
}
