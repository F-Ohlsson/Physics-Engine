#pragma once
#include "core/app.h"
#include "render/window.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"
#include "core/graphicscourse/material/blinnPhongMaterial/BlinnPhongMaterial.h"
#include "core/graphicscourse/camera/Camera.h"
#include "core/graphicscourse/dirLight/DirLight.h"

namespace Game {
class ModelLoadApp : public Core::App {
public:
	Display::Window* appWindow;

	ModelLoadApp();
	~ModelLoadApp();

	bool Open();
	void Close();
	void Run();
	void Exit();
};
}