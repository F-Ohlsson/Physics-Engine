#pragma once
#include "config.h"
#include "core/physicscourse/PhysicsObject.h"
#include "core/physicscourse/ForceApplier.h"
#include "core/physicscourse/CollisionSystem.h"
#include <vector>
#include <deque>


struct contactPair {
	int indexA;
	int indexB;
};

class CollisionSystem;
class ForceApplier;

class PhysicsWorld {
public:
	PhysicsWorld();
	PhysicsWorld(const PhysicsWorld &oldWorld);
	~PhysicsWorld();

	std::vector<PhysicsObject*> physObjs;
	ForceApplier* forceApplier = nullptr;
	CollisionSystem* collisionSystem = nullptr;

	float gravityStrength = 2.0f;

	glm::vec3 gravityDir = glm::vec3(0, -1, 0); //Moon gravity until some sort of collision has been implemented

	float physicsUpdateRate = 1 / 60;
	float physicsUpdateCounter = 1 / 60;

	//Collision stuff
	std::vector<contactPair> contactPairs;

	void Initialize();
	void Tick(float deltaTime, Camera cam);

	void MoveBodies(float deltaTime, Camera cam);
	void BroadPhase();
	void NarrowPhase();

	void AABBSweep(std::vector<contactPair>& contacts);
	uint FindHighestVarianceAxis();



private:
};