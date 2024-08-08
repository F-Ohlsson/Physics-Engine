#pragma once
#include "config.h"
#include "core/graphicscourse/material/blinnPhongMaterial/BlinnPhongMaterial.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"
#include "GLFW/glfw3.h"
#include <tuple>
#include "render/window.h"
#include "core/graphicscourse/camera/Camera.h"
#include "core/physicscourse/Ray.h"
#include "core/physicscourse/Plane.h"
#include "render/debugrender.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h"

class Picker {
public:
	Picker();
	~Picker();
	BlinnPhongMaterial* baseMaterial = nullptr;
	BlinnPhongMaterial* highlightMaterial = nullptr;
	GraphicsNode* currentHighlight = nullptr;
	void AddHighlight(GraphicsNode* highLightNode);
	void RemoveHighlight();
	Ray ShootRay(Display::Window* renderWindow, Camera* cam);
	float IntersectsWithPhysicsObject(Ray ray, PhysicsObject* gameObj, glm::mat4 viewMatrix);

	std::vector<PhysicsObject*> ignoreObjects;

private:
	std::tuple<double, double> GetCursorPos(GLFWwindow* glfwWindow);
	std::tuple<double, double> GetCursorPosPercent(Display::Window* renderWindow);
	std::tuple<double, double> GetCursorPosNDC(Display::Window* renderWindow);
	std::tuple<glm::vec3, glm::vec3> UpdateAABB(triangle triangle, glm::vec3 minAABB, glm::vec3 maxAABB);
	bool PointInsideTriangle(triangle triangle, glm::vec3 planePoint);
};