#pragma once
#include "config.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"
#include <iostream>
#include "rayintersectapp.h"
#include "core/physicscourse/Plane.h"
#include <chrono>
#include "core/physicscourse/PhysicsObject.h"

namespace Game {

RayIntersectApp::RayIntersectApp() {
	cam = nullptr;
	this->picker = std::make_shared<Picker>(Picker());
}

RayIntersectApp::~RayIntersectApp() {
	
}

bool RayIntersectApp::Open() {
	App::Open();

	Core::CVarCreate(Core::CVar_Int, "r_draw_AABBs", "1");
	Core::CVarCreate(Core::CVar_Int, "r_draw_collider", "0");
	Core::CVarCreate(Core::CVar_Int, "r_draw_axes", "0");

	Core::CVarCreate(Core::CVar_Int, "r_damping", "0.f");
	
	this->appWindow = new Display::Window();
	this->appWindow->SetSize(500, 500);
	this->appWindow->Open();
	this->cam = std::make_shared<Camera>(Camera(500, 500, { 10.0f,5.0f,10.0f }));
	if (this->appWindow->IsOpen()) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		Debug::InitDebugRendering();
		this->appWindow->SetUiRender([this]() {
			this->RenderUI();
			});

		return true;
	}
	return false;
}

void RayIntersectApp::Close() {
	App::Close();
}

