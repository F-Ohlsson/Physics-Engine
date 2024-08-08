#pragma once
#include "config.h"
#include "core/app.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
//#include <memory>
#include "core/graphicscourse/material/Material.h"

struct stringVec3 {
	std::string v[3];
	std::string originalString = "";
};

struct vertex {
	glm::vec3 pos = glm::vec3();
	float uv_s = 0.0f;
	float uv_t = 0.0f;
	glm::vec3 normal = glm::vec3();
};

struct vertexConfig {
	int v;
	int vt;
	int vn;
	int bufferIndex;
};

struct primitive {
	GLuint vertBuffer = -1;
	GLuint textureBuffer = -1;
	GLuint normalBuffer = -1;
	GLuint indexBuffer = -1;
	GLuint vertexArray = -1;

	unsigned int numIndices = -1;
	GLenum indexType = GL_UNSIGNED_INT;
	GLuint indexOffset = 0;

	Material* material;
	std::vector<GLuint> textures;
};

class MeshResource {
public:
	MeshResource();
	~MeshResource();

	std::vector<primitive> primitiveVector;
	
	void BindBuffer(GLuint buffer);
	void BindIndexBuffer(primitive primitive);
	void UnbindIndexBuffer();
	void BindVertexArray(primitive primitive);
	void UnbindVertexArray();
	void SetupTris();
	void SetupQuad(float sizeX, float sizeY);
	void SetupFullScreenQuad(float sizeX, float sizeY);
	void SetupCube(float sizeX, float sizeY, float sizeZ);
	void DrawQuads();
	void DrawFullScreenQuad();
	void DrawTris(primitive prim);
	void LoadOBJFile(std::string filepath);
	glm::vec3 ProcessLineVec3(std::string line, std::string delimiter);
	stringVec3 ProcessLineStringVec3(std::string line, std::string delimiter);
};
