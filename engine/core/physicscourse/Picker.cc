#pragma once
#include "config.h"
#include "Picker.h"

Picker::Picker() {
}

Picker::~Picker() {

}

//Change texture and AABB parameters
void Picker::AddHighlight(GraphicsNode* highlightNode) {
	for (int j = 0; j < highlightNode->meshVector.size(); j++) {
		MeshResource* mesh = highlightNode->meshVector[j];
		for (int k = 0; k < mesh->primitiveVector.size(); k++) {
			mesh->primitiveVector[k].material = highlightMaterial;
		}
	}

	if (this->currentHighlight != nullptr) {
		Picker::RemoveHighlight();
	}

	this->currentHighlight = highlightNode;
	this->currentHighlight->parent->physN->AABBcolor = glm::vec4(0, 1, 0, 1);
	this->currentHighlight->parent->physN->AABBlineWeight = 4.f;
}


//Reset texture and AABB parameters
void Picker::RemoveHighlight() {
	if (this->currentHighlight == nullptr)
		return;

	for (int j = 0; j < currentHighlight->meshVector.size(); j++) {
		MeshResource* mesh = currentHighlight->meshVector[j];
		for (int k = 0; k < mesh->primitiveVector.size(); k++) {
			mesh->primitiveVector[k].material = baseMaterial;
		}
	}
	this->currentHighlight->parent->physN->AABBcolor = glm::vec4(1, 1, 1, 1);
	this->currentHighlight->parent->physN->AABBlineWeight = 1.f;
	this->currentHighlight = nullptr;
}

//Return ray in world that passes through camera and position clicked on the screen
Ray Picker::ShootRay(Display::Window* renderWindow, Camera* cam) {
	std::tuple<double, double> mousePos = Picker::GetCursorPosNDC(renderWindow);

	//Depth bounds of camera view
	double nearPlane = cam->projection[3][2] / (cam->projection[2][2] - 1);
	double farPlane = cam->projection[3][2] / (cam->projection[2][2] + 1);

	//Convert screen position to world position
	glm::mat4 viewProjection = cam->projection * cam->view;
	glm::mat4 inverseViewProjection = glm::inverse(viewProjection);
	glm::vec4 screenSpaceMousePos = glm::vec4(get<0>(mousePos), -1*get<1>(mousePos), -nearPlane, 1);
	
	glm::vec4 worldSpaceMousePos = inverseViewProjection * screenSpaceMousePos; 
	worldSpaceMousePos.w = 1.0f / worldSpaceMousePos.w;
	glm::vec3 worldPos = glm::vec3(worldSpaceMousePos.x * worldSpaceMousePos.w, worldSpaceMousePos.y * worldSpaceMousePos.w, worldSpaceMousePos.z * worldSpaceMousePos.w);

	Ray ray = Ray(cam->position, worldPos);

	return ray;
}

