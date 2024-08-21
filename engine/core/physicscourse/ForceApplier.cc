#pragma once
#include "config.h"
#include "core/physicscourse/ForceApplier.h"
#include "render/debugrender.h"

ForceApplier::ForceApplier() {

}

ForceApplier::~ForceApplier() {

}

glm::vec3 ForceApplier::CalculateTorque(PhysicsObject* physObj, glm::vec3 intersectionPoint, glm::vec3 force) {

	glm::vec3 centreMassToForcepoint = intersectionPoint - physObj->physN->AABBposition; //Assumes centre of mass at AABBposition, change if centre of mass is implemented
	glm::vec3 torque = glm::cross(centreMassToForcepoint, force);

	return torque;

}

//Apply force to provided PhysicsObject at provided point
void ForceApplier::ApplyForce(PhysicsObject* physObj, glm::vec3 intersectionPoint, glm::vec3 force) {
	//Linear velocity
	physObj->physN->force += force;
	//Rotation
	physObj->physN->torque += CalculateTorque(physObj, intersectionPoint, force);
}