#pragma once
#include "config.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"
#include <iostream>
#include "unconstraineddynamicsapp.h"
#include "core/physicscourse/Plane.h"
#include <chrono>
#include "core/cvar.h"


namespace Game {

UnconstrainedDynamicsApp::UnconstrainedDynamicsApp() {
	cam = nullptr;
	this->picker = new Picker();
	this->physWorld = new PhysicsWorld();
}

UnconstrainedDynamicsApp::~UnconstrainedDynamicsApp() {

}

bool UnconstrainedDynamicsApp::Open() {
	App::Open();

	Core::CVarCreate(Core::CVar_Float, "r_force_power", "100.f");
	Core::CVarCreate(Core::CVar_Float, "r_object_mass", "5.f");




	this->appWindow = new Display::Window();
	this->appWindow->SetSize(1000, 1000);
	this->appWindow->Open();
	this->cam = new Camera(1000, 1000, { 10.0f,5.0f,10.0f });
	this->physWorld->Initialize();
	if (this->appWindow->IsOpen()) {
		//glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		//glClearColor(0.f, 0.f, 0.f, 1.f);
		Debug::InitDebugRendering();
		this->appWindow->SetUiRender([this]() {
			this->RenderUI();
			});

		return true;
	}
	return false;
}

void UnconstrainedDynamicsApp::Close() {
	App::Close();
}

void UnconstrainedDynamicsApp::Run() {
	glEnable(GL_DEPTH_TEST);
	Input::Keyboard* kbd = Input::GetDefaultKeyboard();
	Input::Mouse* mouse = Input::GetDefaultMouse();

	this->cam->projection = this->cam->ProjectionMatrix(1.0f, 0.1f, 2000.0f);

	//PointLight pointLight = PointLight();
	//pointLight.pos = { 1,2,1 };
	//pointLight.intensity = 10;

	DirLight dirLight = DirLight();
	dirLight.direction = { 0.0f,-1.1f,-2.3f };
	dirLight.colour = { 1.0f,1.0f,1.0f,1.0f };
	dirLight.intensity = 1.0f;

	Core::CVarWriteInt(Core::CVarGet("r_runEPA"), 0);
	PhysicsObject* gameObject1 = new PhysicsObject();


	gameObject1->graphN = new GraphicsNode();
	gameObject1->physN = new PhysicsNode();
	gameObject1->graphN->parent = gameObject1;
	gameObject1->physN->parent = gameObject1;

	//gameObject1->graphN->shadR = new ShaderResource();
	gameObject1->graphN->position = { 0,1,0 };


	gameObject1->graphN->scaling = 1.f;
	gameObject1->graphN->LoadGLTF("../resources/gltf/cube.gltf");
	gameObject1->physN->LoadColliderGLTF("../resources/gltf/cube.gltf");

	//gameObject1->graphN->scaling = 20.f;
	//gameObject1->graphN->LoadGLTF("../resources/gltf/Avocado/Avocado.gltf");

	//gameObject1->graphN->scaling = 4.f;
	//gameObject1->graphN->LoadGLTF("../resources/gltf/FlightHelmet/FlightHelmet.gltf");
	//gameObject1->physN->LoadColliderGLTF("../resources/gltf/icosphere.gltf");


	gameObject1->physN->SetMass(Core::CVarReadFloat(Core::CVarGet("r_object_mass")));
	gameObject1->physN->SetInertiaTensor(e_cuboid);

	physWorld->physObjs.push_back(gameObject1);

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
	gameObject1->graphN->textR->LoadTexture("../resources/textures/BIGLEAVES.png", true);
	//gameObject1->graphN->textR->LoadTexture("../resources/textures/STRIPES.png", true);
	gameObject1->graphN->textR->BindTexture(gameObject1->graphN->textR->texture);

	TextureResource textR = TextureResource();
	textR.LoadTexture("../resources/textures/WATER.png", true);
	//textR.LoadTexture("../resources/textures/STRIPES.png", true);
	textR.BindTexture(textR.texture);

	PhysicsObject* gameObject2 = new PhysicsObject();
	gameObject2->graphN = new GraphicsNode();
	gameObject2->physN = new PhysicsNode();
	gameObject2->graphN->parent = gameObject2;
	gameObject2->physN->parent = gameObject2;

	gameObject2->graphN->shadR = gameObject1->graphN->shadR;
	gameObject2->graphN->position = { 2,-5,-5 };
	gameObject2->graphN->scaling = 1.f;
	gameObject2->physN->SetMass(Core::CVarReadFloat(Core::CVarGet("r_object_mass")));
	gameObject2->graphN->LoadGLTF("../resources/gltf/icosphere.gltf");
	gameObject2->physN->SetInertiaTensor(e_sphere);
	physWorld->physObjs.push_back(gameObject2);


	//std::shared_ptr<GameObject> gameObject3 = std::make_shared<GameObject>(GameObject());
	//gameObject3->graphN = std::make_shared<GraphicsNode>(GraphicsNode());
	//gameObject3->physN = std::make_shared<PhysicsNode>(PhysicsNode());
	//gameObject3->graphN->parent = gameObject3;
	//gameObject3->physN->parent = gameObject3;

	//gameObject3->graphN->shadR = gameObject1->graphN->shadR;
	//gameObject3->graphN->position = { -2,-2,3 };
	//gameObject3->graphN->scaling = 2.f;
	//gameObject3->graphN->LoadGLTF("../resources/gltf/icosphere.gltf");
	//gameObjects.push_back(gameObject3);

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

	for (int i = 0; i < physWorld->physObjs.size(); i++) {
		GraphicsNode* currentGraphN = physWorld->physObjs[i]->graphN;
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


	/*
	float horCounter = 0;
	float verCounter = 0;*/
	glm::vec3 lookPoint = glm::vec3(0, 0, 0);


	//Time stuff
	double upperClamp = 1 / 60.0;
	double deltaTime = upperClamp;
	auto startTime = GetCurrentEpochTime();

	//Force stuff
	glm::vec3 intersection = glm::vec3(0,0,0);
	float intersectionBoxScale = 0.5;
	float invIntersectionBoxScale = 1/0.5;


	////this works
	//glm::vec3* testVector = new glm::vec3(1);
	//glm::vec3* deepCopy = new glm::vec3(*testVector);
	//deepCopy->x = 2;

	////This also works;
	//std::shared_ptr<glm::vec3> testVec = std::make_shared<glm::vec3>(glm::vec3(1));
	//std::shared_ptr<glm::vec3> testVecDeep = std::make_shared<glm::vec3>(*testVec);
	//testVecDeep->x = 2;



	uint worldCounter = 0;

	while (this->appWindow->IsOpen()) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//cam->position = { (sin(horCounter / 10) * 5), verCounter , (-cos(horCounter / 10) * 5) };
		this->cam->view = glm::inverse(this->cam->ViewMatrix(lookPoint));

		this->appWindow->Update();

		int bPause = Core::CVarReadInt(Core::CVarGet("r_pause"));
		int worldFrame = Core::CVarReadInt(Core::CVarGet("r_world_frame"));
		if (!bPause) {

			if (worldFrame == worldStates.size()) {
				for (int i = 0; i < physWorld->physObjs.size(); i++) {
					physWorld->physObjs[i]->physN->SetMass(Core::CVarReadFloat(Core::CVarGet("r_object_mass")));
				}
				physWorld->Tick((float)deltaTime, *this->cam);


				for (int i = 0; i < physWorld->physObjs.size(); i++) {
					dirLight.ApplyLight(physWorld->physObjs[i]->graphN->shadR->shaderProgram);
				}

				//Debug::DrawDebugText("HELLO", glm::vec3(0, 0, 0), glm::vec4(1, 0, 0, 1));
				//Debug::DrawLine(glm::vec3(1,1,1), glm::vec3(0, 0, 0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
				//Debug::DrawBox(glm::vec3(-1, -1, -1), glm::quat(1,0,0,0), 1.f, glm::vec4(1, 0, 0, 1), Debug::RenderMode::WireFrame, 1.f);



				Ray ray;
				if (mouse->pressed[Input::Mouse::Button::LeftButton]) {
					ray = picker->ShootRay(appWindow, cam);
					//See if any node intersects
					PhysicsObject* collidedObject = nullptr;
					float intersectionDistance;
					float closestDistance = std::numeric_limits<float>::max();
					for (int i = 0; i < physWorld->physObjs.size(); i++) {
						intersectionDistance = picker->IntersectsWithPhysicsObject(ray, physWorld->physObjs[i], cam->view);
						if (intersectionDistance >= 0 && intersectionDistance < closestDistance) {
							closestDistance = intersectionDistance;
							collidedObject = physWorld->physObjs[i];
						}
					}

					picker->RemoveHighlight();
					if (collidedObject != nullptr) {
						picker->AddHighlight(collidedObject->graphN);
						glm::vec3 intersectionPoint = ray.startPos + ray.direction * closestDistance;
						intersection = intersectionPoint;
						float forceSize = Core::CVarReadFloat(Core::CVarGet("r_force_power"));
						physWorld->forceApplier->ApplyForce(collidedObject, intersectionPoint, ray.direction * forceSize);
					}
				}

				//Debug::DrawDebugText("INTERSECT", intersection, glm::vec4(1, 0, 0, 1));
				Debug::DrawBox(intersection * invIntersectionBoxScale, glm::quat(1, 0, 0, 0), intersectionBoxScale, glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop, 1.f);

				int drawAxes = Core::CVarReadInt(Core::CVarGet("r_draw_axes"));
				if (drawAxes) {
					Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(3, 0, 0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
					Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 3, 0), 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
					Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 3), 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
				}

				PhysicsWorld* newState = new PhysicsWorld(*physWorld);
				worldStates.push_back(newState);
				Core::CVarWriteInt(Core::CVarGet("r_world_frame"), worldStates.size());
			}
			else {

				physWorld = worldStates[worldFrame - 1];

				for (int i = worldStates.size()-1; i > worldFrame; i--) {
					delete worldStates[i];
				}
				worldStates.erase(std::next(worldStates.begin(), worldFrame), std::next(worldStates.begin(), worldStates.size()));
				worldCounter = worldFrame - 1;

			}


		}
		else { //DISPLAY LAST FRAME
			worldStates[worldFrame-1]->physObjs[0]->physN->SetMass(Core::CVarReadFloat(Core::CVarGet("r_object_mass")));
			worldStates[worldFrame-1]->Tick((float)deltaTime, *this->cam);
			//physWorld = worldStates[worldFrame - 1];

			for (int i = 0; i < worldStates[worldFrame-1]->physObjs.size(); i++) {
				dirLight.ApplyLight(worldStates[worldFrame-1]->physObjs[i]->graphN->shadR->shaderProgram);
			}

		}
		this->appWindow->SwapBuffers();


		if (kbd->pressed[Input::Key::Code::Escape]) {
			this->Exit();
		}



		//Camera controls
		bool cameraControls = true;
		if (cameraControls) {
			float tickMovement = 10.f * (float)deltaTime;

			glm::mat4 transpViewMat = glm::transpose(cam->view);
			glm::vec4 fwd = transpViewMat[2] * (-1.f);
			glm::vec4 up = transpViewMat[1] * (-1.f);
			glm::vec4 right = transpViewMat[0];

			//Forward back movement
			if (kbd->held[Input::Key::Code::W]) {
				/*verCounter += tickMovement;*/
				cam->position += glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
				lookPoint += glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
			}
			if (kbd->held[Input::Key::Code::S]) {
				//verCounter -= tickMovement;
				cam->position -= glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
				lookPoint -= glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;

			}

			//Right left movement
			if (kbd->held[Input::Key::Code::D]) {
				//horCounter += tickMovement;
				cam->position += glm::vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint += glm::vec3(right.x, right.y, right.z) * tickMovement;


			}
			if (kbd->held[Input::Key::Code::A]) {
				//horCounter -= tickMovement;
				cam->position -= glm::vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint -= glm::vec3(right.x, right.y, right.z) * tickMovement;
			}

			//Up down movement
			if (kbd->held[Input::Key::Code::Space]) {
				//horCounter += tickMovement;
				cam->position.y += /*vec3(up.x, up.y, up.z) * */tickMovement;
				lookPoint.y += /*vec3(up.x, up.y, up.z) * */tickMovement;


			}
			if (kbd->held[Input::Key::Code::Shift]) {
				//horCounter -= tickMovement;
				cam->position.y -= /*vec3(up.x, up.y, up.z) * */tickMovement;
				lookPoint.y -= /*vec3(up.x, up.y, up.z) * */tickMovement;
			}

			float rotationSpeedModifier = 2.f;
			//Right left rotation
			if (kbd->held[Input::Key::Code::Right]) {
				//horCounter += tickMovement;
				//cam->position += vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint += glm::vec3(right.x, right.y, right.z) * tickMovement * rotationSpeedModifier;
			}
			if (kbd->held[Input::Key::Code::Left]) {
				//horCounter -= tickMovement;
				//cam->position -= vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint -= glm::vec3(right.x, right.y, right.z) * tickMovement * rotationSpeedModifier;
			}

			//Up down rotation
			if (kbd->held[Input::Key::Code::Down]) {
				//horCounter += tickMovement;
				//cam->position += vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint += glm::vec3(up.x, up.y, up.z) * tickMovement * rotationSpeedModifier;
			}
			if (kbd->held[Input::Key::Code::Up]) {
				//horCounter -= tickMovement;
				//cam->position -= vec3(right.x, right.y, right.z) * tickMovement;
				lookPoint -= glm::vec3(up.x, up.y, up.z) * tickMovement * rotationSpeedModifier;
			}

		} //CAMERA CONTROLS

		//std::cout << glm::length(worldStates[0]->physObjs[0]->graphN->position) << std::endl;
		//std::cout << glm::length(worldStates[worldStates.size()-1]->physObjs[0]->graphN->position) << std::endl;

		//Deltatime
		auto currentTime = GetCurrentEpochTime();
		deltaTime = (currentTime - startTime) * 0.000001;
		if (deltaTime > upperClamp) {
			deltaTime = upperClamp;
		}
		startTime = currentTime;
	}
}

