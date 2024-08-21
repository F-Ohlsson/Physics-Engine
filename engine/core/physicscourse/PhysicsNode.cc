#pragma once
#include "config.h"
#include "PhysicsNode.h"
#include "render/debugrender.h"
#include "gtx/quaternion.hpp"

PhysicsNode::PhysicsNode() {
	this->colliderTriangles = std::vector<triangle>();
}

PhysicsNode::PhysicsNode(const PhysicsNode& pN) {
	this->parent = pN.parent;
	this->AABBscale = pN.AABBscale;
	this->AABBposition = pN.AABBposition;
	this->AABBcolor = pN.AABBcolor;
	this->AABBlineWeight = pN.AABBlineWeight;

	this->mass = pN.mass;
	this->invMass = pN.invMass;

	this->force = pN.force;
	this->linearVelocity = pN.linearVelocity;
	this->linearAcceleration = pN.linearAcceleration;

	this->torque = pN.torque;
	this->angularVelocity = pN.angularVelocity;
	this->angularAcceleration = pN.angularAcceleration;

	this->orientation = pN.orientation;
	this->inertiaTensor = pN.inertiaTensor;
	this->worldAllignedInvertedInertiaTensor = pN.worldAllignedInvertedInertiaTensor;
	this->tensorType = pN.tensorType;

	this->elasticity = pN.elasticity;


	for (int i = 0; i < pN.colliderTriangles.size(); i++) {
		this->colliderTriangles.push_back(pN.colliderTriangles[i]);
	}

	this->SetAABB();
}


PhysicsNode::~PhysicsNode() {
	//TODO DELETE STUFF
	this->parent = nullptr;
}

//Set inertia tensor based on formula for moments of inertia based on collider shape
void PhysicsNode::SetInertiaTensor(tensorTypes tensor) {
	this->tensorType = tensor;
	if (tensor == e_cuboid) {
		if (this->AABBscale == glm::vec3(0, 0, 0)) {
			this->SetAABB();
		}
		float extentX = this->AABBscale.x;
		float extentY = this->AABBscale.y;
		float extentZ = this->AABBscale.z;

		float mat00 = (this->mass * (glm::exp2(extentY) + glm::exp2(extentZ))) / 12;
		float mat11 = (this->mass * (glm::exp2(extentX) + glm::exp2(extentZ))) / 12;
		float mat22 = (this->mass * (glm::exp2(extentZ) + glm::exp2(extentY))) / 12;

		this->inertiaTensor[0] = glm::vec3(mat00, 0, 0);
		this->inertiaTensor[1] = glm::vec3(0, mat11, 0);
		this->inertiaTensor[2] = glm::vec3(0, 0, mat22);
	}
	else if (tensor == e_sphere) {
		if (this->AABBscale == glm::vec3(0, 0, 0)) {
			this->SetAABB();
		}
		float radius = this->AABBscale.x / 2;

		float entry = this->mass * (glm::exp2(radius)) * 2 / 5;

		this->inertiaTensor[0] = glm::vec3(entry, 0, 0);
		this->inertiaTensor[1] = glm::vec3(0, entry, 0);
		this->inertiaTensor[2] = glm::vec3(0, 0, entry);
	}
	else if (tensor == e_cylinder) {
		if (this->AABBscale == glm::vec3(0, 0, 0)) {
			this->SetAABB();
		}
		float height = this->AABBscale.y;
		float radius = this->AABBscale.x / 2;

		float XZentry = ((this->mass * glm::exp2(height)) /12) + ((this->mass * glm::exp2(radius)) / 4);
		float Yentry = ((this->mass * glm::exp2(radius)) / 2);

		this->inertiaTensor[0] = glm::vec3(XZentry, 0, 0);
		this->inertiaTensor[1] = glm::vec3(0, Yentry, 0);
		this->inertiaTensor[2] = glm::vec3(0, 0, XZentry);
	}
	else if (tensor == e_zero) {
		this->inertiaTensor = glm::mat4(0);
		this->worldAllignedInvertedInertiaTensor = glm::mat4(0);
	}

	glm::mat4  rotMatrix = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1)
	);
	if (this->parent->graphN->rotationMatrix != rotMatrix) {
		rotMatrix = this->parent->graphN->rotationMatrix;
	}
	CalcWorldAllignedInvertedInertiaTensor((rotMatrix));

}