void RayIntersectApp::Run() {
	glEnable(GL_DEPTH_TEST);
	Input::Keyboard* kbd = Input::GetDefaultKeyboard();
	Input::Mouse* mouse = Input::GetDefaultMouse();

	this->cam->projection = this->cam->ProjectionMatrix(1.0f, 0.1f, 2000.0f);


	DirLight dirLight = DirLight();
	dirLight.direction = { 0.40f,-1.0f,0.2f };
	dirLight.colour = { 1.0f,1.0f,1.0f,1.0f };
	dirLight.intensity = 1.0f;


	std::vector<PhysicsObject*> gameObjects;

	//Create initial physics object
	PhysicsObject* gameObject1 = new PhysicsObject();

	gameObject1->graphN = new GraphicsNode();
	gameObject1->physN = new PhysicsNode();
	gameObject1->graphN->parent = gameObject1;
	gameObject1->physN->parent = gameObject1;

	gameObject1->graphN->position = { 2,2,-2 };
	
	gameObject1->graphN->scaling = 2.f;
	gameObject1->graphN->LoadGLTF("../resources/gltf/cube.gltf");
	
	gameObjects.push_back(gameObject1);

	//Setting up shaders
	std::string vertexShaderString;
	std::string fragmentShaderString;
	vertexShaderString = gameObject1->graphN->shadR->LoadShaderFromFile("../resources/shaders/BlinnPhongBaseVertex.glsl"); //Base Blinn Phong Shaders
	fragmentShaderString = gameObject1->graphN->shadR->LoadShaderFromFile("../resources/shaders/BlinnPhongBaseFragment.glsl");

	gameObject1->graphN->shadR->vertexShader = gameObject1->graphN->shadR->SetupShader(vertexShaderString.c_str(), GL_VERTEX_SHADER);
	gameObject1->graphN->shadR->fragmentShader = gameObject1->graphN->shadR->SetupShader(fragmentShaderString.c_str(), GL_FRAGMENT_SHADER);
	gameObject1->graphN->shadR->shaderProgram = new GLuint(gameObject1->graphN->shadR->CreateProgram(gameObject1->graphN->shadR->vertexShader, gameObject1->graphN->shadR->fragmentShader));
	if (gameObject1->graphN->textR == nullptr) {
		gameObject1->graphN->textR = new TextureResource(TextureResource());
	}
	gameObject1->graphN->textR->LoadTexture("../resources/textures/OHNO.png", true);
	gameObject1->graphN->textR->BindTexture(gameObject1->graphN->textR->texture);

	//Load texture to use as highlight
	TextureResource textR = TextureResource();
	textR.LoadTexture("../resources/textures/STRIPES.png", true);
	textR.BindTexture(textR.texture);

	//Set up additional physics objects
	PhysicsObject* gameObject2 = new PhysicsObject();
	gameObject2->graphN = new GraphicsNode();
	gameObject2->physN = new PhysicsNode();
	gameObject2->graphN->parent = gameObject2;
	gameObject2->physN->parent = gameObject2;
	gameObject2->graphN->shadR = gameObject1->graphN->shadR;
	gameObject2->graphN->position = { 2,-2,-1 };
	gameObject2->graphN->scaling = 1.f;
	gameObject2->graphN->LoadGLTF("../resources/gltf/icosphere.gltf");
	gameObjects.push_back(gameObject2);

	PhysicsObject*  gameObject3 = new PhysicsObject();
	gameObject3->graphN = new GraphicsNode();
	gameObject3->physN = new PhysicsNode();
	gameObject3->graphN->parent = gameObject3;
	gameObject3->physN->parent = gameObject3;

	gameObject3->graphN->shadR = gameObject1->graphN->shadR;
	gameObject3->graphN->position = { -2,-2,3 };
	gameObject3->graphN->scaling = 2.f;
	gameObject3->graphN->LoadGLTF("../resources/gltf/icosphere.gltf");
	gameObjects.push_back(gameObject3);

	//Create object material
	BlinnPhongMaterial bpMaterial = BlinnPhongMaterial(gameObject1->graphN->shadR->shaderProgram);
	bpMaterial.shininess = 10.0f;
	bpMaterial.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	bpMaterial.diffuse = { 1.0f,1.0f,1.0f,1.0f };
	bpMaterial.specular = { 1.0f,1.0f,1.0f,1.0f };
	BlinnPhongMaterial highlightMaterial = bpMaterial;
	bpMaterial.texture = new GLuint(gameObject1->graphN->textR->texture);
	highlightMaterial.texture = new GLuint(textR.texture);
	BlinnPhongMaterial* bpMaterialPointer = new BlinnPhongMaterial(bpMaterial);
	BlinnPhongMaterial* highlightPointer = new BlinnPhongMaterial(highlightMaterial);

		for (int i = 0; i < gameObjects.size(); i++) {
			GraphicsNode* currentGraphN = gameObjects[i]->graphN;
			//Assign Blinn Phong Material to all primitives of all meshes
			for (int j = 0; j < currentGraphN->meshVector.size(); j++) {
				MeshResource* mesh = currentGraphN->meshVector[j];
				for (int k = 0; k < mesh->primitiveVector.size(); k++) {
					mesh->primitiveVector[k].material = bpMaterialPointer;
				}
			}
		}

	picker->baseMaterial = bpMaterialPointer;
	picker->highlightMaterial = highlightPointer;
	
	float horCounter = 0;
	float verCounter = 0;
	glm::vec3 lookPoint = glm::vec3(0, 0, 0);

	double upperClamp = 1 / 60.0 * 1000000;
	double deltaTime = upperClamp;

	auto startTime = GetCurrentEpochTime();

	while (this->appWindow->IsOpen()) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		cam->position = { (sin(horCounter / 10) * 20), verCounter , (-cos(horCounter / 10) * 20)};
		this->cam->view = glm::inverse(this->cam->ViewMatrix(lookPoint));
		
		this->appWindow->Update();

		//Render all objects
		for (int i = 0; i < gameObjects.size(); i++) {
			gameObjects[i]->Tick(deltaTime, *this->cam, false);

			dirLight.ApplyLight(gameObjects[i]->graphN->shadR->shaderProgram);
			gameObjects[i]->Draw(deltaTime, *this->cam);
		}

		
		this->appWindow->SwapBuffers();

		if (kbd->pressed[Input::Key::Code::Escape]) {
			this->Exit();
		}

		Ray ray;
		if (mouse->held[Input::Mouse::Button::LeftButton]) {
			ray = picker->ShootRay(appWindow, cam.get());
			//See if any node intersects
			PhysicsObject* collidedObject = nullptr;
			float closestDistance = std::numeric_limits<float>::max();
			for (int i = 0; i < gameObjects.size(); i++) {
				float intersectionDistance = picker->IntersectsWithPhysicsObject(ray, gameObjects[i], cam->view);
				if (intersectionDistance >= 0 && intersectionDistance < closestDistance) {
					closestDistance = intersectionDistance;
					collidedObject = gameObjects[i];
				}
			}

			picker->RemoveHighlight();
			if (collidedObject != nullptr) {
					picker->AddHighlight(collidedObject->graphN);
			}
		}

		//Rotate camera according to arrow keys pressed
		float movementSpeed = 10 * deltaTime;
		if (kbd->held[Input::Key::Code::Up]) {
			verCounter += movementSpeed;
		}
		if (kbd->held[Input::Key::Code::Down]) {
			verCounter -= movementSpeed;
		}

		if (kbd->held[Input::Key::Code::Right]) {
			horCounter += movementSpeed;
		}
		if (kbd->held[Input::Key::Code::Left]) {
			horCounter -= movementSpeed;
		}


		//Deltatime
		//Clamp deltatime to prevent physics simulation to run haywire in case of low frame rate
		auto currentTime = GetCurrentEpochTime();
		deltaTime = (currentTime - startTime) * 0.000001;
		if (deltaTime > upperClamp) {
			deltaTime = upperClamp;
		}
		startTime = currentTime;
	}
}

void RayIntersectApp::Exit() {
	App::Exit();
	this->appWindow->Close();
}

void
RayIntersectApp::RenderUI()
{
	if (this->appWindow->IsOpen())
	{
		Debug::DispatchDebugDrawing(this->cam.get());
		Debug::DispatchDebugTextDrawing(this->cam.get());
	}
}

//Lifted from implementation done in multiplayer game for S0012E
unsigned long long RayIntersectApp::GetCurrentEpochTime() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto epoch_ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	return (unsigned long long)epoch_ms;
}

}