#pragma once
#include "config.h"
#include "core/physicscourse/PhysicsWorld.h"
#include "core/physicscourse/PhysicsNode.h"
#include "render/debugrender.h"
#include <vector>
#include <deque>
#include "gtc/random.hpp"

class PhysicsWorld;

struct collisionResults {
	glm::vec3 collisionNormal = glm::vec3(0);
	float collisionDepth = 0.f;
	glm::vec3 collisionPoint = glm::vec3(std::numeric_limits<float>::lowest());
	glm::vec3 collisionPointObjA = glm::vec3(std::numeric_limits<float>::lowest());
	glm::vec3 collisionPointObjB = glm::vec3(std::numeric_limits<float>::lowest());
	//bool finished = false;
};

struct edge {
	glm::vec3 point1;
	glm::vec3 point2;

	edge(glm::vec3 A, glm::vec3 B) {
		this->point1 = A;
		this->point2 = B;
	}
};


struct contactPoint {
	union {
		glm::vec3 vertices[3] = { glm::vec3(0),glm::vec3(0), glm::vec3(0) };

		struct {
			glm::vec3 minkowskyContact;
			glm::vec3 objAContact;
			glm::vec3 objBContact;
		};
	};
	contactPoint() {};
	contactPoint(glm::vec3 point0, glm::vec3 point1, glm::vec3 point2) {
		this->minkowskyContact = point0;
		this->objAContact = point1;
		this->objBContact = point2;
	}
	bool operator==(contactPoint rhs) {
		if (this->minkowskyContact != rhs.minkowskyContact)
			return false;
		if (this->objAContact != rhs.objAContact)
			return false;
		if (this->objBContact != rhs.objBContact)
			return false;

		return true;
	}
};

struct contactTetrahedron {
	contactPoint points[4] = { 
		contactPoint(glm::vec3(0),glm::vec3(0),glm::vec3(0)), 
		contactPoint(glm::vec3(0),glm::vec3(0),glm::vec3(0)), 
		contactPoint(glm::vec3(0),glm::vec3(0),glm::vec3(0)), 
		contactPoint(glm::vec3(0),glm::vec3(0),glm::vec3(0)) };
};

struct contactTriangle {
	contactPoint vertexPos[3];
	glm::vec3 normal = glm::vec3(0);
	contactTriangle() {};
	contactTriangle(contactPoint point0, contactPoint point1, contactPoint point2) {
		this->vertexPos[0] = point0;
		this->vertexPos[1] = point1;
		this->vertexPos[2] = point2;
		this->normal = glm::normalize(glm::cross(point1.minkowskyContact - point0.minkowskyContact, point2.minkowskyContact - point0.minkowskyContact));
	}
};

struct triangle;

class CollisionSystem {
public:
	CollisionSystem();
	CollisionSystem(const CollisionSystem& oldSystem);

	~CollisionSystem();
	PhysicsWorld* physWorld;

	//Helper function
	glm::vec3 BarycentricCoords(triangle faceTriangle, glm::vec3 point); //NOT PROPERLY TESTED, lifted from Picker, where it seems to work as intended
	bool SameHalfspace(glm::vec3 direction, glm::vec3 ao);
	glm::vec3 FindFurthestPoint(int physObjIndex, glm::vec3 direction);
	contactPoint FindSupportPoint(int objAIndex, int objBIndex, glm::vec3 direction, glm::mat4 modelMatrixA, glm::mat4 modelMatrixB);

	//Gilbert Johnson Keerthi
	bool GJK(int indexA, int indexB, contactTetrahedron& tetraSimplex);
	//GJK FUNCTIONS
	bool NextSimplex(std::deque<contactPoint>& simplexPoints, glm::vec3& direction);
	bool SimplexLine(std::deque<contactPoint>& simplexPoints, glm::vec3& direction);
	bool SimplexTriangle(std::deque<contactPoint>& simplexPoints, glm::vec3& direction);
	bool SimplexTetrahedron(std::deque<contactPoint>& simplexPoints, glm::vec3& direction);

	//Expanding Polytope Algorithm
	bool OldEPA(int indexA, int indexB, contactTetrahedron& tetraSimplex, collisionResults& results);
	//EPA FUNCTIONS RELATED TO OLDEPA
	bool GetFaceNormals(const std::vector<contactPoint>& polytope, std::vector<glm::vec3>& faces, uint& minTriangleIndex, std::vector<glm::vec3>& normals, std::vector<float>& distances);
	void AddIfUniqueEdge(std::vector<std::pair<uint, uint>>& edges, const std::vector<glm::vec3>& faces, uint pointA, uint pointB);
	void ExpandPolytope(std::vector<contactPoint>const& polytope, contactPoint const supportPoint, std::vector<glm::vec3>& faces, std::vector<glm::vec3>& normals, std::vector<float>& distances, std::vector<glm::vec3>& newFaces);

	//OWN IMPLEMENTATION BASED ON EPA IMPLEMENTATION IN https://github.com/Another-Ghost/3D-Collision-Detection-and-Resolution-Using-GJK-and-EPA
	bool NewEPA(int indexA, int indexB, contactTetrahedron& tetraSimplex, collisionResults& colResults);

	//REPONSE
	void PhysicsResponse(int indexA, int indexB, collisionResults& results, bool paused);

private:
};