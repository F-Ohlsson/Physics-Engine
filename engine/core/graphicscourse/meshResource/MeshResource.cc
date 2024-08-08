#pragma once
#include "config.h"
#include "MeshResource.h"


MeshResource::MeshResource() {

}

MeshResource::~MeshResource() {
	//assert(false);
	for (int i = 0; i < primitiveVector.size(); i++) {
		glDeleteBuffers(1, &primitiveVector[i].indexBuffer);
		glDeleteBuffers(1, &primitiveVector[i].vertBuffer);
		glDeleteVertexArrays(1, &primitiveVector[i].vertexArray);
		UnbindIndexBuffer();
		UnbindVertexArray();
		BindBuffer(0);
	}

}

void MeshResource::BindBuffer(GLuint buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void MeshResource::BindIndexBuffer(primitive primitive) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.indexBuffer);
}

void MeshResource::UnbindIndexBuffer() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::BindVertexArray(primitive primitive) {
	glBindVertexArray(primitive.vertexArray);
}

void MeshResource::UnbindVertexArray() {
	glBindVertexArray(0);

}

//Values taken from learnopengl for testing purposes
void MeshResource::SetupTris() {
	GLfloat buf[]{
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	GLfloat colour[]{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	};

	GLfloat uv[]{
		1.0f, 1.0f, //top right
		1.0f, 0.0f, //bottom right
		0.0f, 0.0f, //bottom left
		0.0f, 1.0f, //top left
	};
	GLuint indices[]{
	0, 1, 3,   // first triangle
	1, 2, 3,    // second triangle
	};

	primitive newPrim;

	glGenVertexArrays(1, &newPrim.vertexArray);
	BindVertexArray(newPrim);

	//Load vertex positions
	glGenBuffers(1, &newPrim.vertBuffer);
	BindBuffer(newPrim.vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); //Position
	BindBuffer(0);

	////Load colour
	//GLuint test;
	//glGenBuffers(1, &test);
	//BindBuffer(test);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(colour), colour, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	//Load texture UVs
	glGenBuffers(1, &newPrim.textureBuffer);
	BindBuffer(newPrim.textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//Load index buffer
	glGenBuffers(1, &newPrim.indexBuffer);
	BindIndexBuffer(newPrim);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	UnbindIndexBuffer();
	newPrim.numIndices = 6;

	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	UnbindVertexArray();
	this->primitiveVector.push_back(newPrim);
}


void MeshResource::SetupQuad(float sizeX, float sizeY)
{
	
	GLfloat buf[] =
	{
		//face 0
		-(sizeX / 2),	-(sizeY / 2),	0,			// pos 0
		0,		0,		0,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	(sizeY / 2),	0,		// pos 1
		0,		0,		0,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	(sizeY / 2),	0,		// pos 2
		0,		0,		0,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	-(sizeY / 2),	0,		// pos 3
		0,		0,		0,		1,							// color 3
		0,		1,											//texCoord 3
	};
	primitive newPrim;

	glGenBuffers(1, &newPrim.vertBuffer);
	BindBuffer(newPrim.vertBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	BindBuffer(0);

	primitiveVector.push_back(newPrim);
}

void MeshResource::SetupFullScreenQuad(float sizeX, float sizeY) {

	GLfloat buf[] =
	{
		-(sizeX / 2),	-(sizeY / 2),	-1,			// pos 0
		0,0,
		-(sizeX / 2),	(sizeY / 2),	-1,			// pos 1
		0,1,
		(sizeX / 2),	(sizeY / 2),	-1,			// pos 2
		1,1,
		(sizeX / 2),	-(sizeY / 2),	-1,			// pos 3
		1,0,
	};
	primitive newPrim;

	glGenBuffers(1, &newPrim.vertBuffer);
	BindBuffer(newPrim.vertBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	BindBuffer(0);

	primitiveVector.push_back(newPrim);
}

void MeshResource::SetupCube(float sizeX, float sizeY, float sizeZ) 
{
	GLfloat buf[] =
	{
		//face 0 (back, black)
		-(sizeX / 2),	-(sizeY / 2),	-(sizeZ/2),			// pos 0
		0,		0,		0,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 1
		0,		0,		0,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 2
		0,		0,		0,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	-(sizeY / 2),	-(sizeZ / 2),		// pos 3
		0,		0,		0,		1,							// color 3
		0,		1,											//texCoord 3

		//face 1 (left wall, white)
		-(sizeX / 2),	-(sizeY / 2),	-(sizeZ / 2),		// pos 0
		1,		1,		1,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 1
		1,		1,		1,		1,							// color 1
		1,		0,											//texCoord 1
		-(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 2
		1,		1,		1,		1,							// color 2
		0,		0,											//texCoord 2
		-(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 3
		1,		1,		1,		1,							// color 3
		0,		1,											//texCoord 3

		//face 2 (top, red)
		-(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 0
		1,		0,		0,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 1
		1,		0,		0,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 2
		1,		0,		0,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 3
		1,		0,		0,		1,							// color 2
		0,		1,											//texCoord 3

		//face 3 (right wall, green)
		(sizeX / 2),	-(sizeY / 2),	-(sizeZ / 2),		// pos 0
		0,		1,		0,		1,							// color 0
		1,		1,											//texCoord 0
		(sizeX / 2),	(sizeY / 2),	-(sizeZ / 2),		// pos 1
		0,		1,		0,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 2
		0,		1,		0,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 3
		0,		1,		0,		1,							// color 3
		0,		1,											//texCoord 3

		//face 4 (bottom, blue)
		-(sizeX / 2),	-(sizeY / 2),	-(sizeZ / 2),		// pos 0
		0,		0,		1,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 1
		0,		0,		1,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 2
		0,		0,		1,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	-(sizeY / 2),	-(sizeZ / 2),		// pos 3
		0,		0,		1,		1,							// color 3
		0,		1,											//texCoord 3

		//face 5 (front, magenta)
		-(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 0
		1,		0,		1,		1,							// color 0
		1,		1,											//texCoord 0
		-(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 1
		1,		0,		1,		1,							// color 1
		1,		0,											//texCoord 1
		(sizeX / 2),	(sizeY / 2),	(sizeZ / 2),		// pos 2
		1,		0,		1,		1,							// color 2
		0,		0,											//texCoord 2
		(sizeX / 2),	-(sizeY / 2),	(sizeZ / 2),		// pos 3
		1,		0,		1,		1,							// color 3
		0,		1,											//texCoord 3
	};
	primitive newPrim;
	glGenBuffers(1, &newPrim.vertBuffer);
	BindBuffer(newPrim.vertBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	BindBuffer(0);
	primitiveVector.push_back(newPrim);
}

void MeshResource::DrawQuads() 
{
	for (int i = 0; i < primitiveVector.size(); i++) {
		primitive currentPrim = primitiveVector[i];
		BindBuffer(currentPrim.vertBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0); //Position
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 3)); //Colour
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * (3 + 4))); //Texture coordinates


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_QUADS, 0, 4 * 6); //Number of positions * numbe of faces
		BindBuffer(0);
	}

}

void MeshResource::DrawFullScreenQuad() {
	for (int i = 0; i < primitiveVector.size(); i++) {
		primitive currentPrim = primitiveVector[i];
		BindBuffer(currentPrim.vertBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0); //Position
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * (3))); //Texture coordinates


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_QUADS, 0, 4); //Number of positions * numbe of faces
		BindBuffer(0);
	}

}

void MeshResource::DrawTris(primitive prim) {
	BindVertexArray(prim);
	glDrawElements(GL_TRIANGLES, prim.numIndices, prim.indexType, (void*)prim.indexOffset);
	UnbindVertexArray();
}

void MeshResource::LoadOBJFile(std::string filepath) {
	std::string line;
	std::ifstream objFile;

	std::string spaceDelimiter = " ";
	std::string commentDelimiter = "#";
	std::string faceDelimiter = "/";

	std::vector<glm::vec3> positionVector;
	std::vector<glm::vec3> textureVector;
	std::vector<glm::vec3> normalVector;
	std::vector<stringVec3> faceVector;

	//Begin reading file
	objFile.open(filepath);
	if (objFile.is_open()) {
		//std::cout << "File open\n";

		while (!objFile.eof()) {
			getline(objFile, line);

			//Remove comment if it exists
			int commentPos = int(line.find(commentDelimiter));
			if (commentPos != std::string::npos)
				line.erase(commentPos, line.length());
			
			//Begin Vertex reading
			if ((line[0] == 'v') && (line[1] == ' ')) {
				line.erase(0, 1); //Remove "v" from start of line
				line.erase(0, line.find_first_not_of(' ')); //Remove eventual spaces before first value

				//Break out values into vec3 and insert into vector
				glm::vec3 vertexData = ProcessLineVec3(line, spaceDelimiter);
				positionVector.push_back(vertexData);

				//End Vertex reading
			}
			//Begin Texture reading
			else if ((line[0] == 'v') && (line[1] == 't')) {
				line.erase(0, 2); //Remove "vt" from start of line
				line.erase(0, line.find_first_not_of(' ')); //Remove eventual spaces before first value

				//Break out values into vec3 and insert into vector

				glm::vec3 uvData = ProcessLineVec3(line, spaceDelimiter);
				textureVector.push_back(uvData);

				//End Texture reading
			}
			//Begin Normal reading
			else if ((line[0] == 'v') && (line[1] == 'n')) {
				line.erase(0, 2); //Remove "vn" from start of line
				line.erase(0, line.find_first_not_of(' ')); //Remove eventual spaces before first value

				//Break out values into vec3 and insert into vector

				glm::vec3 normalData = ProcessLineVec3(line, spaceDelimiter);
				normalVector.push_back(normalData);

				//End Normal reading
			}
			//Begin Face reading
			else if ((line[0] == 'f') && line[1] == ' ') {
				line.erase(0, 1); //Remove "f" from start of line
				line.erase(0, line.find_first_not_of(' ')); //Remove eventual spaces before first value

				stringVec3 faceData = ProcessLineStringVec3(line, spaceDelimiter);
				faceVector.push_back(faceData);

				//End Face reading
			}
		}
		objFile.close();
		//std::cout << "File closed\n";
	}
	else
		std::cout << "Unable to open file\n";
	//End reading file



	//Start processing faces
	std::vector<stringVec3> faceIndices;

	for (int i = 0; i < faceVector.size(); i++) {
		stringVec3 faceLine;

		for (int j = 0; j < 3; j++) {
			std::string line = faceVector[i].v[j];
			
			faceLine = ProcessLineStringVec3(line, faceDelimiter);
			faceIndices.push_back(faceLine);
		}
	}
	//Finished processing faces

	//Time to actually make it parseable for OpenGL
	std::vector<vertex> vb;		//vertex buffer
	std::vector<GLuint> ib;		//index buffer
	std::unordered_map<std::string, vertexConfig> indexTable;

	for (int i = 0; i < faceIndices.size(); i++) {
		if (indexTable.find(faceIndices[i].originalString) != indexTable.end()) {
			//Fetch it and append its index to index buffer
			ib.push_back(indexTable.at(faceIndices[i].originalString).bufferIndex);
		}
		else {
			stringVec3 vectorPositions = ProcessLineStringVec3(faceIndices[i].originalString, faceDelimiter); //Indices for vertex pos, uvs & normals in respective vectors in that order
			vertexConfig vConfig; //Will ultimately be put into map

			//Setup Vertex obj for vertexBuffer
			vertex vert;
			vConfig.v = std::stoi(vectorPositions.v[0]);
			//Check if vt exists
			if (vectorPositions.v[1] != "") {
				glm::vec3 textCoords = textureVector.at(std::stoi(vectorPositions.v[1])-1);
				vConfig.vt = std::stoi(vectorPositions.v[1]);
				vert.uv_s = textCoords.x;
				vert.uv_t = textCoords.y;
			}
			//Check if vn exists
			if (vectorPositions.v[2] != "") {
				vConfig.vn = std::stoi(vectorPositions.v[2]);
				vert.normal = normalVector.at(std::stoi(vectorPositions.v[2])-1);
			}

			//Assume that vertex positions exists
			vert.pos = positionVector.at(std::stoi(vectorPositions.v[0])-1);

			//Add vertex to VB, IB and MAP
			vb.push_back(vert);
			vConfig.bufferIndex = int(vb.size()) - 1;
			ib.push_back(vConfig.bufferIndex);

			std::pair<std::string, vertexConfig> newEntry(faceIndices[i].originalString, vConfig);
			indexTable.insert(newEntry);
		}
	}
	
	primitive newPrim;
	newPrim.numIndices = int(ib.size());

	//Start binding to Vertex Array
	glGenVertexArrays(1, &newPrim.vertexArray);
	BindVertexArray(newPrim);

	//Load vertex data
	int stride = 3 + 2 + 3; //pos + uv + norm
	glGenBuffers(1, &newPrim.vertBuffer);
	BindBuffer(newPrim.vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(vertex), &vb.data()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0); //pos
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (GLvoid*)(sizeof(GLfloat) * 3)); //texture
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)* stride, (GLvoid*)(sizeof(GLfloat)* (3 + 2))); //normal
	BindBuffer(0);

	//Load index buffer
	glGenBuffers(1, &newPrim.indexBuffer);
	BindIndexBuffer(newPrim);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.size() * sizeof(int), &ib.data()[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//pos
	glEnableVertexAttribArray(1);	//uvs
	glEnableVertexAttribArray(2);	//normals
	UnbindVertexArray();
	UnbindIndexBuffer();

	primitiveVector.push_back(newPrim);
}

