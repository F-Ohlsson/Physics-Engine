#pragma once
#include "config.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"
#include "core/graphicscourse/camera/Camera.h"
#include "core/physicscourse/PhysicsNode.h"
#include "core/physicscourse/PhysicsWorld.h"
#include "core/cvar.h"

class PhysicsNode;
class GraphicsNode;

class PhysicsObject {
	public:
		PhysicsObject();
		PhysicsObject(const PhysicsObject &oldObj);
		~PhysicsObject();



		GraphicsNode* graphN = nullptr;
		PhysicsNode* physN = nullptr;
		void Draw(float deltaTime, Camera cam);
		void Tick(float deltaTime, Camera cam, bool paused);
		void TickPhysics(float deltaTime, bool paused);

		bool AABBoverlap(PhysicsObject* objB);

	private:
};