//Create AABB based on collider size
void PhysicsNode::SetAABB() {
	glm::vec3 tempAABBmin = glm::vec3(std::numeric_limits<float>::max() + 1, std::numeric_limits<float>::max() + 1, std::numeric_limits<float>::max() + 1);
	glm::vec3 tempAABBmax = glm::vec3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
	for (int k = 0; k < this->colliderTriangles.size(); k++) {
		triangle currentTri = this->colliderTriangles[k];

		glm::mat4 modelMatrix = this->parent->graphN->GetCompleteModelMatrix();

		glm::vec4 tempPos0 = glm::vec4(currentTri.vertexPos[0].x, currentTri.vertexPos[0].y, currentTri.vertexPos[0].z, 1);
		glm::vec4 tempPos1 = glm::vec4(currentTri.vertexPos[1].x, currentTri.vertexPos[1].y, currentTri.vertexPos[1].z, 1);
		glm::vec4 tempPos2 = glm::vec4(currentTri.vertexPos[2].x, currentTri.vertexPos[2].y, currentTri.vertexPos[2].z, 1);

		tempPos0 = modelMatrix * tempPos0;
		tempPos1 = modelMatrix * tempPos1;
		tempPos2 = modelMatrix * tempPos2;

		triangle worldTri;
		worldTri.vertexPos[0] = glm::vec3(tempPos0.x, tempPos0.y, tempPos0.z);
		worldTri.vertexPos[1] = glm::vec3(tempPos1.x, tempPos1.y, tempPos1.z);
		worldTri.vertexPos[2] = glm::vec3(tempPos2.x, tempPos2.y, tempPos2.z);

		for (int l = 0; l < 3; l++) {
			tempAABBmin.x = std::min(worldTri.vertexPos[l].x, tempAABBmin.x);
			tempAABBmin.y = std::min(worldTri.vertexPos[l].y, tempAABBmin.y);
			tempAABBmin.z = std::min(worldTri.vertexPos[l].z, tempAABBmin.z);

			tempAABBmax.x = std::max(worldTri.vertexPos[l].x, tempAABBmax.x);
			tempAABBmax.y = std::max(worldTri.vertexPos[l].y, tempAABBmax.y);
			tempAABBmax.z = std::max(worldTri.vertexPos[l].z, tempAABBmax.z);
		}
	}

	this->AABBmin = tempAABBmin;
	this->AABBmax = tempAABBmax;

	this->AABBscale.x = abs(AABBmin.x - AABBmax.x);
	this->AABBscale.y = abs(AABBmin.y - AABBmax.y);
	this->AABBscale.z = abs(AABBmin.z - AABBmax.z);

	this->AABBposition.x = (AABBmax.x + AABBmin.x) / 2;
	this->AABBposition.y = (AABBmax.y + AABBmin.y) / 2;
	this->AABBposition.z = (AABBmax.z + AABBmin.z) / 2;

	SetAABBTriangles();
}

void PhysicsNode::SetMass(float newMass) {
	this->mass = newMass;
	
	this->invMass = newMass == 0 ? 0 : 1 / newMass;

	this->SetInertiaTensor(this->tensorType);
}

