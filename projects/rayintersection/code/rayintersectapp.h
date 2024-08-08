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
#include"core/physicscourse/Picker.h"

namespace Game {
class RayIntersectApp : public Core::App {
public:
	RayIntersectApp();
	~RayIntersectApp();

	Display::Window* appWindow;
	std::shared_ptr<Camera>cam;
	std::shared_ptr<Picker>picker;

	bool Open();
	void Close();
	void Run();
	void Exit();
	void RenderUI();
	unsigned long long GetCurrentEpochTime();
};
}