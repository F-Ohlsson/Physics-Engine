#pragma once
#include "config.h"
#include "core/app.h"
#include "core/graphicscourse/shaderResource/ShaderResource.h"
#include "core/graphicscourse/meshResource/MeshResource.h"


class PointLight {
public:
	PointLight();
	~PointLight();

	glm::vec3 pos;
	glm::vec4 colour;

	float intensity = 1.0f;
	
	void ApplyLight(ShaderResource* shaderResource);
};