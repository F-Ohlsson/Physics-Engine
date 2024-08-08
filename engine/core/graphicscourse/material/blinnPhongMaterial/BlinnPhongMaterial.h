#pragma once
#include "config.h"
#include "core/graphicscourse/material/Material.h"
#include "core/graphicscourse/shaderResource/ShaderResource.h"
#include "core/graphicscourse/textureResource/TextureResource.h"
#include <string>
//#include <memory>
#include <vector>


class BlinnPhongMaterial: public Material {
public:
	BlinnPhongMaterial();
	BlinnPhongMaterial(GLuint* shaderProgram);
	~BlinnPhongMaterial();

	GLuint* vertexShader;
	GLuint* fragmentShader;
	GLuint* texture;

	float shininess = 10.0f;
	glm::vec4 ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.0f, 0.0f, 0.5f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	void ApplyMaterial();
	void ApplyMaterial(std::vector<GLuint> textures);
	void ApplyMaterial(GLuint* shaderProgram);
	void UnbindMaterial();


};