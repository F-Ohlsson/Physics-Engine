#pragma once
#include "config.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"
#include "core/physicscourse/Ray.h"

class ForceApplier {
public:

	ForceApplier();
	~ForceApplier();

	glm::vec3 CalculateTorque(PhysicsObject* physObj, glm::vec3 intersectionPoint, glm::vec3 force);
	void ApplyForce(PhysicsObject* physObj, glm::vec3 intersectionPoint, glm::vec3 force);

private:
};