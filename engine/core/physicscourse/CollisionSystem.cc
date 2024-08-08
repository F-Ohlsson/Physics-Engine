#pragma once
#include "config.h"
#include "core/physicscourse/CollisionSystem.h"

CollisionSystem::CollisionSystem() {

}

CollisionSystem::CollisionSystem(const CollisionSystem& oldSystem) {
	this->physWorld = oldSystem.physWorld;
}

CollisionSystem::~CollisionSystem() {

}

//***************************************************************************************************
//---------------------------------------------------------------------------------------------------
//HELPER FUNCTIONS
//---------------------------------------------------------------------------------------------------
//***************************************************************************************************



// //NOT PROPERLY TESTED, lifted from Picker, where it seems to work as intended
//NEEDS DEBUGGING
glm::vec3 CollisionSystem::BarycentricCoords(triangle faceTriangle, glm::vec3 point) {
	glm::vec3 results;
	float u, v, w;
	glm::vec3 triA = faceTriangle.vertexPos[0];
	glm::vec3 triB = faceTriangle.vertexPos[1];
	glm::vec3 triC = faceTriangle.vertexPos[2];


	//Understand this!
	glm::vec3 AB = triB - triA;
	glm::vec3 AC = triC - triA;
	glm::vec3 AP = point - triA;
	float ABABdot = glm::dot(AB, AB);
	float ABACdot = glm::dot(AB, AC);
	float ACACdot = glm::dot(AC, AC);
	float APABdot = glm::dot(AP, AB);
	float APACdot = glm::dot(AP, AC);
	float denom = ABABdot * ACACdot - ABACdot * ABACdot;
	v = (ACACdot * APABdot - ABACdot * APACdot) / denom;
	w = (ABABdot * APACdot - ABACdot * APABdot) / denom;
	u = 1.0f - v - w;

	results[0] = u;
	results[1] = v;
	results[2] = w;

	return results;
}

//Returns whether two vectors are pointing in the same general direction
bool CollisionSystem::SameHalfspace(glm::vec3 direction1, glm::vec3 direction2) {
	return glm::dot(direction1, direction2) > 0;
}

glm::vec3 CollisionSystem::FindFurthestPoint(int physObjIndex, glm::vec3 direction) {
	glm::vec3 result = glm::vec3(0);
	float maxDistance = std::numeric_limits<float>::lowest();
	direction = glm::normalize(direction);
	PhysicsObject* obj = this->physWorld->physObjs[physObjIndex];

	for (int i = 0; i < obj->physN->colliderTriangles.size(); i++) {
		triangle currentTri = obj->physN->colliderTriangles[i];
		for (int j = 0; j < 3; j++) {
			glm::vec3 currentVector = currentTri.vertexPos[j];
			float distance = glm::dot(currentVector, direction);
			if (distance > maxDistance) {
				maxDistance = distance;
				result = currentTri.vertexPos[j];
			}
		}
	}

	return result;
}



contactPoint CollisionSystem::FindSupportPoint(int objAIndex, int objBIndex, glm::vec3 direction, glm::mat4 modelMatrixA, glm::mat4 modelMatrixB) {
	//World space direction (DIR)
	glm::vec4 dir4 = { direction ,0 };

	//Convert DIR to Model A space (inv A model matrix)
	//Calculate support point A 
	//Transform Support Point A to World space (A model matrix)
	glm::vec4 modelDirectionA = glm::inverse(modelMatrixA) * dir4;
	glm::vec3 pointSuppA = FindFurthestPoint(objAIndex, modelDirectionA);
	glm::vec4 worldPointA = { pointSuppA,1 };
	worldPointA = modelMatrixA * worldPointA;

	//Convert DIR to Model B space (inv B model matrix)
	//Calculate support point B
	//Transform Support Point B to World space (B model matrix)
	glm::vec4 modelDirectionB = glm::inverse(modelMatrixB) * dir4;
	glm::vec3 pointSuppB = FindFurthestPoint(objBIndex, modelDirectionB * -1.f);
	glm::vec4 worldPointB = { pointSuppB,1 };
	worldPointB = modelMatrixB * worldPointB;

	contactPoint results;
	results.vertices[0] = worldPointA - worldPointB; //Supportpoint on Minkowsky sum
	results.vertices[1] = worldPointA; //Supportpoint in world space on object A
	results.vertices[2] = worldPointB; //Supportpoint in world space on object B

	return results;
}



//***************************************************************************************************
//---------------------------------------------------------------------------------------------------
//GJK FUNCTIONS
//---------------------------------------------------------------------------------------------------
//***************************************************************************************************