void UnconstrainedDynamicsApp::Exit() {
	while (!this->worldStates.empty()) {
		PhysicsWorld* tempPointer = this->worldStates.back();
		this->worldStates.erase(this->worldStates.end() - 1);
		delete tempPointer;
		int i = 0;
	}
	App::Exit();
	this->appWindow->Close();
}

void
UnconstrainedDynamicsApp::RenderUI()
{
	if (this->appWindow->IsOpen())
	{
		ImGui::Begin("Debug");


		Core::CVar* r_draw_AABBs = Core::CVarGet("r_draw_AABBs");
		int drawAABBs = Core::CVarReadInt(r_draw_AABBs);
		if (ImGui::Checkbox("Draw AABBs", (bool*)&drawAABBs))
			Core::CVarWriteInt(r_draw_AABBs, drawAABBs);


		Core::CVar* r_draw_collider = Core::CVarGet("r_draw_collider");
		int drawCollider = Core::CVarReadInt(r_draw_collider);
		if (ImGui::Checkbox("Draw Colliders", (bool*)&drawCollider))
			Core::CVarWriteInt(r_draw_collider, drawCollider);

		Core::CVar* r_draw_axes = Core::CVarGet("r_draw_axes");
		int drawAxes = Core::CVarReadInt(r_draw_axes);
		if (ImGui::Checkbox("Draw Axes", (bool*)&drawAxes))
			Core::CVarWriteInt(r_draw_axes, drawAxes);

		//Core::CVar* r_draw_light_sphere_id = Core::CVarGet("r_draw_light_sphere_id");
		//int lightSphereId = Core::CVarReadInt(r_draw_light_sphere_id);
		//if (ImGui::InputInt("LightSphereId", (int*)&lightSphereId))
		//	Core::CVarWriteInt(r_draw_light_sphere_id, lightSphereId);

		ImGui::End();

		ImGui::Begin("Physics");


		Core::CVar* r_force_power = Core::CVarGet("r_force_power");
		float forcePower = Core::CVarReadFloat(r_force_power);
		if (ImGui::DragFloat("Force Power", (float*)&forcePower, 1.0f, 0.0f, 5000.f))
			Core::CVarWriteFloat(r_force_power, forcePower);

		Core::CVar* r_object_mass = Core::CVarGet("r_object_mass");
		float objectMass = Core::CVarReadFloat(r_object_mass);
		if (ImGui::DragFloat("Object Mass", (float*)&objectMass, 1.0f, 1.0f, 5000.f))
			Core::CVarWriteFloat(r_object_mass, objectMass);

		ImGui::End();

		ImGui::Begin("Replay");

		Core::CVar* r_pause = Core::CVarGet("r_pause");
		int pause = Core::CVarReadInt(r_pause);
		if (ImGui::Checkbox("Pause", (bool*)&pause))
			Core::CVarWriteInt(r_pause, pause);

		Core::CVar* r_world_frame = Core::CVarGet("r_world_frame");
		int  worldFrame = Core::CVarReadInt(r_world_frame);
		if (ImGui::SliderInt("World Frame", (int*)&worldFrame, 1.f , worldStates.size()))
			Core::CVarWriteInt(r_world_frame, worldFrame);

		ImGui::End();

		Debug::DispatchDebugDrawing(this->cam);
		Debug::DispatchDebugTextDrawing(this->cam);
	}
}

//Lifted from implementation done in multiplayer game for S0012E
unsigned long long UnconstrainedDynamicsApp::GetCurrentEpochTime() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto epoch_ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	return (unsigned long long)epoch_ms;
}

}