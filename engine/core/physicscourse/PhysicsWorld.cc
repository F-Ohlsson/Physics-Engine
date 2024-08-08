#pragma once
#include "config.h"
#include "PhysicsWorld.h"
#include "render/debugrender.h"
#include "gtc/random.hpp"


PhysicsWorld::PhysicsWorld() {
	this->physObjs = std::vector<PhysicsObject*>();
	this->collisionSystem = new CollisionSystem();
	this->forceApplier = new ForceApplier();
	this->collisionSystem->physWorld = this;

}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& oldWorld) {
	for (int i = 0; i < oldWorld.physObjs.size(); i++) {
		PhysicsObject* pObj = new PhysicsObject(*oldWorld.physObjs[i]);
		this->physObjs.push_back(pObj);
	}

	this->forceApplier = new ForceApplier(*oldWorld.forceApplier);
	this->collisionSystem = new CollisionSystem(*oldWorld.collisionSystem);
	this->collisionSystem->physWorld = this;
}


PhysicsWorld::~PhysicsWorld() {
	while (!this->physObjs.empty()) {
		PhysicsObject* tempPointer = this->physObjs.back();
		this->physObjs.erase(this->physObjs.end()-1);
		delete tempPointer;
	}
}



void PhysicsWorld::Initialize() {
	Core::CVarCreate(Core::CVar_Int, "r_draw_AABBs", "1");
	Core::CVarCreate(Core::CVar_Int, "r_draw_collider", "0");
	Core::CVarCreate(Core::CVar_Int, "r_draw_axes", "0");
	Core::CVarCreate(Core::CVar_Int, "r_draw_EPA_results", "0");
	Core::CVarCreate(Core::CVar_Int, "r_world_frame", "0");
	Core::CVarCreate(Core::CVar_Int, "r_pause", "0");
	Core::CVarCreate(Core::CVar_Int, "r_step_forward", "0");
	Core::CVarCreate(Core::CVar_Int, "r_step_backward", "0");
	Core::CVarCreate(Core::CVar_Int, "r_step_size", "1");
	Core::CVarCreate(Core::CVar_Int, "r_gravity", "0");
	Core::CVarCreate(Core::CVar_Int, "r_damping", "1");

	Core::CVarCreate(Core::CVar_Int, "r_runEPA", "1");
	Core::CVarCreate(Core::CVar_Int, "r_runResponse", "1");

}

void PhysicsWorld::Tick(float deltaTime, Camera cam) {
	physicsUpdateCounter += deltaTime;

	for (int i = 0; i < this->physObjs.size(); i++) {
		this->physObjs[i]->Draw(deltaTime, cam);
	}

	if (physicsUpdateCounter >= physicsUpdateRate) {
		this->MoveBodies(deltaTime, cam);
		this->BroadPhase();
		this->NarrowPhase();
		physicsUpdateCounter = 0;
	}

}

void PhysicsWorld::MoveBodies(float deltaTime, Camera cam) {
	int bPause = Core::CVarReadInt(Core::CVarGet("r_pause"));
	int bGravity = Core::CVarReadInt(Core::CVarGet("r_gravity"));

	for (int i = 0; i < this->physObjs.size(); i++) {
		//Apply gravity
		if (bGravity) {
			if (!bPause) {
				glm::vec3 gravForce = this->gravityDir * this->gravityStrength * (this->physObjs[i]->physN->mass);
				this->forceApplier->ApplyForce(this->physObjs[i], this->physObjs[i]->physN->AABBposition, gravForce);
			}
		}

		//Apply other physics
		this->physObjs[i]->TickPhysics(deltaTime, bPause);
	}
}

void PhysicsWorld::BroadPhase() {
	AABBSweep(this->contactPairs);
}

