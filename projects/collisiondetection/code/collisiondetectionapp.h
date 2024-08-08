#pragma once
#include "core/app.h"
#include "render/window.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"
#include "core/graphicscourse/material/blinnPhongMaterial/BlinnPhongMaterial.h"
#include "core/graphicscourse/camera/Camera.h"
#include "core/graphicscourse/pointLight/PointLight.h"
#include "core/graphicscourse/dirLight/DirLight.h"
#include "imgui.h"
#include "core/cvar.h"
#include "render/debugrender.h"
#include "core/physicscourse/Ray.h"
#include "core/physicscourse/Picker.h"
#include "core/physicscourse/ForceApplier.h"
#include "core/physicscourse/PhysicsWorld.h"

namespace Game {
class CollisionDetectionApp : public Core::App {
public:
	CollisionDetectionApp();
	~CollisionDetectionApp();

	Display::Window* appWindow;
	Camera* cam = nullptr;
	Picker* picker = nullptr;
	PhysicsWorld* physWorld = nullptr;
	std::vector<PhysicsWorld*> worldStates;



	bool Open();
	void Close();
	void Run();
	void Exit();
	void RenderUI();
	unsigned long long GetCurrentEpochTime();
};
}