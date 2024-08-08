#pragma once
#include "config.h"
#include "ShaderResource.h"
#include <iostream>

ShaderResource::ShaderResource() {
	this->vertexShader = -1;
	this->fragmentShader = -1;
	this->shaderProgram = new GLuint(-1);
	
}

ShaderResource::~ShaderResource() {
	//if(this->vertexShader != -1)
	//	glDeleteShader(this->vertexShader);
	//if (this->fragmentShader != -1)
	//	glDeleteShader(this->fragmentShader);
	//if (*this->shaderProgram.get() != -1)
	//	glDeleteProgram(*this->shaderProgram.get());
	delete this->shaderProgram;
}


GLuint ShaderResource::SetupShader(const GLchar* input, GLuint type) {

	// setup shader shader
	GLuint returnInt = glCreateShader(type);
	GLint length = static_cast<GLint>(std::strlen(input));
	glShaderSource(returnInt, 1, &input, &length);
	glCompileShader(returnInt);

	// get error log
	GLint shaderLogSize;
	glGetShaderiv(returnInt, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(returnInt, shaderLogSize, NULL, buf);
		printf("[SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}
	return returnInt;
}

std::string ShaderResource::LoadShaderFromFile(const char* filePath) {
	std::string line;
	std::ifstream shaderFile;
	std::stringstream stringStream;

	shaderFile.open(filePath);
	if (shaderFile.is_open()) {
		//start reading file
		while (getline(shaderFile, line)) {
			stringStream << line << "\n"; //Write current line into shader string
		}
		shaderFile.close();
		return stringStream.str();
	}
	else 
		std::cout << "Unable to open file.\n";

	return "ERROR";
}

GLuint ShaderResource::CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	GLint shaderLogSize;
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &shaderLogSize);
	//Get error log
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(shaderProgram, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s", buf);
		delete[] buf;
	}

	return shaderProgram;
}

void ShaderResource::UseProgram(GLuint* shaderProgram) {
	if (shaderProgram != nullptr) {
		GLuint test = *shaderProgram;
		glUseProgram(*shaderProgram);
	}
	else
		glUseProgram(0);
}

GLuint ShaderResource::FetchUniform(GLuint* program, const char* uniformName) {
	return glGetUniformLocation(*program, uniformName);
}

//Uniform Edit Functions
void ShaderResource::EditUniformMat4fv(GLuint* program, const char* uniformName, glm::mat4 data) {
	glUniformMatrix4fv(FetchUniform(program, uniformName), 1, GL_FALSE, &data[0][0]);
}

void ShaderResource::EditUniform4fv(GLuint* program, const char* uniformName, glm::vec4 data) {
	glUniform4fv(FetchUniform(program, uniformName), 1, &data[0]);
}

void ShaderResource::EditUniform3fv(GLuint* program, const char* uniformName, glm::vec3 data) {
	glUniform3fv(FetchUniform(program, uniformName), 1, &data[0]);
}


void ShaderResource::EditUniform1i(GLuint* program, const char* uniformName, GLuint value) {
	glUniform1i(FetchUniform(program, uniformName), value);
}

void ShaderResource::EditUniform1f(GLuint* program, const char* uniformName, float data) {
	glUniform1f(FetchUniform(program, uniformName), data);
}

