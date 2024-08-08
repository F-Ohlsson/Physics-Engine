#pragma once
#include "config.h"
#include "DefaultMaterial.h"

DefaultMaterial::DefaultMaterial() {
	//
	//How to make sure it's not already loaded?
	//

	ShaderResource shadR = ShaderResource();
	std::string shaderStringVertex = shadR.LoadShaderFromFile("../resources/shaders/DefaultVertexShader.glsl");
	this->vertexShader = new GLuint(shadR.SetupShader(shaderStringVertex.c_str(), GL_VERTEX_SHADER));

	std::string shaderStringFragment = shadR.LoadShaderFromFile("../resources/shaders/DefaultFragmentShader.glsl");
	this->fragmentShader = new GLuint(shadR.SetupShader(shaderStringFragment.c_str(), GL_FRAGMENT_SHADER));

	this->shaderProgram = new GLuint(shadR.CreateProgram(*this->vertexShader, *this->fragmentShader));

	TextureResource textR = TextureResource();
	textR.LoadTexture("../resources/textures/default.png", false);
	
	this->texture = new GLuint(textR.texture);

}

DefaultMaterial::DefaultMaterial(GLuint* shaderProgram) {
	GLuint shaders[2]; //How many shaders to fetch from program
	glGetAttachedShaders(*shaderProgram, 2, 0, shaders);

	this->vertexShader = new GLuint(shaders[0]);
	this->fragmentShader = new GLuint(shaders[1]);
	this->shaderProgram = shaderProgram;
}


DefaultMaterial::~DefaultMaterial() {

}

void DefaultMaterial::ApplyMaterial() {
	ShaderResource shadR;
	TextureResource textR;
 	shadR.UseProgram(this->shaderProgram);
	//shadR.EditUniform1f(this->shaderProgram, "shininess", this->shininess);
	if(this->texture != NULL)
		textR.BindTexture(*this->texture);
	shadR.EditUniform1i(this->shaderProgram, "tex");
	//shadR.EditUniform4fv(this->shaderProgram, "materialAmbientColour", this->ambient);
	//shadR.EditUniform4fv(this->shaderProgram, "materialDiffuseColour", this->diffuse);
	//shadR.EditUniform4fv(this->shaderProgram, "materialSpecularColour", this->specular);
}

void DefaultMaterial::ApplyMaterial(std::vector<GLuint> textures) {
	int i = 0;
}

void DefaultMaterial::ApplyMaterial(GLuint* shaderProgram) {
	int i = 0;
}


void DefaultMaterial::UnbindMaterial() {
	ShaderResource* shadR = new ShaderResource();
	TextureResource* textR = new TextureResource();
	textR->BindTexture(0);
	shadR->UseProgram(0);

	delete shadR;
	delete textR;
}
