#pragma once
#include "config.h"
#include "core/app.h"
//#include <memory>
#include <vector>

class Material {
public:
	GLuint* shaderProgram;

	virtual void ApplyMaterial() = 0;
	virtual void ApplyMaterial(std::vector<GLuint> textures) = 0;
	virtual void ApplyMaterial(GLuint* shaderProgram) = 0;
	virtual void UnbindMaterial() = 0;

};