//Checks if any object intersects with the ray provided
float Picker::IntersectsWithPhysicsObject(Ray ray, PhysicsObject* gameObj, glm::mat4 viewMatrix) {
	for (int i = 0; i < ignoreObjects.size(); i++) {
		if (ignoreObjects[i] == gameObj) {
			return std::numeric_limits<float>::lowest();
		}
	}

	glm::vec3 intersectionPoint = glm::vec3(std::numeric_limits<float>::lowest(),0,0);
	glm::vec3 testPoint = glm::vec3(0,0,0);

	//First check intersection with AABB
	for (int i = 0; i < 11; i++) {
		triangle triangle = gameObj->physN->AABB[i];

		Plane plane = Plane(triangle.vertexPos[0], triangle.vertexPos[1], triangle.vertexPos[2]);
		float distance = ray.PlaneIntersectionDistance(plane);


		glm::vec4 color = glm::vec4(0, 0, 1, 1);

		if (distance >= 0) {
			glm::vec3 planeIntersection = ray.PlaneIntersectionPoint(plane);

			if (PointInsideTriangle(triangle, planeIntersection)) {
				intersectionPoint = planeIntersection;
				float scale = 0.2f;
				Debug::DrawBox(intersectionPoint * (1 / scale), glm::quat(1, 0, 0, 0), scale, glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop, 1.f);
			}
		}
	}
	if (intersectionPoint.x == std::numeric_limits<float>::lowest()) { //If no collision, reset AABB visualisation and exit early
		gameObj->physN->AABBcolor = glm::vec4(1, 1, 1, 1);
		gameObj->physN->AABBlineWeight = 1.f;
		return std::numeric_limits<float>::max();
	}
	intersectionPoint = glm::vec3(std::numeric_limits<float>::lowest(), 0, 0); //If collision, show collision with AABB
	gameObj->physN->AABBcolor = glm::vec4(1, 1, 0, 1);
	gameObj->physN->AABBlineWeight = 2.f;


	glm::mat4 modelMatrix = (gameObj->graphN->GetCompleteModelMatrix());
	//If collision with AABB, proceed to check collision with object collider triangles
	for (int i = 0; i < gameObj->physN->colliderTriangles.size(); i++) {
		triangle modelTri = gameObj->physN->colliderTriangles[i];

		glm::mat4 invModelMat = glm::inverse(modelMatrix);
		glm::mat4 transpInvModelMat = glm::inverse(glm::transpose(invModelMat));

		//Translate world space ray to local space ray since object collider is in local space
		Ray localRay;
		glm::vec4 localStartPos = invModelMat * glm::vec4(ray.startPos.x, ray.startPos.y, ray.startPos.z,1);
		localRay.startPos = glm::vec3(localStartPos.x, localStartPos.y, localStartPos.z);
		glm::vec4 localDirection = transpInvModelMat * glm::vec4(ray.direction.x, ray.direction.y, ray.direction.z, 0);
		localRay.direction = glm::vec3(localDirection.x, localDirection.y, localDirection.z);


		//Plane intersection in local space
		Plane plane = Plane(modelTri.vertexPos[0], modelTri.vertexPos[1], modelTri.vertexPos[2]);
  		float distance = localRay.PlaneIntersectionDistance(plane);
		float scale = 1.f;
				
		//If ray intersects plane set up by triangle
		if (distance >= 0) {
			glm::vec3 testPoint = localRay.PlaneIntersectionPoint(plane);
			if (PointInsideTriangle(modelTri, testPoint)) {
				intersectionPoint = testPoint; 
				break;
			}
		}
	}

	//If point is inside a triangle we have a hit
	if(intersectionPoint.x != std::numeric_limits<float>::lowest()){
		glm::vec4 worldIntersection = modelMatrix * glm::vec4(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z, 1);
		glm::vec3 distanceRay = glm::vec3(worldIntersection.x, worldIntersection.y, worldIntersection.z) - ray.startPos;
		float distance = glm::length(glm::vec3(distanceRay.x,distanceRay.y,distanceRay.z));
		return distance;
	}

	return std::numeric_limits<float>::lowest();
}


std::tuple<double,double> Picker::GetCursorPos(GLFWwindow* glfwWindow) {
	double xPos, yPos;
	glfwGetCursorPos(glfwWindow, &xPos, &yPos);
	return std::make_tuple(xPos, yPos);;
}

//Get cursor coordinates in percentage of the total screen height/width
std::tuple<double, double> Picker::GetCursorPosPercent(Display::Window* renderWindow) {
	//Get mouse coordinates and window height and width
	std::tuple<double, double> mouseCoord = Picker::GetCursorPos(renderWindow->window);
	int32 winW, winH;
	renderWindow->GetSize(winW, winH);

	double xPosPercent, yPosPercent;
	xPosPercent = (get<0>(mouseCoord) / winW);
	yPosPercent = get<1>(mouseCoord) / winH;

	return std::make_tuple(xPosPercent, yPosPercent);
}

