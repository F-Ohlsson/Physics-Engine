#pragma once
#include "config.h"
#include "GraphicsNode.h"
#include "render/stb_image.h"
#include "render/debugrender.h"

GraphicsNode::GraphicsNode() {
	radiansX = 0.0f;
	radiansY = 0.0f;
	this->textR = new TextureResource(TextureResource::TextureResource());
	this->shadR = new ShaderResource(ShaderResource::ShaderResource());
}

GraphicsNode::GraphicsNode(const GraphicsNode& gN) {
	this->meshR = gN.meshR;
	this->meshVector = gN.meshVector;
	this->position = glm::vec3(gN.position.x, gN.position.y, gN.position.z);
	this->radiansX = gN.radiansX;
	this->radiansY = gN.radiansY;
	this->shaderVector = gN.shaderVector;
	this->shadR = gN.shadR;
	this->textR = gN.textR;
	this->textureVector = gN.textureVector;
	this->scaling = gN.scaling;

	this->parent = gN.parent;
	this->rotationMatrix = gN.rotationMatrix;
}

GraphicsNode::~GraphicsNode() {
	//TODO DELETE STUFF
}

 GraphicsNode::GraphicsNode(GraphicsNode&& move) noexcept{

}

GraphicsNode& GraphicsNode::operator=(GraphicsNode&& rhs) noexcept {
	return rhs;
}

//
//void GraphicsNode::DrawQuads(mat4 viewProjection) {
//	//Cycle through meshes
//	for (int i = 0; i < this->meshVector.size(); i++) {
//		MeshResource* currentMesh = this->meshVector[i].get();
//
//		//Cycle through primitives
//		for (int j = 0; j < currentMesh->primitiveVector.size(); j++) {
//			primitive currentPrim = currentMesh->primitiveVector[j];
//			currentMesh->BindBuffer(currentPrim.vertBuffer);
//			this->textR->BindTexture(this->textR->texture);
//
//			//Prepare model matrix
//			mat4 modelMatrix = mat4(
//				{ 1,0,0,0 },
//				{ 0,1,0,0 },
//				{ 0,0,1,0 },
//				{ position.x,position.y,position.z,1 }
//			);
//			modelMatrix = modelMatrix * MathlibMat4::RotationX(this->radiansX) * MathlibMat4::RotationY(this->radiansY);
//			mat4 mvp = viewProjection * modelMatrix;
//
//			//Send mvp to shader
//			this->shadR->UseProgram(this->shadR->shaderProgram);
//			this->shadR->EditUniformMat4fv(this->shadR->shaderProgram, "mvp", MathlibMat4::ToGlmMat4(mvp));
//			//Send texture to shader
//			this->shadR->EditUniform1i(this->shadR->shaderProgram, "tex");
//
//			this->meshR->DrawQuads();
//
//			this->meshR->BindBuffer(0);
//			this->textR->BindTexture(0);
//			this->shadR->UseProgram(0);
//		}
//
//
//	}
//	
//}
//
//void GraphicsNode::DrawTrisDeferred(mat4 view, mat4 projection, float scaling, std::shared_ptr<GLuint> geometryShaderProgram) {
//	ShaderResource shadR;
//	for (int i = 0; i < meshVector.size(); i++) { //For each mesh
//		MeshResource* currentMesh = meshVector[i].get();
//		for (int j = 0; j < currentMesh->primitiveVector.size(); j++) { //For each primitive
//			primitive const currentPrim = currentMesh->primitiveVector[j];
//
//			//Prepare model matrix
//			mat4 modelMatrix = mat4(
//				{ scaling,0,0,0 },
//				{ 0,scaling,0,0 },
//				{ 0,0,scaling,0 },
//				{ position.x,position.y,position.z,1 }
//			);
//
//			modelMatrix = modelMatrix * MathlibMat4::RotationX(this->radiansX) * MathlibMat4::RotationY(this->radiansY);
//			mat4 mvp = projection * view * modelMatrix;
//
//			glm::vec3 camPos = glm::vec3(view[3][0], view[3][1], view[3][2]);
//			shadR.UseProgram(geometryShaderProgram);
//
//			std::shared_ptr<GLuint> shaderProgram = geometryShaderProgram;
//
//			std::vector<GLuint> textures = currentPrim.textures;
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, textures[0]);
//			shadR.EditUniform1i(shaderProgram, "tex", 0);
//			if (textures.size() > 1) {
//				glActiveTexture(GL_TEXTURE0 + 1);
//				glBindTexture(GL_TEXTURE_2D, textures[1]);
//				shadR.EditUniform1i(shaderProgram, "normalMap", 1);
//			}
//
//			shadR.EditUniformMat4fv(shaderProgram, "mvp", MathlibMat4::ToGlmMat4(mvp));
//			shadR.EditUniformMat4fv(shaderProgram, "m", MathlibMat4::ToGlmMat4(modelMatrix));
//			shadR.EditUniform3fv(shaderProgram, "camPos", camPos);
//			shadR.EditUniformMat4fv(shaderProgram, "mTransposedInverse", MathlibMat4::ToGlmMat4(MathlibMat4::Transpose(MathlibMat4::Inverse(modelMatrix))));
//
//
//			this->meshVector[i]->DrawTris(currentPrim);
//			shadR.UseProgram(0);
//			glActiveTexture(GL_TEXTURE0);
//		}
//	}
//}

