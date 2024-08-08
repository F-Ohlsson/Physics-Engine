#pragma once
#include "config.h"
//#include <memory>
#include "core/graphicsCourse/meshResource/MeshResource.h"
#include "core/graphicsCourse/textureResource/TextureResource.h"
#include "core/graphicsCourse/shaderResource/ShaderResource.h"
#include "core/physicscourse/PhysicsObject.h"
#include <nlohmann/json.hpp>
#include <fx/gltf.h>
#include <vector>

class PhysicsObject;
class Camera;

class GraphicsNode {
public:
	GraphicsNode();
	GraphicsNode(const GraphicsNode&);
	~GraphicsNode();
	GraphicsNode(GraphicsNode&& move) noexcept;

	glm::vec3 position = glm::vec3();
	float radiansX = 0.0f; //Keeps track of current rotation along X axis
	float radiansY = 0.0f; //Keeps track of current rotation along Y axis
	float radiansZ = 0.0f;
	float scaling;

	PhysicsObject* parent = nullptr;

	glm::mat4 rotationMatrix = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1)
	);

	//Legacy, will break old projects if removed
	MeshResource* meshR;
	TextureResource* textR;
	ShaderResource* shadR;
	//

	std::vector<MeshResource*> meshVector;
	std::vector<TextureResource*> textureVector;
	std::vector<ShaderResource*> shaderVector;



	GraphicsNode& operator =(GraphicsNode&&) noexcept;
	//void DrawQuads(mat4 viewProjection); //Doesn't use index buffer or vertex array. It's not good
	//void DrawTrisDeferred(mat4 view, mat4 projection, float scaling, std::shared_ptr<GLuint> geometryShaderProgram);
	void DrawTris(glm::mat4 view, glm::mat4 projection, float scaling); //Uses index buffer and vertex array. Seems better
	void LoadGLTF(std::string filename);
	std::string GetAccessorString(int attribArrayIndex);
	void Tick(float deltaTime, Camera cam, bool paused);
	void TickPhysics(float deltaTime, bool paused);
	glm::mat4 GetCompleteModelMatrix();
};