//Get cursor coordinates in Normalised Device Coordinates
std::tuple<double, double> Picker::GetCursorPosNDC(Display::Window* renderWindow) {
	//Get mouse coordinates and window height and width
	std::tuple<double, double> mousePercent = Picker::GetCursorPosPercent(renderWindow);

	double xPosNDC = ((get<0>(mousePercent) * 2) - 1);
	double yPosNDC = ((get<1>(mousePercent) * 2) - 1);

	return std::make_tuple(xPosNDC, yPosNDC);
}

std::tuple<glm::vec3, glm::vec3> Picker::UpdateAABB(triangle triangle, glm::vec3 minAABB, glm::vec3 maxAABB) {
	glm::vec3 newMin = minAABB;
	glm::vec3 newMax = maxAABB;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			newMin.x = std::min(triangle.vertexPos[j].x, newMin.x);
			newMin.y = std::min(triangle.vertexPos[j].y, newMin.y);
			newMin.z = std::min(triangle.vertexPos[j].z, newMin.z);

			newMax.x = std::max(triangle.vertexPos[j].x, newMax.x);
			newMax.y = std::max(triangle.vertexPos[j].y, newMax.y);
			newMax.z = std::max(triangle.vertexPos[j].z, newMax.z);
		}
	}

	return std::make_tuple(newMin, newMax);
}


//READ UP AND UNDERSTAND BETTER
bool Picker::PointInsideTriangle(triangle triangle, glm::vec3 planePoint) {


	//Calculate the stuff here
	//USE BARYCENTRIC COORDINATES

	glm::vec3 triA = triangle.vertexPos[0];
	glm::vec3 triB = triangle.vertexPos[1];
	glm::vec3 triC = triangle.vertexPos[2];

	glm::vec3 triAB = triB - triA;
	glm::vec3 triAC = triC - triA;

	if (false) {
		//Heavily inspired by code found in Real-Time Collision Detection (Christer Ericson)
		glm::vec3 vecAP = planePoint - triA;

		float dotABAB = glm::dot(triAB, triAB);
		float dotABAC = glm::dot(triAB, triAC);
		float dotACAC = glm::dot(triAC, triAC);
		float dotAPAB = glm::dot(vecAP, triAB);
		float dotAPAC = glm::dot(vecAP, triAC);

		float denominator = dotABAB * dotACAC - dotABAC * dotABAC;

		float v = (dotACAC * dotAPAB - dotABAC * dotAPAC) / denominator;
		float w = (dotABAB * dotAPAC - dotABAC * dotAPAB) / denominator;

		float u = 1.0f - v - w;

		glm::vec3 baryCoord = glm::vec3(v, w, u);

		if (v < 0 || w < 0 || u < 0)
		{
			return false;
		}
		else if (v > 1 || w > 1 || u > 1) {
			return false;
		}

	}


	if (true) {
		bool success = true;
		//Based on
		glm::vec3 triangleNormal = glm::cross(triAB, triAC);
		float triangleArea = glm::length(triangleNormal) / 2;

		glm::vec3 C;

		//calculate w
		glm::vec3 vecPA = planePoint - triA;
		C = glm::cross(triAB, vecPA);
		float w = (glm::length(C) / 2) / triangleArea;
		if (glm::dot(triangleNormal, C) < 0) {
			success = false;
		}

		//calculate u
		glm::vec3 triBC = triC - triB;
		glm::vec3 vecPB = planePoint - triB;
		C = glm::cross(triBC, vecPB);
		float u = (glm::length(C) / 2) / triangleArea;
		if (glm::dot(triangleNormal, C) < 0) {
			success = false;
		}

		//calculate v
		glm::vec3 triCA = triA - triC;
		glm::vec3 vecPC = planePoint - triC;
		C = glm::cross(triCA, vecPC);
		float v = (glm::length(C) / 2) / triangleArea;
		if (glm::dot(triangleNormal, C) < 0) {
			success = false;
		}

		if (!success) {
			//Debug::DrawLine(triangle.vertexPos[0].ToGlmVec3(), planePoint.ToGlmVec3(), 1, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			return false;
		}
		else {
			int i = 0;
		}
	}

	return true;
}