void GraphicsNode::DrawTris(glm::mat4 view, glm::mat4 projection, float scaling) {
	ShaderResource shadR;
	for (int i = 0; i < meshVector.size(); i++) { //For each mesh
		MeshResource* currentMesh = meshVector[i];
		for (int j = 0; j < currentMesh->primitiveVector.size(); j++) { //For each primitive
			primitive const currentPrim = currentMesh->primitiveVector[j];

			glm::mat4 modelMatrix = GetCompleteModelMatrix();
			glm::mat4 mvp = projection * view * modelMatrix;
			
			glm::vec3 camPos = glm::vec3(view[3][0], view[3][1], view[3][2]);

			if (currentPrim.textures.size() != 0)
				currentPrim.material->ApplyMaterial(currentPrim.textures);
			else
				currentPrim.material->ApplyMaterial();

			GLuint* shaderProgram = currentPrim.material->shaderProgram;
			shadR.EditUniformMat4fv(shaderProgram, "mvp", mvp);
			shadR.EditUniformMat4fv(shaderProgram, "m", modelMatrix);
			shadR.EditUniform3fv(shaderProgram, "camPos", camPos);
			shadR.EditUniformMat4fv(shaderProgram, "mTransposedInverse", glm::transpose(glm::inverse(modelMatrix)));


			this->meshVector[i]->DrawTris(currentPrim);
			currentPrim.material->UnbindMaterial();
		}
			
	}
	if (this->parent->physN->AABBscale == glm::vec3(0, 0, 0)) {
		this->parent->physN->SetAABB();
	}
	}

