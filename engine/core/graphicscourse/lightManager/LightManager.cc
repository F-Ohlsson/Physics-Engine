#pragma once
#include "config.h"
#include "core/graphicscourse/lightManager/LightManager.h"
#include "core/graphicscourse/camera/Camera.h"

LightManager::LightManager() {

}

void LightManager::ApplyLight(ShaderResource* shaderResource, glm::mat4 viewMatrix, glm::mat4 projMatrix) {
	shaderResource->UseProgram(shaderResource->shaderProgram);
	shaderResource->EditUniform1i(shaderResource->shaderProgram, "numLights", this->pointLightV.size());
	GraphicsNode tempNode;
	for (int i = 0; i < this->pointLightV.size(); i++) {
		tempNode.position = this->pointLightV[i].pos;
		tempNode.meshVector.push_back(this->sphere);
		//tempNode.DrawTris(viewMatrix, projMatrix, 0.1f);
		std::string arrayString = "pointLights[" + std::to_string(i) + std::string("]");
		shaderResource->UseProgram(shaderResource->shaderProgram);
		shaderResource->EditUniform3fv(shaderResource->shaderProgram, std::string(arrayString + ".pos").c_str(), this->pointLightV[i].pos);
		shaderResource->EditUniform4fv(shaderResource->shaderProgram, std::string(arrayString + ".colour").c_str(), this->pointLightV[i].colour);
		shaderResource->EditUniform1f(shaderResource->shaderProgram, std::string(arrayString + ".intensity").c_str(), this->pointLightV[i].intensity);
	}

	//I don't get it
	tempNode.DrawTris(viewMatrix, projMatrix, 0.1f);
	this->dirLight.ApplyLight(shaderResource->shaderProgram);
}

void LightManager::ApplyDeferredDirLight(GLuint* shaderProgram) {
	this->dirLight.ApplyLight(shaderProgram);
}

//void LightManager::ApplyDeferredPointLight(std::shared_ptr<GLuint> shaderProgram, mat4 viewMatrix, mat4 projMatrix) {
//	glDepthFunc(GL_EQUAL);
//	glCullFace(GL_FRONT);
//	glDepthMask(GL_FALSE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE);
//	ShaderResource shadR;
//	shadR.UseProgram(shaderProgram);
//
//
//	for (int i = 0; i < this->pointLightV.size(); i++) {
//		PointLight currentLight = this->pointLightV[i];
//		glm::vec4 currentLightColour = currentLight.colour;
//		float maxVal = 0;
//		for (int j = 0; j < 3; j++) {
//			if (currentLightColour[j] > maxVal)
//				maxVal = currentLightColour[j];
//		}	
//
//		float lightRadius = sqrt((maxVal / 254) * 1500);
//
// 		shadR.EditUniform1f(shaderProgram, "lightRadius", lightRadius);
//		shadR.EditUniform3fv(shaderProgram, "lightPos", currentLight.pos);
//		shadR.EditUniform4fv(shaderProgram, "lightColour", currentLight.colour);
//		shadR.EditUniform1f(shaderProgram, "lightIntensity", currentLight.intensity);
//
//		glBindVertexArray(this->sphere->primitiveVector[0].vertexArray);
//		glDrawElements(GL_TRIANGLES, this->sphere->primitiveVector[0].numIndices, this->sphere->primitiveVector[0].indexType, (void*)this->sphere->primitiveVector[0].indexOffset);
//		glBindVertexArray(0);
//	}
//
//	glDisable(GL_BLEND);
//	glDepthFunc(GL_LESS);
//	glCullFace(GL_BACK);
//	glDepthMask(GL_TRUE);
//}

void LightManager::AddPointLight(PointLight newPL) {
	this->pointLightV.push_back(newPL);
}

void LightManager::AddPointLight(glm::vec3 position, glm::vec4 colour, float intensity) {
	PointLight newPL;
	newPL.pos = position;
	newPL.colour = colour;
	newPL.intensity = intensity;

	this->pointLightV.push_back(newPL);
}

void LightManager::DrawPointLights(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
	GraphicsNode tempNode;
	ShaderResource shadR;
	tempNode.meshVector.push_back(this->sphere);
	for (int i = 0; i < this->pointLightV.size(); i++) {
		tempNode.position = this->pointLightV[i].pos;
		tempNode.DrawTris(viewMatrix, projMatrix, 0.1f);
		shadR.UseProgram(this->sphere->primitiveVector[0].material->shaderProgram);
		shadR.EditUniform4fv(this->sphere->primitiveVector[0].material->shaderProgram, "colour", this->pointLightV[i].colour);
		this->sphere->primitiveVector[0].material->ApplyMaterial();
	}
}

void LightManager::NewDirLight(DirLight newDL) {
	this->dirLight = newDL;
}

void LightManager::NewDirLight(glm::vec3 direction, glm::vec4 colour, float intensity) {
	DirLight newDL;
	newDL.direction = direction;
	newDL.colour = colour;
	newDL.intensity = intensity;
	this->dirLight = newDL;
}

void LightManager::UpdatePositions(float x, float y, float z) {
	for (int i = 0; i < this->pointLightV.size(); i++) {
		this->pointLightV[i].pos.x += (x);
		this->pointLightV[i].pos.y += (y);
		this->pointLightV[i].pos.z += (z);
	}
}

void LightManager::SetUp() {
	this->sphere = new MeshResource();
	this->sphere->LoadOBJFile("../resources/meshes/sphere.obj");
}