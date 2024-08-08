#pragma once
#include "config.h"
#include "core/graphicscourse/shaderResource/ShaderResource.h"

class DirLight {
public:
	DirLight();
	~DirLight();

	glm::vec3 direction;
	glm::vec4 colour;
	float intensity;

	void ApplyLight(GLuint* shaderProgram);
};