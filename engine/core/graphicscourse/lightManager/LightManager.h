#pragma once
#include "config.h"
#include "core/graphicscourse/dirLight/DirLight.h"
#include "core/graphicscourse/pointLight/PointLight.h"
#include "core/graphicscourse/graphicsNode/GraphicsNode.h";
#include "core/graphicscourse/material/defaultMaterial/DefaultMaterial.h"
#include <vector>

class LightManager {
public:
		LightManager();

		MeshResource* sphere;

		void ApplyLight(ShaderResource* shaderResource, glm::mat4 viewMatrix, glm::mat4 projMatrix);
		void ApplyDeferredDirLight(GLuint* shaderProgram );
		//void ApplyDeferredPointLight(std::shared_ptr<GLuint> shaderProgram, mat4 viewMatrix, mat4 projMatrix);
		void AddPointLight(PointLight newPL);
		void AddPointLight(glm::vec3 position, glm::vec4 colour, float intensity);
		void DrawPointLights(glm::mat4 viewMatrix, glm::mat4 projMatrix);
		void NewDirLight(DirLight newDL);
		void NewDirLight(glm::vec3 direction, glm::vec4 colour, float intensity);

		void UpdatePositions(float x, float y, float z);
		void SetUp();
private:
	std::vector<PointLight> pointLightV;
	DirLight dirLight;
};