void GraphicsNode::LoadGLTF(std::string filename) {
	fx::gltf::Document doc = fx::gltf::LoadFromText(filename);
	
	//Used if no targets are specified in gltf file
	std::vector<GLuint> targetLocations;

	//Set up buffers
	std::vector<GLuint> buffers;
	buffers.reserve(doc.bufferViews.size());
	for (int i = 0; i < doc.bufferViews.size(); i++) {
		buffers.push_back(0);
	}

	//Buffer all data
	glGenBuffers((GLsizei)doc.bufferViews.size(), &buffers[0]);
	for (int i = 0; i < doc.bufferViews.size(); i++) {
		fx::gltf::BufferView bufferView = doc.bufferViews[i];
		
		GLenum target = (GLenum)bufferView.target;
		
		//Cycle through all primitives to see if current bufferview is used as index buffer anywhere
		if (target == 0) {
			bool indexBuffer = false;
			for (int j = 0; j < doc.meshes.size(); j++) {
				for (int k = 0; k < doc.meshes[j].primitives.size();k++) {
					if (doc.meshes[j].primitives[k].indices == i) {
						indexBuffer = true;
					}
				}
			}
			if (indexBuffer){
				target = GL_ELEMENT_ARRAY_BUFFER;
			}
			else {
				target = GL_ARRAY_BUFFER;
			}
			targetLocations.push_back(target);
		}

		glBindBuffer(target, buffers[i]);

		glBufferData(target, bufferView.byteLength, &(doc.buffers[bufferView.buffer].data[0]) + bufferView.byteOffset, GL_STATIC_DRAW);

	}


	std::vector<triangle> collisionTriangles;
	for (int i = 0; i < doc.meshes.size(); i++) { //For each mesh
		MeshResource* meshR = new MeshResource();
		for (int j = 0; j < doc.meshes[i].primitives.size(); j++) { //For each primitive
			fx::gltf::Primitive currentPrim = doc.meshes[i].primitives[j];
			primitive prim;
			glGenVertexArrays(1, &prim.vertexArray);
			meshR->BindVertexArray(prim);

			//ATTTRIBUTES
			fx::gltf::Attributes attributes = currentPrim.attributes;
			for (int k = 0; k < attributes.size(); k++) {
				GLuint accessorLocation = attributes[this->GetAccessorString(k)]; //Find correct type of value based on current index
				GLuint bufferLocation = doc.accessors[accessorLocation].bufferView;

				GLuint target;
				if (targetLocations.size() == 0) {
					target = GLint(doc.bufferViews[doc.accessors[accessorLocation].bufferView].target);
				}
				else {
					target = targetLocations[bufferLocation];
				}


				//HEAVILY inspired by Physics.cc
				if (this->GetAccessorString(k) == "POSITION") {
					auto indexBufferAccessor = doc.accessors[currentPrim.indices];
					auto indexViewBuffer = doc.bufferViews[indexBufferAccessor.bufferView];
					auto indexBuffer = doc.buffers[indexViewBuffer.buffer];

					uint16_t const* indexBufferData = (uint16_t const*)&indexBuffer.data[indexBufferAccessor.byteOffset + doc.bufferViews[indexBufferAccessor.bufferView].byteOffset]; //Currently only supports this type (UnsignedShort)

					auto vertexBufferAccessor = doc.accessors[attributes[this->GetAccessorString(k)]];
					auto vertexViewBuffer = doc.bufferViews[vertexBufferAccessor.bufferView];
					auto vertexBuffer = doc.buffers[vertexViewBuffer.buffer];
					float const* vertexBufferData = (float const*)&vertexBuffer.data[vertexBufferAccessor.byteOffset + doc.bufferViews[vertexBufferAccessor.bufferView].byteOffset];


					size_t vectorSize = (doc.accessors[accessorLocation].type == fx::gltf::Accessor::Type::Vec3) ? 3 : 4;
					for (uint l = 0; l < indexBufferAccessor.count; l += 3) {
						triangle triangle;
						float test = vertexBufferData[vectorSize * indexBufferData[l]];
						triangle.vertexPos[0] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l]  ],
							vertexBufferData[vectorSize * indexBufferData[l]+1],
							vertexBufferData[vectorSize * indexBufferData[l]+2]
						);
						triangle.vertexPos[1] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l+1]    ],
							vertexBufferData[vectorSize * indexBufferData[l+1] + 1],
							vertexBufferData[vectorSize * indexBufferData[l+1] + 2]
						);
						triangle.vertexPos[2] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l+2]    ],
							vertexBufferData[vectorSize * indexBufferData[l+2] + 1],
							vertexBufferData[vectorSize * indexBufferData[l+2] + 2]
						);
						
						triangle.vertexPos[0];
						triangle.vertexPos[1];
						triangle.vertexPos[2];
						
						triangle.vertexPos[0];
						triangle.vertexPos[1];
						triangle.vertexPos[2];

						glm::vec3 edge1 = triangle.vertexPos[1] - triangle.vertexPos[0];
						glm::vec3 edge2 = triangle.vertexPos[2] - triangle.vertexPos[0];

						triangle.normal = glm::normalize(glm::cross(edge1, edge2));

						collisionTriangles.push_back(triangle);
					}


				}

				glBindBuffer(target, buffers[bufferLocation]);
				GLint type = (GLint)doc.accessors[accessorLocation].type;
				GLenum componentType = (GLenum)doc.accessors[accessorLocation].componentType;
				GLenum normalized = (GLenum)doc.accessors[accessorLocation].normalized;
				GLuint stride = doc.bufferViews[bufferLocation].byteStride;
				GLuint offset = doc.accessors[accessorLocation].byteOffset;

				glEnableVertexAttribArray(k);
				glVertexAttribPointer(k, type, componentType, normalized, stride, (void*)offset);
				glBindBuffer(target, 0);
			}



			//TEXTURES
			if (doc.materials.size() != 0) {

			////If URI
			//fx::gltf::Material primitiveMaterial = doc.materials[currentPrim.material];
			//std::vector<std::string> pathStrings;
			//std::string directoryname = filename.substr(0, filename.find_last_of('/') + 1);

			//GLuint baseColourTextureIndex = primitiveMaterial.pbrMetallicRoughness.baseColorTexture.index;
			//std::string baseColourTexturePath = directoryname + doc.images[baseColourTextureIndex].uri;
			//pathStrings.push_back(baseColourTexturePath);

			//GLuint normalTextureIndex = primitiveMaterial.normalTexture.index;
			//if(normalTextureIndex != -1){
			//	std::string normalTexturePath = directoryname + doc.images[normalTextureIndex].uri;
			//	pathStrings.push_back(normalTexturePath);
			//}


			//for (int i = 0; i < pathStrings.size(); i++) {
			//	int width, height, channels;
			//	unsigned char* data = stbi_load(pathStrings[i].c_str(), &width, &height, &channels, 0);

			//	//Check if fileformat supports RGB/RGBA
			//	GLenum fileChannels = -1;
			//	if (channels == 3) {
			//		fileChannels = GL_RGB;
			//	}
			//	else if (channels == 4) {
			//		fileChannels = GL_RGBA;
			//	}

			//	prim.textures.push_back(0);

			//	glGenTextures(1, &prim.textures.back());
			//	glBindTexture(GL_TEXTURE_2D, prim.textures.back());
			//	if (data) {
			//		glTexImage2D(GL_TEXTURE_2D, 0, fileChannels, width, height, 0, fileChannels, GL_UNSIGNED_BYTE, data);

			//		glGenerateMipmap(GL_TEXTURE_2D);
			//	}
			//	else {
			//		std::cout << "No data found" << "\n";
			//	}

			//	glBindTexture(GL_TEXTURE_2D, 0); 
			//	stbi_image_free(data);
			//}

			}


			//Index buffer
			int indexLocation = doc.meshes[i].primitives[j].indices;
			prim.indexBuffer = buffers[doc.accessors[indexLocation].bufferView];

			meshR->BindIndexBuffer(prim);
			prim.numIndices = doc.accessors[indexLocation].count;
			prim.indexType = (GLenum)doc.accessors[indexLocation].componentType;
			prim.indexOffset = doc.accessors[indexLocation].byteOffset;


			meshR->UnbindVertexArray();
			meshR->UnbindIndexBuffer();//Unbind index buffer after unbinding VAO to ensure index buffer is stored in VAO
			meshR->primitiveVector.push_back(prim);
		}

		this->meshVector.push_back(meshR);
		int banana = 0;
	}

	//Only set render model to collider if no collider mesh has been loaded
	if (this->parent->physN->colliderTriangles.empty()) {
		this->parent->physN->colliderTriangles = collisionTriangles;
		this->parent->physN->SetAABB();
	}
}



