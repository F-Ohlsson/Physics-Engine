#pragma once
#include "config.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"
#include <iostream>
#include "modelloadapp.h"

namespace Game {

ModelLoadApp::ModelLoadApp() {

}

ModelLoadApp::~ModelLoadApp() {

}

bool ModelLoadApp::Open() {
	App::Open();
	
	this->appWindow = new Display::Window();
	this->appWindow->SetSize(500, 500);
	this->appWindow->Open();
	if (this->appWindow->IsOpen()) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		return true;
	}
	return false;
}

void ModelLoadApp::Close() {
	App::Close();
}

void ModelLoadApp::Run() {
	glEnable(GL_DEPTH_TEST);
	Input::Keyboard* kbd = Input::GetDefaultKeyboard();

	Camera camera = Camera(500, 500, { 10.0f,5.0f,10.0f });
	camera.projection = camera.ProjectionMatrix(1.0f, 0.1f, 2000.0f);

	std::vector<GraphicsNode*> graphNodes;

	PhysicsObject* obj1 = new PhysicsObject;
	PhysicsNode* pNode1 = new PhysicsNode;
	GraphicsNode* gNode1 = new GraphicsNode;
	gNode1->parent = obj1;
	pNode1->parent = obj1;

	obj1->graphN = gNode1;
	obj1->physN = pNode1;

	gNode1->shadR = new ShaderResource;
	gNode1->LoadGLTF("../resources/gltf/cube.gltf");
	gNode1->position = { 1,1,1 };
	gNode1->scaling = 1.f;
	graphNodes.push_back(gNode1);

	//Setting up shaders

	std::string vertexShaderString;
	std::string fragmentShaderString;
	vertexShaderString = gNode1->shadR->LoadShaderFromFile("../resources/shaders/BlinnPhongBaseVertex.glsl"); //Base Blinn Phong Shaders
	fragmentShaderString = gNode1->shadR->LoadShaderFromFile("../resources/shaders/BlinnPhongBaseFragment.glsl");

	gNode1->shadR->vertexShader = gNode1->shadR->SetupShader(vertexShaderString.c_str(), GL_VERTEX_SHADER);
	gNode1->shadR->fragmentShader = gNode1->shadR->SetupShader(fragmentShaderString.c_str(), GL_FRAGMENT_SHADER);
	gNode1->shadR->shaderProgram = new GLuint(gNode1->shadR->CreateProgram(gNode1->shadR->vertexShader, gNode1->shadR->fragmentShader));

	if (gNode1->textR == nullptr) {
		gNode1->textR = new TextureResource(TextureResource());
	}
	gNode1->textR->LoadTexture("../resources/textures/OHNO.png", true);
	gNode1->textR->BindTexture(gNode1->textR->texture);



	PhysicsObject* obj2 = new PhysicsObject;
	PhysicsNode* pNode2 = new PhysicsNode;
	GraphicsNode* gNode2 = new GraphicsNode;
	gNode2->parent = obj2;
	pNode2->parent = obj2;
	obj2->graphN = gNode2;
	obj2->physN = pNode2;

	gNode2->shadR = gNode1->shadR;
	gNode2->LoadGLTF("../resources/gltf/cylinder.gltf");
	gNode2->position = { 2,5,-4 };
	gNode2->scaling = 1.f;
	graphNodes.push_back(gNode2);

	PhysicsObject* obj3 = new PhysicsObject;
	PhysicsNode* pNode3 = new PhysicsNode;
	GraphicsNode* gNode3 = new GraphicsNode;
	gNode3->parent = obj3;
	pNode3->parent = obj3;
	obj3->graphN = gNode3;
	obj3->physN = pNode3;

	gNode3->shadR = gNode1->shadR;
	gNode3->LoadGLTF("../resources/gltf/icosphere.gltf");
	gNode3->position = { -3,-2,5 };
	gNode3->scaling = 1.f;
	graphNodes.push_back(gNode3);


	BlinnPhongMaterial bpMaterial = BlinnPhongMaterial(gNode1->shadR->shaderProgram);
	bpMaterial.shininess = 10.0f;
	bpMaterial.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	bpMaterial.diffuse = { 1.0f,1.0f,1.0f,1.0f };
	bpMaterial.specular = { 1.0f,1.0f,1.0f,1.0f };
	bpMaterial.texture = new GLuint(gNode1->textR->texture);
	BlinnPhongMaterial* bpMaterialPointer = &bpMaterial;

	for (int i = 0; i < graphNodes.size(); i++) {
		//Assign Blinn Phong Material to all primitives of all meshes
		for (int j = 0; j < graphNodes[i]->meshVector.size(); j++) {
			MeshResource* mesh = graphNodes[i]->meshVector[j];
			for (int k = 0; k < mesh->primitiveVector.size(); k++) {
				mesh->primitiveVector[k].material = bpMaterialPointer;
			}
		}
	}


	DirLight dirLight = DirLight();
	dirLight.direction = { -0.5f,-1.0f,-2.3f };
	dirLight.colour = { 1.0f,1.0f,1.0f,1.0f };
	dirLight.intensity = 1.0f;


	camera.LookAt(glm::vec3(0, 0, 0));
	bool pan = false;
	float cameraCounter = 0;
	float cameraSpeed = 0.001f;

	while (this->appWindow->IsOpen()) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->appWindow->Update();

		if (kbd->pressed[Input::Key::Code::Space]) {
			pan = pan ? false : true;
		}

		if (pan) {
			cameraCounter += cameraSpeed;
		}

		camera.position = glm::vec3(glm::sin(cameraCounter)*10, 10, glm::cos(cameraCounter)*10);
		camera.view = inverse(camera.ViewMatrix({ 0,0,0 }));
		camera.LookAt(glm::vec3(0, 0, 0));

		for (int i = 0; i < graphNodes.size(); i++) {
			dirLight.ApplyLight(graphNodes[i]->shadR->shaderProgram);
			graphNodes[i]->DrawTris(camera.view, camera.projection, graphNodes[i]->scaling);
		}

		this->appWindow->SwapBuffers();

		if (kbd->pressed[Input::Key::Code::Escape]) {
				this->Exit();
		}
		//this->Exit();
	}
}

void ModelLoadApp::Exit() {
	App::Exit();
	this->appWindow->Close();
}
}