bool CollisionSystem::GJK(int indexA, int indexB, contactTetrahedron& tetraSimplex) {

	std::deque<contactPoint> simplexContacts = std::deque<contactPoint>();

	//std::deque<glm::vec3> simplexPoints = std::deque<glm::vec3>();
	//std::deque<glm::vec3> simplexPointsObjA = std::deque<glm::vec3>();
	//std::deque<glm::vec3> simplexPointsObjB = std::deque<glm::vec3>();

	//float x = glm::linearRand(-1.f, 1.f);
	//float y = glm::linearRand(-1.f, 1.f);
	//float z = glm::linearRand(-1.f, 1.f);
	//
	float x = 1.f;
	float y = 0.f;
	float z = 0.f;

	//float x = 1.f;
	//float y = -1.f;
	//float z = -1.f;

	glm::vec3 direction = glm::normalize(glm::vec3(x, y, z));
	//glm::vec3 direction = this->physWorld->physObjs[indexA]->physN->AABBposition - this->physWorld->physObjs[indexB]->physN->AABBposition;
	//First point

	glm::mat4 modelMatrixA = physWorld->physObjs[indexA]->graphN->GetCompleteModelMatrix();
	glm::mat4 modelMatrixB = physWorld->physObjs[indexB]->graphN->GetCompleteModelMatrix();

	contactPoint supportPointResults = FindSupportPoint(indexA, indexB, direction, modelMatrixA, modelMatrixB);
	glm::vec3 supportPoint = supportPointResults.minkowskyContact;
	glm::vec3 supportPointObjA = supportPointResults.objAContact;
	glm::vec3 supportPointObjB = supportPointResults.objBContact;

	simplexContacts.push_front(supportPointResults);

	//simplexPoints.push_front(supportPoint);
	//simplexPointsObjA.push_front(supportPointObjA);
	//simplexPointsObjB.push_front(supportPointObjB);

	direction = glm::normalize(supportPoint * -1.f);
	//start iteration

	int iterator = 0;
	int maxIterations = 100;


	bool results = false;
	while (iterator < maxIterations) {
		supportPointResults = FindSupportPoint(indexA, indexB, direction, modelMatrixA, modelMatrixB);
		supportPoint = supportPointResults.minkowskyContact;
		supportPointObjA = supportPointResults.objAContact;
		supportPointObjB = supportPointResults.objBContact;

		if (glm::dot(supportPoint, direction) < 0)
			break;


		//Most recent point first (point A)
		simplexContacts.push_front(supportPointResults);

		//simplexPoints.push_front(supportPoint);
		//simplexPointsObjA.push_front(supportPointObjA);
		//simplexPointsObjB.push_front(supportPointObjB);

		if (NextSimplex(simplexContacts, direction)) {
			tetraSimplex.points[0] = simplexContacts[0];
			tetraSimplex.points[1] = simplexContacts[1];
			tetraSimplex.points[2] = simplexContacts[2];
			tetraSimplex.points[3] = simplexContacts[3];

			//tetraSimplex.vertexPos[0] = simplexPoints[0];
			//tetraSimplex.vertexPos[1] = simplexPoints[1];
			//tetraSimplex.vertexPos[2] = simplexPoints[2];
			//tetraSimplex.vertexPos[3] = simplexPoints[3];

			//tetraSimplexObjA.vertexPos[0] = simplexPointsObjA[0];
			//tetraSimplexObjA.vertexPos[1] = simplexPointsObjA[1];
			//tetraSimplexObjA.vertexPos[2] = simplexPointsObjA[2];
			//tetraSimplexObjA.vertexPos[3] = simplexPointsObjA[3];

			//tetraSimplexObjB.vertexPos[0] = simplexPointsObjB[0];
			//tetraSimplexObjB.vertexPos[1] = simplexPointsObjB[1];
			//tetraSimplexObjB.vertexPos[2] = simplexPointsObjB[2];
			//tetraSimplexObjB.vertexPos[3] = simplexPointsObjB[3];



			results = true;
			break;
		}

		iterator++;
	}


	if (results) {
		//this->physWorld->physObjs[indexA]->physN->AABBcolor = glm::vec4(1, 0, 1, 1);
		//this->physWorld->physObjs[indexB]->physN->AABBcolor = glm::vec4(1, 0, 1, 1);

		this->physWorld->physObjs[indexA]->physN->AABBcolor = glm::vec4(0, 0, 1, 1);
		this->physWorld->physObjs[indexB]->physN->AABBcolor = glm::vec4(1, 0, 1, 1);

		this->physWorld->physObjs[indexA]->physN->AABBlineWeight = 4;
		this->physWorld->physObjs[indexB]->physN->AABBlineWeight = 4;

		bool drawTetrahedron = false;
		if (drawTetrahedron) {
			Debug::DrawLine(tetraSimplex.points[0].minkowskyContact, tetraSimplex.points[1].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[0].minkowskyContact, tetraSimplex.points[2].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[0].minkowskyContact, tetraSimplex.points[3].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);

			Debug::DrawLine(tetraSimplex.points[3].minkowskyContact, tetraSimplex.points[1].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[2].minkowskyContact, tetraSimplex.points[1].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[2].minkowskyContact, tetraSimplex.points[3].minkowskyContact, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);


			Debug::DrawLine(tetraSimplex.points[0].minkowskyContact, glm::vec3(0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[1].minkowskyContact, glm::vec3(0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[2].minkowskyContact, glm::vec3(0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(tetraSimplex.points[3].minkowskyContact, glm::vec3(0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);


			Debug::DrawDebugText("A", tetraSimplex.points[0].minkowskyContact, glm::vec4(1, 0, 0, 1));
			Debug::DrawDebugText("B", tetraSimplex.points[1].minkowskyContact, glm::vec4(1, 0, 0, 1));
			Debug::DrawDebugText("C", tetraSimplex.points[2].minkowskyContact, glm::vec4(1, 0, 0, 1));
			Debug::DrawDebugText("D", tetraSimplex.points[3].minkowskyContact, glm::vec4(1, 0, 0, 1));
			Debug::DrawDebugText("0", glm::vec3(0), glm::vec4(1, 1, 1, 1));
		}

		//Debug::DrawDebugText("HIT", glm::vec3(0), glm::vec4(1, 0, 0, 1));
		return true;
	}
	else if (!results) {
		this->physWorld->physObjs[indexA]->physN->AABBcolor = glm::vec4(1, 1, 1, 1);
		this->physWorld->physObjs[indexB]->physN->AABBcolor = glm::vec4(1, 1, 1, 1);

		this->physWorld->physObjs[indexA]->physN->AABBlineWeight = 1;
		this->physWorld->physObjs[indexB]->physN->AABBlineWeight = 1;
	}

	return false;
}





//Selects correct way of finding next point based on what current simplex is
bool CollisionSystem::NextSimplex(std::deque<contactPoint>& simplexPoints, glm::vec3& direction) {
	switch (simplexPoints.size()) {
	case 2: return SimplexLine(simplexPoints, direction);
	case 3: return SimplexTriangle(simplexPoints, direction);
	case 4: return SimplexTetrahedron(simplexPoints, direction);
	}

	return false;
}


//Find new direction to create triangle
bool CollisionSystem::SimplexLine(std::deque<contactPoint>& simplexPoints, glm::vec3& direction) {
	//I have two points
	contactPoint pointA = simplexPoints[0];
	contactPoint pointB = simplexPoints[1];


	glm::vec3 AB = pointB.minkowskyContact - pointA.minkowskyContact;
	glm::vec3 AO = pointA.minkowskyContact * -1.f;


	//Perpendicular to plane AB & AO is on
	glm::vec3 ABxAO = glm::cross(AB, AO);

	//Perpendicular to AB & ABxAO, gives normal pointing in general direction of O
	glm::vec3 tripleProduct = glm::cross(ABxAO, AB);

	//If vector from A (second added point) and B (first added point) is point in the general direction that AO does, we proceed
	if (SameHalfspace(AB, AO)) {
		direction = tripleProduct;

	}
	else { //Remove first point since it can't be valid
		simplexPoints = { pointA };
		direction = AO;
	}


	return false;
}

bool CollisionSystem::SimplexTriangle(std::deque<contactPoint>& simplexPoints, glm::vec3& direction) {
	//I have three points
	contactPoint pointA = simplexPoints[0];
	contactPoint pointB = simplexPoints[1];
	contactPoint pointC = simplexPoints[2];

	//Debug::DrawLine(pointA, pointB, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointA, pointC, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointC, pointB, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);


	glm::vec3 AB = pointB.minkowskyContact - pointA.minkowskyContact;
	glm::vec3 AC = pointC.minkowskyContact - pointA.minkowskyContact;
	glm::vec3 AO = pointA.minkowskyContact * -1.f;


	//Perpendicular to plane AB & AO is on
	glm::vec3 ABxAC = glm::cross(AB, AC);
	//normal pointing away from the simplex in the perpendicular to AB
	glm::vec3 ABLineNormal = glm::cross(AB, ABxAC);
	glm::vec3 ACLineNormal = glm::cross(ABxAC, AC);

	//FROM CASEY VIDEO

	//If AC line normal is pointing toward origin
	if (SameHalfspace(ACLineNormal, AO)) {
		//If AC and AO points in same direction, we know that origin exists outside AC line within points A and C, meaning that point B can be excluded
		if (SameHalfspace(AC, AO)) {
			//simplexPoints.clear();
			simplexPoints = { pointA, pointC };
			direction = glm::cross(glm::cross(AC, AO), AC);
			return SimplexLine(simplexPoints, direction);
		}
		//Otherwise origin exists outside AC line but outside area set up within points A and C.
		else {
			//STAR
			//Since the area outside BC was excluded in Line step we can conclude point C as invalid
			if (SameHalfspace(AB, AO)) {
				simplexPoints = { pointA, pointB };
				direction = glm::cross(glm::cross(AB, AO), AB);
				return SimplexLine(simplexPoints, direction);
			}
			//If neither AB or AC is pointing toward origin, scrap them both since neither are valid
			//This should basically never happen, as long as the new point passed origin
			else {
				simplexPoints = { pointA };
				direction = AO;
			}
		}
	}
	else {
		//If AB line normal (perpendicular to line pointing OUT from triangle) alligns with AO, origin lies outside AB plane
		if (SameHalfspace(ABLineNormal, AO)) {
			//STAR
			//Since the area outside BC was excluded in Line step we can conclude point C as invalid
			if (SameHalfspace(AB, AO)) {
				simplexPoints = { pointA, pointB };
				direction = glm::cross(glm::cross(AB, AO), AB);
				return SimplexLine(simplexPoints, direction);
			}
			//If neither AB or AC is pointing toward origin, scrap them both since neither are valid
			//This should basically never happen, as long as the new point passed origin
			else {
				simplexPoints = { pointA };
				direction = AO;
			}
		}
		//If neither AB or AC line normals co allign with AO the point is somewhere inside the 3d planes set up by the triangle lines. Move on to finding a fourth point
		else {
			//If the vector perpendicular to ABC triangle coalligns with AO, we use that as the search direction
			if (SameHalfspace(ABxAC, AO)) {
				//simplexPoints = { pointA, pointB, pointC };
				direction = ABxAC;
			}
			//Otherwise we use the vector perpendicular to ABC but in opposite direction (ACxAB really)
			//Since we're relying on a clockwise vertex winding we need to update the order of the vertices to take into consideration that the direction is now mirrored
			else {
				simplexPoints = { pointA, pointC, pointB };
				direction = ABxAC * -1.f;
			}
		}
	}

	return false;
}

bool CollisionSystem::SimplexTetrahedron(std::deque<contactPoint>& simplexPoints, glm::vec3& direction) {
	//I have four points
	contactPoint pointA = simplexPoints[0];
	contactPoint pointB = simplexPoints[1];
	contactPoint pointC = simplexPoints[2];
	contactPoint pointD = simplexPoints[3];

	//Debug::DrawLine(pointA, pointB, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointA, pointC, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointA, pointD, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	// 
	//Debug::DrawLine(pointB, pointC, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointB, pointD, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
	//Debug::DrawLine(pointC, pointD, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);

	//Debug::DrawDebugText("A", pointA, glm::vec4(1, 0, 0, 1));
	//Debug::DrawDebugText("B", pointB, glm::vec4(1, 0, 0, 1));
	//Debug::DrawDebugText("C", pointC, glm::vec4(1, 0, 0, 1));
	//Debug::DrawDebugText("D", pointD, glm::vec4(1, 0, 0, 1));


	glm::vec3 AB = pointB.minkowskyContact - pointA.minkowskyContact;
	glm::vec3 AC = pointC.minkowskyContact - pointA.minkowskyContact;
	glm::vec3 AD = pointD.minkowskyContact - pointA.minkowskyContact;

	glm::vec3 AO = pointA.minkowskyContact * -1.f;


	//Need to check validity of 3 triangles
	//ABC
	//ADB (not ABD because of clockwise vertex winding)
	//ACD
	//BCD needs no check since it was deemed fine by the SimplexTriangle function

	//Compute normals pointing out from tetrahedron
	glm::vec3 ABxAC = glm::cross(AB, AC); //ABC normal
	glm::vec3 ACxAD = glm::cross(AC, AD); //ACD normal
	glm::vec3 ADxAB = glm::cross(AD, AB); //ADB normal

	//int bDEBUG = Core::CVarReadInt(Core::CVarGet("r_DEBUG"));
	//if (bDEBUG) {
	//	std::cout << glm::dot(ABxAC, AO) << std::endl;
	//	std::cout << glm::dot(ACxAD, AO) << std::endl;
	//	std::cout << glm::dot(ADxAB, AO) << std::endl;
	//}


	//Start with ABC
	if (SameHalfspace(ABxAC, AO)) {

		//If AO is located outside plane set up by ABC
		//D is invalid

		simplexPoints = { pointA, pointB , pointC };
		direction = glm::cross(glm::cross(ABxAC, AO), ABxAC);

		//If we realized that the tetrahedron was invalid, we remove the excluded point and do a validity check for the new triangle
		//This will in turn check the voronoi regions for the triangle and set a new direction if needed
		//If triangle is deemed invalid, it will then send to line case and validity will be checked there
		return SimplexTriangle(simplexPoints, direction);
	}
	//Next ACD
	else if (SameHalfspace(ACxAD, AO)) {
		//If AO is located outside plane set up by ACD
		//B is invalid

		simplexPoints = { pointA, pointC , pointD };
		direction = glm::cross(glm::cross(ACxAD, AO), ACxAD);


		return SimplexTriangle(simplexPoints, direction);

	}
	//Next ADB
	else if (SameHalfspace(ADxAB, AO)) {
		//If AO is located outside plane set up by ADB
		//C is invalid

		simplexPoints = { pointA, pointD , pointB };
		direction = glm::cross(glm::cross(ADxAB, AO), ADxAB);


		return SimplexTriangle(simplexPoints, direction);

	}

	//If the origin isn't outside any of the planes set up by the triangles in our tetrahedron it exists inside it, meaning we have a collision
	return true;
}


//***************************************************************************************************
//---------------------------------------------------------------------------------------------------
//EPA FUNCTIONS
//---------------------------------------------------------------------------------------------------
//***************************************************************************************************


//HEAVILY from https://winter.dev/articles/epa-algorithm
bool CollisionSystem::OldEPA(int indexA, int indexB, contactTetrahedron& tetraSimplex, collisionResults& results) {

	//Tetrahedron face indices (counter clockwise winding)
	glm::vec3 ACBindices = { 0,2,1 };
	glm::vec3 ABDindices = { 0,1,3 };
	glm::vec3 ADCindices = { 0,3,2 };
	glm::vec3 BCDindices = { 1,2,3 };

	std::vector<glm::vec3> faces = { ACBindices,ABDindices,ADCindices,BCDindices };

	std::vector<contactPoint> polytope = { tetraSimplex.points[0] ,tetraSimplex.points[1], tetraSimplex.points[2], tetraSimplex.points[3]};

	uint minTriangleIndex = -1;
	std::vector<glm::vec3> normals;
	std::vector<float> distances;
	//Get face normals (writes to minTriangleIndex, normals & distances)
	GetFaceNormals(polytope, faces, minTriangleIndex, normals, distances);


	glm::mat4 modelMatrixA = this->physWorld->physObjs[indexA]->graphN->GetCompleteModelMatrix();
	glm::mat4 modelMatrixB = this->physWorld->physObjs[indexB]->graphN->GetCompleteModelMatrix();

	glm::vec3 minNormal = normals[minTriangleIndex];
	float minDistance = std::numeric_limits<float>::max();

	while (minDistance == std::numeric_limits<float>::max()) {


		minNormal = normals[minTriangleIndex];
		minDistance = distances[minTriangleIndex];


		contactPoint supportPoint = FindSupportPoint(indexA, indexB, minNormal, modelMatrixA, modelMatrixB);


		glm::vec3 closestFaceToSupPoint = supportPoint.minkowskyContact - polytope[faces[minTriangleIndex].x].minkowskyContact;
		float projOnSearchDirection = glm::dot(minNormal, closestFaceToSupPoint);


		//If distance to new point from previous closest face in the search direction is so small that it essentially is the same point, we have found the furthest point in that direction
		//This if will then return false, breaking the while
		float tolerance = 0.001f;
		if (projOnSearchDirection > tolerance) {
			minDistance = std::numeric_limits<float>::max(); //Reset minDistance to ensure outer while loop runs one more time

			//Expand polytope in direction of min normal
			std::vector<glm::vec3> newFaces;
			ExpandPolytope(polytope, supportPoint, faces, normals, distances, newFaces);


			//Safe to add supportPoint to polytope after we've removed all faces in the same halfspace and set up new vertex coordinates for new faces
			polytope.push_back(supportPoint);

			//Calculate new normals, distances and closest triangle index for the new faces
			std::vector<glm::vec3> newNormals;
			std::vector<float> newDistances;
			uint newMinTriIndex;
			GetFaceNormals(polytope, newFaces, newMinTriIndex, newNormals, newDistances); //This fills newNormals, newDistances and newMinTriIndex



			//Find closest face among the old faces
			//Since we've removed the previously closest face from the polytope we must do this check here as well
			//Set closest face to be closest face of the old faces
			float oldMinDistance = std::numeric_limits<float>::max();
			for (uint i = 0; i < distances.size(); i++) {
				if (distances[i] < oldMinDistance) {
					oldMinDistance = distances[i];
					minTriangleIndex = i;
				}
			}

			if (newDistances.size() == 0) {
				std::cout << "NO DISTANCE" << std::endl;
				break;
			}

			//Check whether closest face of the new faces is closer than the closest face of the old faces
			//If yes, set new closest face index to the closest new face, otherwise 
			if (newDistances[newMinTriIndex] < oldMinDistance) {
				minTriangleIndex = newMinTriIndex + distances.size();
			}

			//Insert new distances, normals and faces at the end of their respective vectors
			faces.insert(faces.end(), newFaces.begin(), newFaces.end());
			distances.insert(distances.end(), newDistances.begin(), newDistances.end());
			normals.insert(normals.end(), newNormals.begin(), newNormals.end());

		}

	}


	//If while has been broken we have found the polytope with the face closest to the origin 
	//Which includes the collision information
	glm::vec3 minFaceIndices = faces[minTriangleIndex];

	triangle closestFace = triangle(
		polytope[minFaceIndices[0]].minkowskyContact,
		polytope[minFaceIndices[1]].minkowskyContact,
		polytope[minFaceIndices[2]].minkowskyContact);

	triangle closestFaceObjA = triangle(
		polytope[minFaceIndices[0]].objAContact,
		polytope[minFaceIndices[1]].objAContact,
		polytope[minFaceIndices[2]].objAContact);

	triangle closestFaceObjB = triangle(
		polytope[minFaceIndices[0]].objBContact,
		polytope[minFaceIndices[1]].objBContact,
		polytope[minFaceIndices[2]].objBContact);


	//Project origin onto closest polytope face
	//SOMETIMES PROJECTED POINT IS NOT INSIDE CLOSEST FACE
	//TODO: Investigate this
	//glm::vec3 OriginToFace = closestFace.vertexPos[0]; //Vector between origin and closest face
	//float distance = glm::dot(OriginToFace, glm::normalize(minNormal)); //Projection of O->Face vector on collision normal
	//glm::vec3 projPoint = OriginToFace - distance * minNormal; //Move projection distance in the direction of the collision normal to end up on the closest face


	//THIS DOES THE SAME THING BUT USING RAYS
	//WORKS BETTER IT SEEMS
	Plane closestPlane = Plane(closestFace.vertexPos[0], closestFace.vertexPos[1], closestFace.vertexPos[2]);
	Ray testRay = Ray(glm::vec3(0), glm::vec3(0)+minNormal);
	glm::vec3 projPoint = testRay.PlaneIntersectionPoint(closestPlane);


	//Calculate barycentric coordinates of projected origin point
	glm::vec3 baryCoord = BarycentricCoords(closestFace, projPoint);


	//P = uA+vB+wC
	glm::vec3 collisionPointObjA = closestFaceObjA.vertexPos[0] * baryCoord[0] + closestFaceObjA.vertexPos[1] * baryCoord[1] + closestFaceObjA.vertexPos[2] * baryCoord[2];
	glm::vec3 collisionPointObjB = closestFaceObjB.vertexPos[0] * baryCoord[0] + closestFaceObjB.vertexPos[1] * baryCoord[1] + closestFaceObjB.vertexPos[2] * baryCoord[2];


	//Collision results
	results.collisionNormal = minNormal;
	results.collisionDepth = minDistance; //
	results.collisionPoint = projPoint; //
	results.collisionPointObjA = collisionPointObjA; //In world space
	results.collisionPointObjB = collisionPointObjB; //In world space

	Core::CVar* r_draw_EPA_results = Core::CVarGet("r_draw_EPA_results");
	int drawEPA = Core::CVarReadInt(r_draw_EPA_results);
	if (drawEPA) {
		Debug::DrawDebugText("P", projPoint, glm::vec4(0, 0, 0, 1));

		//Polytope Min Face
		Debug::DrawLine(closestFace.vertexPos[0], closestFace.vertexPos[1], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFace.vertexPos[1], closestFace.vertexPos[2], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFace.vertexPos[2], closestFace.vertexPos[0], 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);

		//A min face
		Debug::DrawLine(closestFaceObjA.vertexPos[0], closestFaceObjA.vertexPos[1], 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFaceObjA.vertexPos[1], closestFaceObjA.vertexPos[2], 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFaceObjA.vertexPos[2], closestFaceObjA.vertexPos[0], 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawDebugText("A", collisionPointObjA, glm::vec4(0, 0, 1, 1));

		//B min face
		Debug::DrawLine(closestFaceObjB.vertexPos[0], closestFaceObjB.vertexPos[1], 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFaceObjB.vertexPos[1], closestFaceObjB.vertexPos[2], 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawLine(closestFaceObjB.vertexPos[2], closestFaceObjB.vertexPos[0], 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
		Debug::DrawDebugText("B", collisionPointObjB, glm::vec4(1, 0, 1, 1));
	
		//Collision normal
		Debug::DrawLine(glm::vec3(0), glm::vec3(0) + minNormal, 10.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop); //Draw collision normal
		Debug::DrawLine(glm::vec3(0), glm::vec3(0) + minNormal * (minDistance), 5.0f, glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), Debug::RenderMode::AlwaysOnTop); //Draw collision depth on normal

	}


	return true;
}


//Calculates the normals of all faces in a given polytope
bool CollisionSystem::GetFaceNormals(const std::vector<contactPoint>& polytope, std::vector<glm::vec3>& faces, uint& minTriangleIndex, std::vector<glm::vec3>& normals, std::vector<float>& distances) {
	std::vector<glm::vec3> workingNormals;
	std::vector<float> workingDistances;
	float workingMinDistance = std::numeric_limits<float>::max();
	uint workingMinTriangleIndex = 0;

	for (uint i = 0; i < faces.size(); i++) {
		glm::vec3 currentFace = faces[i];

		//NOT SAME AS ABC IN TETRAHEDRON
		glm::vec3 facePointA = polytope[(int)currentFace[0]].minkowskyContact;
		glm::vec3 facePointB = polytope[(int)currentFace[1]].minkowskyContact;
		glm::vec3 facePointC = polytope[(int)currentFace[2]].minkowskyContact;

		glm::vec3 AB = facePointB - facePointA;
		glm::vec3 AC = facePointC - facePointA;

		glm::vec3 cross = glm::cross(AB, AC);

		glm::vec3 faceNormal = glm::normalize(cross);

		//check if normal is pointing out from face or not
		//Since origin is inside volume we know that a normal pointing away from the volume will allign with a vector between origin and any point on the triangle, OA, OB or OC
		double distance = glm::dot(faceNormal, facePointA);

		//If pointing into polytope volume, reverse distance and normal
		if (distance < 0.) {
			faceNormal *= -1;
			distance *= -1;
			//int tempIndexB = currentFace[1];
			//int tempIndexC = currentFace[2];
			//faces[i][1] = tempIndexC;
			//faces[i][2] = tempIndexB;
		}

		workingNormals.push_back(faceNormal);
		workingDistances.push_back(distance);

		if (distance < workingMinDistance) {
			workingMinTriangleIndex = i;
			workingMinDistance = distance;
		}
	}

	minTriangleIndex = workingMinTriangleIndex;
	normals = workingNormals;
	distances = workingDistances;
	return true;
}

void CollisionSystem::AddIfUniqueEdge(std::vector<std::pair<uint, uint>>& uniqueEdges, const std::vector<glm::vec3>& faces, uint pointA, uint pointB) {

	std::pair<uint, uint> newPair = std::make_pair(pointA, pointB);
	std::pair<uint, uint> reversePair = std::make_pair(pointB, pointA);


	for (uint i = 0; i < uniqueEdges.size(); i++) {
		if (uniqueEdges[i] == reversePair) {
			//std::cout << "REVERSE FOUND";
			uniqueEdges.erase(uniqueEdges.begin() + i);
			return;
		}
		else if (uniqueEdges[i] == newPair) {
			std::cout << "DUPLICATE, SAME DIRECTION" << std::endl;
		}
	}

	uniqueEdges.push_back(newPair);

	//std::cout << pointA << std::endl;
	//std::cout << pointB << std::endl;
}


//Expands polytope in direction of supportpoint
// Removes all polytope faces visible by supportpoint
// Adds new faces to newFaces vector
void CollisionSystem::ExpandPolytope(std::vector<contactPoint>const& polytope, contactPoint const supportPoint, std::vector<glm::vec3>& faces, std::vector<glm::vec3>& normals, std::vector<float>& distances, std::vector<glm::vec3>& newFaces) {
	std::vector<std::pair<uint, uint>> uniqueEdges;
	for (int i = normals.size() - 1; i >= 0; i--) {

		//We must remove all faces visible by the support point in order to prevent stray points to remain inside the polytope
		glm::vec3 faceToSupport = supportPoint.minkowskyContact - polytope[faces[i][0]].minkowskyContact;

		if (SameHalfspace(normals[i], faceToSupport)) {

			AddIfUniqueEdge(uniqueEdges, faces, faces[i][0], faces[i][1]);
			AddIfUniqueEdge(uniqueEdges, faces, faces[i][1], faces[i][2]);
			AddIfUniqueEdge(uniqueEdges, faces, faces[i][2], faces[i][0]);

			faces.erase(faces.begin() + i);
			normals.erase(normals.begin() + i);
			distances.erase(distances.begin() + i);

		}
	}

	//Add new faces to polytope
	for (uint i = 0; i < uniqueEdges.size(); i++) {
		uint vertex1 = uniqueEdges[i].first;
		uint vertex2 = uniqueEdges[i].second;
		uint vertex3 = polytope.size();
		//^^^ Support point is going to have the index that is current highest +1. Which is also the size of the polytope (5th entry will have index 4 etc.) so adding size() as index works as long as you do it before adding supportPoint to the polytope

		glm::vec3 newFace = { vertex1,vertex2,vertex3 };
		newFaces.push_back(newFace);
	}
}

//***************************************************************************************************
//---------------------------------------------------------------------------------------------------
//New EPA
//---------------------------------------------------------------------------------------------------
//***************************************************************************************************

//HEAVILY BASED ON EPA IMPLMENTATION IN
//https://github.com/Another-Ghost/3D-Collision-Detection-and-Resolution-Using-GJK-and-EPA
bool CollisionSystem::NewEPA(int indexA, int indexB, contactTetrahedron& tetraSimplex, collisionResults& results) {

	glm::mat4 modelMatrixA = this->physWorld->physObjs[indexA]->graphN->GetCompleteModelMatrix();
	glm::mat4 modelMatrixB = this->physWorld->physObjs[indexB]->graphN->GetCompleteModelMatrix();

	std::vector<contactTriangle> faces;

	//Set up initial faces
	contactTriangle ABC = contactTriangle{ tetraSimplex.points[0],tetraSimplex.points[1], tetraSimplex.points[2] };
	contactTriangle ACD = contactTriangle{ tetraSimplex.points[0],tetraSimplex.points[2], tetraSimplex.points[3] };
	contactTriangle ADB = contactTriangle{ tetraSimplex.points[0],tetraSimplex.points[3], tetraSimplex.points[1] };
	contactTriangle BDC = contactTriangle{ tetraSimplex.points[1],tetraSimplex.points[3], tetraSimplex.points[2] };

	faces = { ABC,ACD,ADB,BDC };

	uint closestFaceIndex = INT_MAX;

	float algorithmTolerance = 0.001;
	uint maxIterations = 100;
	bool algorithmConverged = false;
	float shortestDistance = FLT_MAX;
	for (uint iteration = 0; iteration < maxIterations; iteration++) {
		//Find face closest to origin
		shortestDistance = FLT_MAX;
		for (int i = 0; i < faces.size(); i++) {
			float faceDistance = glm::dot(faces[i].vertexPos[0].minkowskyContact, faces[i].normal);
			if (faceDistance < shortestDistance) {
				shortestDistance = faceDistance;
				closestFaceIndex = i;
			}
		}

		if (closestFaceIndex == INT_MAX)
			break;

		//Expand polytope in direction of closest face normal
		glm::vec3 searchDir = faces[closestFaceIndex].normal;

		contactPoint supportPoint = FindSupportPoint(indexA, indexB, searchDir, modelMatrixA, modelMatrixB);

		//Check if distance between face and furthest point in normal direction is small enough to conclude algorithm
		if ((glm::dot(supportPoint.minkowskyContact, searchDir) - shortestDistance) < algorithmTolerance){
			//CONCLUSION REACHED
			algorithmConverged = true;
			break;
		}

		//If not concluded
		//Remove all faces visible from supportpoint
		std::vector<std::tuple<contactPoint, contactPoint>> looseEdges;
		for (int i = 0; i < faces.size(); i++) {
			glm::vec3 faceToSupport = supportPoint.minkowskyContact - faces[i].vertexPos[0].minkowskyContact;
			if (SameHalfspace(faces[i].normal, faceToSupport)) { //If vector between facenormal and supportpoint share halfspace the face is visible from the supportpoint
				for (int j = 0; j < 3; j++) {
					std::tuple<contactPoint, contactPoint> currentEdge = { faces[i].vertexPos[j], faces[i].vertexPos[(j + 1) % 3]  }; //Clever way of making sure first edge is 0->1, then 1->2, then 2->0. NOT MY IDEA, I'm not clever enough for that
					bool duplicateEdge = false;
					for (int k = 0; k < looseEdges.size(); k++) {
						contactPoint currentEdgeNodeA = std::get<0>(currentEdge);
						contactPoint currentEdgeNodeB = std::get<1>(currentEdge);

						contactPoint looseEdgeNodeA = std::get<0>(looseEdges[k]);
						contactPoint looseEdgeNodeB = std::get<1>(looseEdges[k]);


						//Remove any loose edge that is the reverse of the one currently being checked.
						//Check reverse first since it's a much more likely result
						if (currentEdgeNodeA == looseEdgeNodeB && currentEdgeNodeB == looseEdgeNodeA) {
							looseEdges.erase(looseEdges.begin() + k);
							duplicateEdge = true;
							break; //End loop since edges should only be able to occur once
						}
						//Also check same direction to be on the safe side, this shouldn't occur
						else if (currentEdgeNodeA == looseEdgeNodeA && currentEdgeNodeB == looseEdgeNodeB) {
							//Duplicate found, remove from list since both triangles connected to that edge should be removed
							looseEdges.erase(looseEdges.begin() + k);
							duplicateEdge = true;
							break; //End loop since edges should only be able to occur once
						}
					}

					if (!duplicateEdge) {
						//Edge should be added to loose edges
						looseEdges.push_back(currentEdge);
					}
				}

				//Remove triangle from list of faces
				faces.erase(faces.begin() + i);
				i--;
			}
		}

		//reconstruct polytope with supportPoint
		for (int i = 0; i < looseEdges.size(); i++) {
			contactPoint edgeA = std::get<0>(looseEdges[i]);
			contactPoint edgeB = std::get<1>(looseEdges[i]);
			contactTriangle newFace = contactTriangle(edgeA, edgeB, supportPoint);

			//Check for incorrect winding
			float bias = 0.0001f;
			//float bias = 0.f;
			if (glm::dot(newFace.vertexPos[0].minkowskyContact, newFace.normal) + bias < 0) {
				contactPoint tempPoint = newFace.vertexPos[0];
				newFace.vertexPos[0] = newFace.vertexPos[1];
				newFace.vertexPos[1] = tempPoint;
				newFace.normal *= -1;
			}
			faces.push_back(newFace);
		}

	}
	if (algorithmConverged) {
		triangle closestFace = triangle(faces[closestFaceIndex].vertexPos[0].minkowskyContact, faces[closestFaceIndex].vertexPos[1].minkowskyContact, faces[closestFaceIndex].vertexPos[2].minkowskyContact);
		Plane closestPlane = Plane(closestFace.vertexPos[0], closestFace.vertexPos[1], closestFace.vertexPos[2]);
		Ray normalRay = Ray(glm::vec3(0), glm::vec3(0) + closestFace.normal);

		//Project intersectionpoint onto plane set up by closest face
		glm::vec3 projPoint = normalRay.PlaneIntersectionPoint(closestPlane);
		//Calculate barycentric coordinates of projection point
		glm::vec3 baryCoord = BarycentricCoords(closestFace, projPoint);

		//P = uA+vB+wC
		//Use barycentric coordinates to find collision point on Object A and B
		glm::vec3 collisionPointObjA = faces[closestFaceIndex].vertexPos[0].objAContact * baryCoord[0] + faces[closestFaceIndex].vertexPos[1].objAContact * baryCoord[1] + faces[closestFaceIndex].vertexPos[2].objAContact * baryCoord[2];
		glm::vec3 collisionPointObjB = faces[closestFaceIndex].vertexPos[0].objBContact * baryCoord[0] + faces[closestFaceIndex].vertexPos[1].objBContact * baryCoord[1] + faces[closestFaceIndex].vertexPos[2].objBContact * baryCoord[2];

		//Collision results
		results.collisionNormal = closestFace.normal;
		results.collisionDepth = shortestDistance; //
		results.collisionPoint = projPoint; //
		results.collisionPointObjA = glm::vec4(collisionPointObjA, 1); //In world space
		results.collisionPointObjB = glm::vec4(collisionPointObjB, 1); //In world space

		int drawEPA = Core::CVarReadInt(Core::CVarGet("r_draw_EPA_results"));
		if (drawEPA) {
			Debug::DrawDebugText("P", projPoint, glm::vec4(0, 0, 0, 1));

			//Polytope Min Face
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[0].minkowskyContact, faces[closestFaceIndex].vertexPos[1].minkowskyContact, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[1].minkowskyContact, faces[closestFaceIndex].vertexPos[2].minkowskyContact, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[2].minkowskyContact, faces[closestFaceIndex].vertexPos[0].minkowskyContact, 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);

			//A min face
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[0].objAContact, faces[closestFaceIndex].vertexPos[1].objAContact, 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[1].objAContact, faces[closestFaceIndex].vertexPos[2].objAContact, 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[2].objAContact, faces[closestFaceIndex].vertexPos[0].objAContact, 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawDebugText("A", results.collisionPointObjA, glm::vec4(0, 0, 1, 1));

			////B min face
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[0].objBContact, faces[closestFaceIndex].vertexPos[1].objBContact, 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[1].objBContact, faces[closestFaceIndex].vertexPos[2].objBContact, 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			Debug::DrawLine(faces[closestFaceIndex].vertexPos[2].objBContact, faces[closestFaceIndex].vertexPos[0].objBContact, 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
			//Debug::DrawDebugText("B", glm::inverse(modelMatrixB) * glm::vec4(localB + objBpos, 1), glm::vec4(1, 0, 1, 1));
			Debug::DrawDebugText("B", results.collisionPointObjB, glm::vec4(1, 0, 1, 1));

			//Collision normal
			Debug::DrawLine(glm::vec3(0), glm::vec3(0) + results.collisionNormal, 10.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop); //Draw collision normal
			Debug::DrawLine(glm::vec3(0), glm::vec3(0) + results.collisionNormal * (results.collisionDepth), 5.0f, glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), Debug::RenderMode::AlwaysOnTop); //Draw collision depth on normal

		}
		return true;
	}


	return false;
}


//***************************************************************************************************
//---------------------------------------------------------------------------------------------------
//Response
//---------------------------------------------------------------------------------------------------
//***************************************************************************************************
void CollisionSystem::PhysicsResponse(int indexA, int indexB, collisionResults& colResults, bool paused) {
	PhysicsNode* objA = this->physWorld->physObjs[indexA]->physN;
	PhysicsNode* objB = this->physWorld->physObjs[indexB]->physN;

	glm::vec3 radiusVectorA = colResults.collisionPointObjA - objA->AABBposition; //World space
	glm::vec3 radiusVectorB = colResults.collisionPointObjB - objB->AABBposition; //World space

	//Object velocities at collision point
	glm::vec3 angularVelocityAtColPointA = glm::cross(objA->angularVelocity, radiusVectorA);
	glm::vec3 angularVelocityAtColPointB = glm::cross(objB->angularVelocity, radiusVectorB);

	glm::vec3 totalVelocityA = objA->linearVelocity + angularVelocityAtColPointA;
	glm::vec3 totalVelocityB = objB->linearVelocity + angularVelocityAtColPointB;

	//Value representing the difference in velocity projected onto the collision normal
	float normalVelocity = glm::dot(colResults.collisionNormal, (totalVelocityB - totalVelocityA));

	//effective mass
	float effMassA = (objA->invMass + (glm::dot((objA->worldAllignedInvertedInertiaTensor) * (glm::cross(glm::cross(radiusVectorA, colResults.collisionNormal), radiusVectorA)), colResults.collisionNormal)));

	float effMassB = (objB->invMass + (glm::dot((objB->worldAllignedInvertedInertiaTensor) * (glm::cross(glm::cross(radiusVectorB, colResults.collisionNormal), radiusVectorB)), colResults.collisionNormal)));

	//Impulse strength and direction
	float impulse = (-(1.0f + (objA->elasticity * objB->elasticity)) * normalVelocity) / (effMassA + effMassB);
	if (impulse < 0) impulse = 0;

	glm::vec3 impulseVector = colResults.collisionNormal * impulse;

	glm::vec3 offsetPosA = colResults.collisionDepth * -colResults.collisionNormal * (objA->invMass / (objA->invMass + objB->invMass));
	glm::vec3 offsetPosB = colResults.collisionDepth * colResults.collisionNormal * (objB->invMass / (objA->invMass + objB->invMass));

	//Linear velocity addition
	glm::vec3 objALinVelAdd = -impulseVector * objA->invMass;
	glm::vec3 objBLinVelAdd = impulseVector * objB->invMass;

	//Angular velocity addition
	glm::vec3 objAAngVelAdd = objA->worldAllignedInvertedInertiaTensor * glm::cross(radiusVectorA, -impulseVector);
	glm::vec3 objBAngVelAdd = objB->worldAllignedInvertedInertiaTensor * glm::cross(radiusVectorB, impulseVector);

	//Put the thing together
	if (!paused) {
		//If object is movable, offsett position by collisionDepth * collisionNormal equivalent to the invMass compared to total mass
		if (objA->mass != 0) {
			glm::vec3 offsetPos = offsetPosA;
			if (isnan(offsetPos.x))
				offsetPos = glm::vec3(0);

			this->physWorld->physObjs[indexA]->graphN->position += offsetPos;

			//Add linear and angular velocity
			objA->linearVelocity += objALinVelAdd;
			objA->angularVelocity += objAAngVelAdd;
		}

		if (objB->mass != 0) {
			glm::vec3 offsetPos = offsetPosB;
			if (isnan(offsetPos.x))
				offsetPos = glm::vec3(0);

			this->physWorld->physObjs[indexB]->graphN->position += offsetPos;

			objB->linearVelocity += objBLinVelAdd;
			objB->angularVelocity += objBAngVelAdd;
		}
	}

}