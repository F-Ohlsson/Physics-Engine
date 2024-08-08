#pragma once
#include "config.h"
#include "core/graphicscourse/material/Material.h"
#include "core/graphicscourse/shaderResource/ShaderResource.h"
#include "core/graphicscourse/textureResource/TextureResource.h"
#include <string>
//#include <memory>
#include <vector>


class DefaultMaterial : public Material {
public:
	DefaultMaterial();
	DefaultMaterial(GLuint* shaderProgram);
	~DefaultMaterial();
	
	GLuint* vertexShader;
	GLuint* fragmentShader;
	GLuint* texture;

	glm::vec4 colour;
	
	void ApplyMaterial();
	void ApplyMaterial(std::vector<GLuint> textures);
	void ApplyMaterial(GLuint* shaderProgram);
	void UnbindMaterial();


};