#pragma once
#include "config.h"
#include "core/app.h"
#include <string>
#include <fstream>
#include <sstream>

class ShaderResource{
public:
	ShaderResource();
	~ShaderResource();

	ShaderResource& operator=(ShaderResource const& rhs);

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint* shaderProgram = nullptr;


	GLuint SetupShader(const GLchar* input, GLuint type);
	std::string LoadShaderFromFile(const char* filePath);

	GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
	void UseProgram(GLuint* shaderProgram);

	GLuint FetchUniform(GLuint* program, const char* uniformName);
	void EditUniformMat4fv(GLuint* program, const char* uniformName, glm::mat4 data);
	void EditUniform4fv(GLuint* program, const char* uniformName, glm::vec4 data);
	void EditUniform3fv(GLuint* program, const char* uniformName, glm::vec3 data);
	void EditUniform1i(GLuint* program, const char* uniformName, GLuint textureLocation = 0);
	void EditUniform1f(GLuint* program, const char* uniformName, float data);
};