//Function that takes a string and splits it according to specified delimiter character, returns first three splits as a vec3
glm::vec3 MeshResource::ProcessLineVec3(std::string line, std::string delimiter) {

	glm::vec3 workVector = glm::vec3();
	for (int i = 0; i < 3; i++) {
		//Structure line to work in final buffer "x,y,z,"
		int spacePos = int(line.find(delimiter));

		//If no more spaces, read value until end of line
		if (spacePos == std::string::npos)
			spacePos = int(line.length());

		//Insert value into corresponding value
		std::string substring = line.substr(0, spacePos);
		if(spacePos != 0)
			workVector[i] = std::stof(substring);

		//Delete characters until next value
		line.erase(0, spacePos + 1);
	}
	return workVector;
}

//Does the same thing as it's vec3 bretheren but returns three strings instead of floats
stringVec3 MeshResource::ProcessLineStringVec3(std::string line, std::string delimiter) {
	stringVec3 workVector;
	workVector.originalString = line;
	for (int i = 0; i < 3; i++) {
		//Structure line to work in final buffer "x,y,z,"
		int spacePos = int(line.find(delimiter));

		//If no more spaces, read value until end of line
		if (spacePos == std::string::npos)
			spacePos = int(line.length());

		//Insert value into corresponding value
		std::string substring = line.substr(0, spacePos);
		workVector.v[i] = substring;

		//Delete characters until next value
		line.erase(0, spacePos + 1);
	}
	return workVector;
}