void PhysicsNode::SetAABBTriangles() {
	float padding = 0.0f;
	float xPositive = (this->AABBposition.x) + this->AABBscale.x / 2 + padding;
	float xNegative = (this->AABBposition.x) - this->AABBscale.x / 2 - padding;
	float yPositive = (this->AABBposition.y) + this->AABBscale.y / 2 + padding;
	float yNegative = (this->AABBposition.y) - this->AABBscale.y / 2 - padding;
	float zPositive = (this->AABBposition.z) + this->AABBscale.z / 2 + padding;
	float zNegative = (this->AABBposition.z) - this->AABBscale.z / 2 - padding;

	//Front face
	AABB[0].vertexPos[0] = glm::vec3(xNegative, yPositive, zPositive);
	AABB[0].vertexPos[1] = glm::vec3(xPositive, yPositive, zPositive);
	AABB[0].vertexPos[2] = glm::vec3(xPositive, yNegative, zPositive);

	AABB[1].vertexPos[0] = glm::vec3(xNegative, yPositive, zPositive);
	AABB[1].vertexPos[1] = glm::vec3(xNegative, yNegative, zPositive);
	AABB[1].vertexPos[2] = glm::vec3(xPositive, yNegative, zPositive);

	//Back face
	AABB[2].vertexPos[0] = glm::vec3(xNegative, yPositive, zNegative);
	AABB[2].vertexPos[1] = glm::vec3(xPositive, yPositive, zNegative);
	AABB[2].vertexPos[2] = glm::vec3(xPositive, yNegative, zNegative);

	AABB[3].vertexPos[0] = glm::vec3(xNegative, yPositive, zNegative);
	AABB[3].vertexPos[1] = glm::vec3(xNegative, yNegative, zNegative);
	AABB[3].vertexPos[2] = glm::vec3(xPositive, yNegative, zNegative);

	//Right face
	AABB[4].vertexPos[0] = glm::vec3(xNegative, yPositive, zNegative);
	AABB[4].vertexPos[1] = glm::vec3(xNegative, yNegative, zNegative);
	AABB[4].vertexPos[2] = glm::vec3(xNegative, yPositive, zPositive);

	AABB[5].vertexPos[0] = glm::vec3(xNegative, yPositive, zPositive);
	AABB[5].vertexPos[1] = glm::vec3(xNegative, yNegative, zPositive);
	AABB[5].vertexPos[2] = glm::vec3(xNegative, yNegative, zNegative);

	//Left face
	AABB[6].vertexPos[0] = glm::vec3(xPositive, yPositive, zNegative);
	AABB[6].vertexPos[1] = glm::vec3(xPositive, yNegative, zNegative);
	AABB[6].vertexPos[2] = glm::vec3(xPositive, yPositive, zPositive);

	AABB[7].vertexPos[0] = glm::vec3(xPositive, yPositive, zPositive);
	AABB[7].vertexPos[1] = glm::vec3(xPositive, yNegative, zPositive);
	AABB[7].vertexPos[2] = glm::vec3(xPositive, yNegative, zNegative);

	//Top face
	AABB[8].vertexPos[0] = glm::vec3(xNegative, yPositive, zPositive);
	AABB[8].vertexPos[1] = glm::vec3(xNegative, yPositive, zNegative);
	AABB[8].vertexPos[2] = glm::vec3(xPositive, yPositive, zNegative);

	AABB[9].vertexPos[0] = glm::vec3(xPositive, yPositive, zNegative);
	AABB[9].vertexPos[1] = glm::vec3(xPositive, yPositive, zPositive);
	AABB[9].vertexPos[2] = glm::vec3(xNegative, yPositive, zPositive);

	//Bottom face
	AABB[10].vertexPos[0] = glm::vec3(xNegative, yNegative, zPositive);
	AABB[10].vertexPos[1] = glm::vec3(xNegative, yNegative, zNegative);
	AABB[10].vertexPos[2] = glm::vec3(xPositive, yNegative, zNegative);

	AABB[11].vertexPos[0] = glm::vec3(xPositive, yNegative, zNegative);
	AABB[11].vertexPos[1] = glm::vec3(xPositive, yNegative, zPositive);
	AABB[11].vertexPos[2] = glm::vec3(xNegative, yNegative, zPositive);

	for (int i = 0; i < 12; i++) {
		AABB[i].normal = glm::cross(AABB[i].vertexPos[1] - AABB[i].vertexPos[0], AABB[i].vertexPos[2] - AABB[i].vertexPos[0]);
	}
}

