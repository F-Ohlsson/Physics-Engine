#pragma once
#include "config.h"
#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() {

}

PhysicsObject::~PhysicsObject() {

}

//DO THINGS
void PhysicsObject::Tick(float deltaTime, Camera cam) {
	this->graphN->Tick(deltaTime, cam);
	this->physN->Tick(deltaTime);
}
