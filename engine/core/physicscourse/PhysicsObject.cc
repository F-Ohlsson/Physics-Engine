#pragma once
#include "config.h"
#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() {
	this->graphN = nullptr;
	this->physN = nullptr;
}

PhysicsObject::PhysicsObject(const PhysicsObject& oldObj) {
	this->graphN = new GraphicsNode(*oldObj.graphN);
	this->physN = new PhysicsNode(*oldObj.physN);

	this->graphN->parent = this;
	this->physN->parent = this;
}


PhysicsObject::~PhysicsObject() {
	//TODO, DELETE TEH STUFF
	delete this->graphN;
	delete this->physN;
}

void PhysicsObject::Draw(float deltaTime, Camera cam) {
	this->graphN->Tick(deltaTime, cam, false);
}

//Update physics object according to time since last update
void PhysicsObject::Tick(float deltaTime, Camera cam, bool paused) {
	//this->graphN->Tick(deltaTime, cam, paused);
	this->physN->Tick(deltaTime, paused);
}

//Apply physics according to time since last update
void PhysicsObject::TickPhysics(float deltaTime, bool paused) {
	this->graphN->TickPhysics(deltaTime, paused);
	this->physN->Tick(deltaTime, paused);
}


bool PhysicsObject::AABBoverlap(PhysicsObject* objB) {
	bool xAxis = this->physN->AABBmax.x > objB->physN->AABBmin.x && objB->physN->AABBmax.x > this->physN->AABBmin.x;
	if (!xAxis)
		return false;
	
	bool yAxis = this->physN->AABBmax.y > objB->physN->AABBmin.y && objB->physN->AABBmax.y > this->physN->AABBmin.y;
	if (!yAxis)
		return false;

	bool zAxis = this->physN->AABBmax.z > objB->physN->AABBmin.z && objB->physN->AABBmax.z > this->physN->AABBmin.z;
	if (!zAxis)
		return false;

	return true;
}