void PhysicsNode::Tick(float deltaTime, bool paused) {
	if (!paused) {
		//Debug::DrawLine(this->AABBposition, this->AABBposition + this->linearVelocity, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop); //Linear velocity
		//Debug::DrawLine(this->AABBposition, this->AABBposition + this->angularVelocity, 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(0, 1, 1, 1), Debug::RenderMode::AlwaysOnTop); //Angular velocity

		this->linearAcceleration += this->force * invMass;
		this->linearVelocity += this->linearAcceleration * deltaTime;
		this->AABBposition += (this->linearVelocity);

		this->SetAABB(); //This should only be called if angular velocity != 0 but if I change this shit breaks and I don't know why

		bool angularPhysics = true;
		if(angularPhysics)
			this->Rotate(deltaTime);

		//Zero force and torque as well as linear and angular acceleration as their addition should only act as an impulse
		this->force = glm::vec3(0, 0, 0);
		this->torque = glm::vec3(0, 0, 0);

		this->linearAcceleration = glm::vec3(0);
		this->angularAcceleration = glm::vec3(0);

		this->Damping(deltaTime);
	}

	int drawAABB = Core::CVarReadInt(Core::CVarGet("r_draw_AABBs"));
	if (drawAABB) {
		Debug::DrawBox(this->AABBposition, glm::quat(1, 0, 0, 0), this->AABBscale.x, this->AABBscale.y, this->AABBscale.z, this->AABBcolor, Debug::RenderMode::WireFrame, this->AABBlineWeight);
	}

	int drawCollider = Core::CVarReadInt(Core::CVarGet("r_draw_collider"));
	if (drawCollider) {
		for (uint i = 0; i < this->colliderTriangles.size(); i++) {
			triangle currentTri = this->colliderTriangles[i];
			Debug::DrawLine(currentTri.vertexPos[0], currentTri.vertexPos[1], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(currentTri.vertexPos[0], currentTri.vertexPos[2], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(currentTri.vertexPos[1], currentTri.vertexPos[2], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
		}
	}
}

void PhysicsNode::Rotate(float deltaTime) {
	this->angularAcceleration += this->worldAllignedInvertedInertiaTensor * this->torque;
	this->angularVelocity += this->angularAcceleration * deltaTime;
	if (isnan(this->angularVelocity.x)) {
		this->angularVelocity = glm::vec3(0);
	}


	glm::quat newOri = glm::angleAxis(glm::radians(glm::length(this->angularVelocity)), glm::normalize(this->angularVelocity));

	if (!isnan(newOri.x)) {
		this->orientation = glm::normalize(newOri * this->orientation);
		glm::mat4 rotMatrix = glm::toMat4(this->orientation);
		CalcWorldAllignedInvertedInertiaTensor(rotMatrix);

		this->parent->graphN->rotationMatrix = rotMatrix;
		this->SetAABB();
	}


}
//------------------------------------------------------------------------------
/**
*/
static void TransformInertiaTensor(glm::mat4& iitWorld,
	const glm::mat4& iitBody,
	const glm::mat4& rotmat)
{
	float t4 = rotmat[0][0] * iitBody[0][0] +
		rotmat[0][1] * iitBody[1][0] +
		rotmat[0][2] * iitBody[2][0];
	float t9 = rotmat[0][0] * iitBody[0][1] +
		rotmat[0][1] * iitBody[1][1] +
		rotmat[0][2] * iitBody[2][1];
	float t14 = rotmat[0][0] * iitBody[0][2] +
		rotmat[0][1] * iitBody[1][2] +
		rotmat[0][2] * iitBody[2][2];
	float t28 = rotmat[1][0] * iitBody[0][0] +
		rotmat[1][1] * iitBody[1][0] +
		rotmat[1][2] * iitBody[2][0];
	float t33 = rotmat[1][0] * iitBody[0][1] +
		rotmat[1][1] * iitBody[1][1] +
		rotmat[1][2] * iitBody[2][1];
	float t38 = rotmat[1][0] * iitBody[0][2] +
		rotmat[1][1] * iitBody[1][2] +
		rotmat[1][2] * iitBody[2][2];
	float t52 = rotmat[2][0] * iitBody[0][0] +
		rotmat[2][1] * iitBody[1][0] +
		rotmat[2][2] * iitBody[2][0];
	float t57 = rotmat[2][0] * iitBody[0][1] +
		rotmat[2][1] * iitBody[1][1] +
		rotmat[2][2] * iitBody[2][1];
	float t62 = rotmat[2][0] * iitBody[0][2] +
		rotmat[2][1] * iitBody[1][2] +
		rotmat[2][2] * iitBody[2][2];

	iitWorld[0][0] = t4 * rotmat[0][0] +
		t9 * rotmat[0][1] +
		t14 * rotmat[0][2];
	iitWorld[0][1] = t4 * rotmat[1][0] +
		t9 * rotmat[1][1] +
		t14 * rotmat[1][2];
	iitWorld[0][2] = t4 * rotmat[2][0] +
		t9 * rotmat[2][1] +
		t14 * rotmat[2][2];
	iitWorld[1][0] = t28 * rotmat[0][0] +
		t33 * rotmat[0][1] +
		t38 * rotmat[0][2];
	iitWorld[1][1] = t28 * rotmat[1][0] +
		t33 * rotmat[1][1] +
		t38 * rotmat[1][2];
	iitWorld[1][2] = t28 * rotmat[2][0] +
		t33 * rotmat[2][1] +
		t38 * rotmat[2][2];
	iitWorld[2][0] = t52 * rotmat[0][0] +
		t57 * rotmat[0][1] +
		t62 * rotmat[0][2];
	iitWorld[2][1] = t52 * rotmat[1][0] +
		t57 * rotmat[1][1] +
		t62 * rotmat[1][2];
	iitWorld[2][2] = t52 * rotmat[2][0] +
		t57 * rotmat[2][1] +
		t62 * rotmat[2][2];
}


void PhysicsNode::CalcWorldAllignedInvertedInertiaTensor(glm::mat4 rotMat) {
	
	//Book solution
	glm::mat4 worldInvInertiaTensor = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));
	TransformInertiaTensor(worldInvInertiaTensor, this->inertiaTensor, rotMat);
	this->worldAllignedInvertedInertiaTensor = worldInvInertiaTensor;


	////Own solution (with help from ChatGPT)
	////Gives good results, very comparable to the solution from the book
	////However, I can't vouch for its complete accuracy which is why I'm leaving it commented out of the final results
	//glm::mat3 rot3 = glm::mat3(rotMat);

	//glm::mat3 transpRot3 = glm::transpose(rot3);
	//glm::mat3 orthogonalMatrix = rot3 * transpRot3; //Preserves only the orthogonal vectors
	//glm::mat3 inverseOrthogonalMatrix = glm::inverse(orthogonalMatrix);

	//glm::mat3 ownWorldInvInertiaTensor = inverseOrthogonalMatrix * this->inertiaTensor;
	//this->worldAllignedInvertedInertiaTensor = ownWorldInvInertiaTensor;


}


void PhysicsNode::LoadColliderGLTF(std::string filename) {
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
				for (int k = 0; k < doc.meshes[j].primitives.size(); k++) {
					if (doc.meshes[j].primitives[k].indices == i) {
						indexBuffer = true;
					}
				}
			}
			if (indexBuffer) {
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
		for (int j = 0; j < doc.meshes[i].primitives.size(); j++) { //For each primitive
			fx::gltf::Primitive currentPrim = doc.meshes[i].primitives[j];

			//ATTTRIBUTES
			fx::gltf::Attributes attributes = currentPrim.attributes;
			for (int k = 0; k < attributes.size(); k++) {
				std::string accessorString = this->parent->graphN->GetAccessorString(k);
				GLuint accessorLocation = attributes[accessorString]; //Find correct type of value based on current index
				GLuint bufferLocation = doc.accessors[accessorLocation].bufferView;

				GLuint target;
				if (targetLocations.size() == 0) {
					target = GLint(doc.bufferViews[doc.accessors[accessorLocation].bufferView].target);
				}
				else {
					target = targetLocations[bufferLocation];
				}


				//HEAVILY inspired by physics implementation by Fredrik Lindahl at LTU
				if (accessorString == "POSITION") {
					auto indexBufferAccessor = doc.accessors[currentPrim.indices];
					auto indexViewBuffer = doc.bufferViews[indexBufferAccessor.bufferView];
					auto indexBuffer = doc.buffers[indexViewBuffer.buffer];

					uint16_t const* indexBufferData = (uint16_t const*)&indexBuffer.data[indexBufferAccessor.byteOffset + doc.bufferViews[indexBufferAccessor.bufferView].byteOffset]; //Currently only supports this type (UnsignedShort)

					auto vertexBufferAccessor = doc.accessors[attributes[accessorString]];
					auto vertexViewBuffer = doc.bufferViews[vertexBufferAccessor.bufferView];
					auto vertexBuffer = doc.buffers[vertexViewBuffer.buffer];
					float const* vertexBufferData = (float const*)&vertexBuffer.data[vertexBufferAccessor.byteOffset + doc.bufferViews[vertexBufferAccessor.bufferView].byteOffset];


					size_t vectorSize = (doc.accessors[accessorLocation].type == fx::gltf::Accessor::Type::Vec3) ? 3 : 4;
					for (uint l = 0; l < indexBufferAccessor.count; l += 3) {
						triangle triangle;
						float test = vertexBufferData[vectorSize * indexBufferData[l]];
						triangle.vertexPos[0] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l]],
							vertexBufferData[vectorSize * indexBufferData[l] + 1],
							vertexBufferData[vectorSize * indexBufferData[l] + 2]
						);
						triangle.vertexPos[1] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l + 1]],
							vertexBufferData[vectorSize * indexBufferData[l + 1] + 1],
							vertexBufferData[vectorSize * indexBufferData[l + 1] + 2]
						);
						triangle.vertexPos[2] = glm::vec3(
							vertexBufferData[vectorSize * indexBufferData[l + 2]],
							vertexBufferData[vectorSize * indexBufferData[l + 2] + 1],
							vertexBufferData[vectorSize * indexBufferData[l + 2] + 2]
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
			}

		}
		this->colliderTriangles = collisionTriangles;
		this->SetAABB();
	}
}

void PhysicsNode::Damping(float deltaTime) {
	Core::CVar* r_damping = Core::CVarGet("r_damping");
	int enableDamping = Core::CVarReadInt(r_damping);
	if (enableDamping) {
		//Damping
		//Set to 0 if too small
		this->linearVelocity *= glm::length(this->linearVelocity) > 0.00005f ? 0.999 : 0;
		this->angularVelocity *= glm::length(this->angularVelocity) > 0.00005f ? 0.999 : 0;

	}

}