std::string GraphicsNode::GetAccessorString(int attribArrayIndex) {
	switch (attribArrayIndex) {
		case 0:	
			return "POSITION";
			break;
		case 1:
			return "TEXCOORD_0";
			break;
		case 2:
			return "NORMAL";
			break;
		case 3:
			return "TANGENT";
			break;
	}
	return "NONE";
}



void GraphicsNode::Tick(float deltaTime, Camera cam, bool paused) {
	this->DrawTris(cam.view, cam.projection, this->scaling);
}

void GraphicsNode::TickPhysics(float deltaTime, bool paused) {
	if (!paused) {
		this->position += (this->parent->physN->linearVelocity * deltaTime);
	}
}

glm::mat4 GraphicsNode::GetCompleteModelMatrix() {
	//Prepare model matrix
	glm::mat4 modelMatrix = glm::mat4(
		{ this->scaling,0,0,0 },
		{ 0,this->scaling,0,0 },
		{ 0,0,this->scaling,0 },
		{ this->position.x,this->position.y,this->position.z,1 }
	);
	//this->rotationMatrix = MathlibMat4::RotationX(this->radiansX) * MathlibMat4::RotationY(this->radiansY) * MathlibMat4::RotationZ(this->radiansZ);
	modelMatrix = modelMatrix * this->rotationMatrix;
	return modelMatrix;
}