void PhysicsWorld::NarrowPhase() {
	contactTetrahedron contactTetra = contactTetrahedron();
	for (int i = 0; i < this->contactPairs.size(); i++) {
		int indexA = this->contactPairs[i].indexA;
		int indexB = this->contactPairs[i].indexB;

		if (this->collisionSystem->GJK(indexA, indexB, contactTetra)) {
			//If collision
			//SEND TO EPA
			collisionResults colResults;
			Core::CVar* r_runEPA = Core::CVarGet("r_runEPA");
			int runEPA = Core::CVarReadInt(r_runEPA);
			if (runEPA) {
				//if (this->collisionSystem->OldEPA(indexA, indexB, contactTetra, colResults)) {
				if (this->collisionSystem->NewEPA(indexA, indexB, contactTetra, colResults)) {
					//If we got results from EPA, run reponse
					int paused = Core::CVarReadInt(Core::CVarGet("r_pause"));
					//if (!paused) {
					if (true) {
						int runResponse = Core::CVarReadInt(Core::CVarGet("r_runResponse"));
						if (runResponse) {
							this->collisionSystem->PhysicsResponse(indexA, indexB, colResults, paused);
						}
					}
				}
			}
		}
	}
	this->contactPairs.clear();

}

void PhysicsWorld::AABBSweep(std::vector<contactPair>& contacts) {
	uint highestVarianceAxis = FindHighestVarianceAxis();

	//Tuple of float and int for min and max of AABB in axis direction and object index
	//Sort by float, compare indices. If index == index, close possibility of adding more collisions
	std::vector<std::tuple<float, uint>> sortedAxisEntries;
	for (uint i = 0; i < this->physObjs.size(); i++) {
		sortedAxisEntries.push_back(std::make_tuple(this->physObjs[i]->physN->AABBmin[highestVarianceAxis], i));
		sortedAxisEntries.push_back(std::make_tuple(this->physObjs[i]->physN->AABBmax[highestVarianceAxis], i));
	}
	std::sort(sortedAxisEntries.begin(),sortedAxisEntries.end());

	//Sweep for most variance
	std::vector<uint> openIndices;
	for (int i = 0; i < sortedAxisEntries.size(); i++) {
		bool inList = false;
		uint axisEntryIndex = get<1>(sortedAxisEntries[i]);
		for (uint j = 0; j < openIndices.size(); j++) {
			if (openIndices[j] == axisEntryIndex) {
				//End has been found for this open index
				//Remove it from open indices
				openIndices.erase(openIndices.begin() + j);
				inList = true;
				j--;
				break;
			}
			else {
				contactPair newContact = contactPair(openIndices[j], axisEntryIndex);

				//Duplicate collision check, this is purely to not have two collisions per collision when applying reposnse
				//Since it only checks against previously confirmed collisions the cost is fairly low as long as a massive number of objects are colliding per frame ... which maybe will be the case
				bool duplicate = false;
				for (int k = 0; k < contacts.size(); k++) { //I DO NOT LIKE THIS!!!
					if (newContact.indexA == contacts[k].indexA && newContact.indexB == contacts[k].indexB) {
						duplicate = true;
						break;
					}
					else if (newContact.indexA == contacts[k].indexB && newContact.indexB == contacts[k].indexA) {
						duplicate = true;
						break;
					}
				}
				if (!duplicate) {
						bool results = this->physObjs[newContact.indexA]->AABBoverlap(this->physObjs[newContact.indexB]);
						if(results)
							contacts.push_back(newContact);
				}
			}
		}
		if (!inList) { //Not in open indices, time to open it for potential contacts
			openIndices.push_back(axisEntryIndex);
		}
	}

}


//SEEMS TO WORK
uint PhysicsWorld::FindHighestVarianceAxis() {
	std::vector<std::pair<float, float>> axisMinMax = { 
		std::make_pair<float, float>(FLT_MAX, FLT_MIN), 
		std::make_pair<float, float>(FLT_MAX, FLT_MIN), 
		std::make_pair<float, float>(FLT_MAX, FLT_MIN)};
	//For every object
	for (uint i = 0; i < this->physObjs.size(); i++) {
		//For each axis
		for (uint j = 0; j < 3; j++) {
			//Update the axis min and max
			axisMinMax[j].first = std::min(axisMinMax[j].first, this->physObjs[i]->physN->AABBmin[j]);
			axisMinMax[j].second = std::max(axisMinMax[j].second, this->physObjs[i]->physN->AABBmax[j]);
		}
	}

	uint axisIndex;
	float highestVariance = FLT_MIN;
	for (uint i = 0; i < 3; i++) {
		//If variance on axis is larger than previous largest variance, set as new variance axis
		float axisVariance = axisMinMax[i].second - axisMinMax[i].first;
		if (axisVariance > highestVariance) {
			highestVariance = axisVariance;
			axisIndex = i;
		}
	}

	return axisIndex;
}