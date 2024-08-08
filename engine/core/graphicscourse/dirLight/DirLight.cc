#pragma once
#include "config.h"
#include "core/graphicscourse/dirLight/DirLight.h"

DirLight::DirLight() {
}

DirLight::~DirLight() {

}

void DirLight::ApplyLight(GLuint* shaderProgram) {
	ShaderResource shadR;
	shadR.UseProgram(shaderProgram);
	shadR.EditUniform1f(shaderProgram, "dirLightIntensity", this->intensity);
	shadR.EditUniform3fv(shaderProgram, "dirLightDirection",- this->direction);
	shadR.EditUniform4fv(shaderProgram, "dirLightColour", this->colour);
	shadR.UseProgram(0);

}