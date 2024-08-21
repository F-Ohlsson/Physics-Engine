#pragma once
#include "config.h"
#include "core/physicscourse/PhysicsObject.h"
#include <vector>

class PhysicsObject;

enum tensorTypes {
	e_zero = 0,
	e_sphere = 1,
	e_cuboid = 2,
	e_cylinder = 3,
};

struct triangle {
	glm::vec3 vertexPos[3] = { glm::vec3(0),glm::vec3(0),glm::vec3(0) };
	glm::vec3 normal = glm::vec3(0);
	triangle() {};
	triangle(glm::vec3 point0, glm::vec3 point1, glm::vec3 point2) {
		this->vertexPos[0] = point0;
		this->vertexPos[1] = point1;
		this->vertexPos[2] = point2;
		this->normal = glm::normalize(glm::cross(point1 - point0, point2 - point0));
	}
};

class PhysicsNode {
public:
	PhysicsNode();
	PhysicsNode(const PhysicsNode& pN);
	~PhysicsNode();

	PhysicsObject* parent = nullptr;

	float invMass = 0.f;
	float mass = 0.f;
	float elasticity = 0.f;


	//Axis Alligned Bounding Box
	glm::vec3 AABBmin = glm::vec3(0);
	glm::vec3 AABBmax = glm::vec3(0);
	glm::vec3 AABBscale = glm::vec3(0);
	glm::vec3 AABBposition = glm::vec3(0);
	glm::vec4 AABBcolor = glm::vec4(1);
	float AABBlineWeight = 1.f;
	triangle AABB[12];
	

	//Linear
	glm::vec3 force = glm::vec3(0);
	glm::vec3 linearVelocity = glm::vec3(0);
	glm::vec3 linearAcceleration = glm::vec3(0);

	//Angular
	glm::vec3 torque = glm::vec3(0); //Rotational axis and angular force (length)
	glm::vec3 angularVelocity = glm::vec3(0);
	glm::vec3 angularAcceleration = glm::vec3(0);
	glm::quat orientation = glm::quat(1,0,0,0); //Essentially the rotation matrix
	glm::mat3 inertiaTensor;
	glm::mat3 worldAllignedInvertedInertiaTensor;

	std::vector<triangle> colliderTriangles;

	void SetInertiaTensor(tensorTypes tensor);
	void SetAABB();
	void SetMass(float newMass);
	void SetAABBTriangles();
	void Tick(float deltaTime, bool paused);
	void Rotate(float deltaTime);
	void CalcWorldAllignedInvertedInertiaTensor(glm::mat4 rotMat);
	void LoadColliderGLTF(std::string filename);
	void Damping(float deltaTime);

private:
	//Physics
	tensorTypes tensorType = tensorTypes::e_zero;
};