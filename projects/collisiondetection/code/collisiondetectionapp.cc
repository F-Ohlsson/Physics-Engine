#pragma once
#include "config.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"
#include <iostream>
#include "collisiondetectionapp.h"
#include "core/physicscourse/Plane.h"
#include <chrono>
#include "core/cvar.h"
#include "gtc/random.hpp"

namespace Game {

	CollisionDetectionApp::CollisionDetectionApp() {
		cam = nullptr;
		this->picker = new Picker();
		this->physWorld = new PhysicsWorld();
	}

	CollisionDetectionApp::~CollisionDetectionApp() {

	}

	bool CollisionDetectionApp::Open() {
		App::Open();

		Core::CVarCreate(Core::CVar_Float, "r_force_power", "100.f");
		Core::CVarCreate(Core::CVar_Float, "r_object_mass", "5.f");


		int width = 1000;
		int height = 700;

		this->appWindow = new Display::Window();
		this->appWindow->SetSize(width, height);
		this->appWindow->Open();
		this->cam = new Camera(width, height, { 10.0f,5.0f,10.0f });
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

	void CollisionDetectionApp::Close() {
		App::Close();
	}

	void CollisionDetectionApp::Run() {
		glEnable(GL_DEPTH_TEST);
		Input::Keyboard* kbd = Input::GetDefaultKeyboard();
		Input::Mouse* mouse = Input::GetDefaultMouse();

		this->cam->projection = this->cam->ProjectionMatrix(1.0f, 0.1f, 2000.0f);

		DirLight dirLight = DirLight();
		dirLight.direction = { 0.0f,-1.0f,-2.3f };
		dirLight.colour = { 1.0f,1.0f,1.0f,1.0f };
		dirLight.intensity = 1.0f;

		Core::CVarWriteInt(Core::CVarGet("r_runResponse"), 0);
		Core::CVarWriteInt(Core::CVarGet("r_damping"), 0);
		Core::CVarWriteInt(Core::CVarGet("r_draw_EPA_results"), 0);


		PhysicsObject* gameObject1 = new PhysicsObject();


		gameObject1->graphN = new GraphicsNode();
		gameObject1->physN = new PhysicsNode();
		gameObject1->graphN->parent = gameObject1;
		gameObject1->physN->parent = gameObject1;

		gameObject1->graphN->position = { 2,2,0 };


		gameObject1->graphN->scaling = 1.f;
		gameObject1->graphN->LoadGLTF("../resources/gltf/cylinder.gltf");
		gameObject1->physN->LoadColliderGLTF("../resources/gltf/cylinder.gltf");


		gameObject1->physN->SetMass(10.f);
		gameObject1->physN->SetInertiaTensor(e_cylinder);

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
		gameObject1->graphN->textR->LoadTexture("../resources/textures/OHNO.png", true);
		gameObject1->graphN->textR->BindTexture(gameObject1->graphN->textR->texture);

		TextureResource textR = TextureResource();
		textR.LoadTexture("../resources/textures/STRIPES.png", true);
		textR.BindTexture(textR.texture);

		bool testObjects = true;
		if (testObjects) {

			bool cubes = true;
			if (cubes) {
				////SET UP A BUNCH OF CUBES
				for (int i = 0; i < 15; i++) {
					PhysicsObject* newGameObj = new PhysicsObject();
					newGameObj->graphN = new GraphicsNode();
					newGameObj->physN = new PhysicsNode();
					newGameObj->graphN->parent = newGameObj;
					newGameObj->physN->parent = newGameObj;

					float x = glm::linearRand(-10.f, 10.f);
					float y = glm::linearRand(-10.f, 10.f);
					float z = glm::linearRand(-10.f, 10.f);


					newGameObj->graphN->shadR = gameObject1->graphN->shadR;

					newGameObj->graphN->position = { x,y,z };
					newGameObj->graphN->scaling = 1.f;
					newGameObj->graphN->LoadGLTF("../resources/gltf/cube.gltf");
					newGameObj->physN->SetMass(10.f);
					newGameObj->physN->SetInertiaTensor(e_cuboid);
					newGameObj->physN->angularVelocity = glm::vec3(glm::linearRand(-0.2f, 0.2f));

					physWorld->physObjs.push_back(newGameObj);
				}
			}
			bool cylinders = true;
			if (cylinders) {
				////SET UP A BUNCH OF CYLINDERS
				for (int i = 0; i < 15; i++) {
					PhysicsObject* newGameObj = new PhysicsObject();
					newGameObj->graphN = new GraphicsNode();
					newGameObj->physN = new PhysicsNode();
					newGameObj->graphN->parent = newGameObj;
					newGameObj->physN->parent = newGameObj;

					float x = glm::linearRand(-10.f, 10.f);
					float y = glm::linearRand(-10.f, 10.f);
					float z = glm::linearRand(-10.f, 10.f);


					newGameObj->graphN->shadR = gameObject1->graphN->shadR;

					newGameObj->graphN->position = { x,y,z };
					newGameObj->graphN->scaling = 1.f;
					newGameObj->graphN->LoadGLTF("../resources/gltf/cylinder.gltf");
					newGameObj->physN->SetMass(10.f);
					newGameObj->physN->SetInertiaTensor(e_cylinder);
					newGameObj->physN->angularVelocity = glm::vec3(glm::linearRand(-0.2f, 0.2f));

					physWorld->physObjs.push_back(newGameObj);
				}
			}

			bool spheres = true;
			if (spheres) {
				////SET UP A BUNCH OF SPHERES
				for (int i = 0; i < 20; i++) {
					PhysicsObject* newGameObj = new PhysicsObject();
					newGameObj->graphN = new GraphicsNode();
					newGameObj->physN = new PhysicsNode();
					newGameObj->graphN->parent = newGameObj;
					newGameObj->physN->parent = newGameObj;

					float x = glm::linearRand(-10.f, 10.f);
					float y = glm::linearRand(-10.f, 10.f);
					float z = glm::linearRand(-10.f, 10.f);


					newGameObj->graphN->shadR = gameObject1->graphN->shadR;

					newGameObj->graphN->position = { x,y,z };
					newGameObj->graphN->scaling = 1.f;
					newGameObj->graphN->LoadGLTF("../resources/gltf/icosphere.gltf");
					newGameObj->physN->SetMass(10.f);
					newGameObj->physN->SetInertiaTensor(e_sphere);
					newGameObj->physN->angularVelocity = glm::vec3(glm::linearRand(-0.2f, 0.2f));

					physWorld->physObjs.push_back(newGameObj);
				}
			}
		}

		BlinnPhongMaterial bpMaterial = BlinnPhongMaterial(gameObject1->graphN->shadR->shaderProgram);
		bpMaterial.shininess = 10.0f;
		bpMaterial.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
		bpMaterial.diffuse = { 1.0f,1.0f,1.0f,1.0f };
		bpMaterial.specular = { 1.0f,1.0f,1.0f,1.0f };
		bpMaterial.texture = new GLuint(gameObject1->graphN->textR->texture);
		BlinnPhongMaterial highlightMaterial = bpMaterial;
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


		glm::vec3 lookPoint = glm::vec3(0, 0, 0);


		//Time stuff
		double upperClamp = 1 / 60.0;
		double deltaTime = upperClamp;
		auto startTime = GetCurrentEpochTime();

		//Force stuff
		glm::vec3 intersection = glm::vec3(0, 0, 0);
		float intersectionBoxScale = 0.5;
		float invIntersectionBoxScale = 1 / 0.5;


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
					physWorld->Tick((float)deltaTime, *this->cam);


					for (int i = 0; i < physWorld->physObjs.size(); i++) {
						dirLight.ApplyLight(physWorld->physObjs[i]->graphN->shadR->shaderProgram);
					}

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



					PhysicsWorld* newState = new PhysicsWorld(*physWorld);
					worldStates.push_back(newState);
					Core::CVarWriteInt(Core::CVarGet("r_world_frame"), worldStates.size());
				}
				else {

					physWorld = worldStates[worldFrame - 1];

					for (int i = worldStates.size() - 1; i > worldFrame; i--) {
						delete worldStates[i];
					}
					worldStates.erase(std::next(worldStates.begin(), worldFrame), std::next(worldStates.begin(), worldStates.size()));
					worldCounter = worldFrame - 1;

				}


			}
			else { //DISPLAY LAST FRAME
				worldStates[worldFrame - 1]->Tick((float)deltaTime, *this->cam);

				for (int i = 0; i < worldStates[worldFrame - 1]->physObjs.size(); i++) {
					dirLight.ApplyLight(worldStates[worldFrame - 1]->physObjs[i]->graphN->shadR->shaderProgram);
				}

			}

			//Draw axes if troggled in UI
			if (Core::CVarReadInt(Core::CVarGet("r_draw_axes"))) {
				Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(3, 0, 0), 1.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);
				Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 3, 0), 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
				Debug::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 3), 1.0f, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 1, 1), Debug::RenderMode::AlwaysOnTop);
				Debug::DrawDebugText("X", glm::vec3(2, 0, 0), glm::vec4(1, 0, 0, 1));
				Debug::DrawDebugText("Y", glm::vec3(0, 2, 0), glm::vec4(0, 1, 0, 1));
				Debug::DrawDebugText("Z", glm::vec3(0, 0, 2), glm::vec4(0, 0, 1, 1));
			}

			this->appWindow->SwapBuffers();


			if (kbd->pressed[Input::Key::Code::Escape]) {
				this->Exit();
			}

			Core::CVar* r_step_forward = Core::CVarGet("r_step_forward");
			int stepFwd = Core::CVarReadInt(r_step_forward);
			Core::CVar* r_step_backward = Core::CVarGet("r_step_backward");
			int stepBkwd = Core::CVarReadInt(r_step_backward);
			if (stepFwd) {
				Core::CVar* r_world_frame = Core::CVarGet("r_world_frame");
				int  worldFrame = Core::CVarReadInt(r_world_frame);
				if (worldFrame < worldStates.size()) {
					worldFrame++;
					Core::CVarWriteInt(r_world_frame, worldFrame);
				}
				Core::CVarWriteInt(r_step_forward, 0);
			}
			else if (stepBkwd) {
				Core::CVar* r_world_frame = Core::CVarGet("r_world_frame");
				int  worldFrame = Core::CVarReadInt(r_world_frame);
				if (worldFrame > 1) {
					worldFrame--;
					Core::CVarWriteInt(r_world_frame, worldFrame);
				}
				Core::CVarWriteInt(r_step_backward, 0);
			}




			//Camera controls
			bool cameraControls = true;
			if (cameraControls) {
				float tickMovement = 10.f * (float)deltaTime;

				if (kbd->held[Input::Key::Code::G]) {
					tickMovement = 2.f * (float)deltaTime;
				}

				glm::mat4 transpViewMat = glm::transpose(cam->view);
				glm::vec4 fwd = transpViewMat[2] * (-1.f);
				glm::vec4 up = transpViewMat[1] * (-1.f);
				glm::vec4 right = transpViewMat[0];

				//Forward back movement
				if (kbd->held[Input::Key::Code::W]) {
					cam->position += glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
					lookPoint += glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
				}
				if (kbd->held[Input::Key::Code::S]) {
					cam->position -= glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;
					lookPoint -= glm::vec3(fwd.x, fwd.y, fwd.z) * tickMovement;

				}

				//Right left movement
				if (kbd->held[Input::Key::Code::D]) {
					cam->position += glm::vec3(right.x, right.y, right.z) * tickMovement;
					lookPoint += glm::vec3(right.x, right.y, right.z) * tickMovement;


				}
				if (kbd->held[Input::Key::Code::A]) {
					cam->position -= glm::vec3(right.x, right.y, right.z) * tickMovement;
					lookPoint -= glm::vec3(right.x, right.y, right.z) * tickMovement;
				}

				//Up down movement
				if (kbd->held[Input::Key::Code::Space]) {
					cam->position.y += tickMovement;
					lookPoint.y += tickMovement;


				}
				if (kbd->held[Input::Key::Code::Shift]) {
					cam->position.y -= tickMovement;
					lookPoint.y -= tickMovement;
				}

				float rotationSpeedModifier = 2.f;
				//Right left rotation
				if (kbd->held[Input::Key::Code::Right]) {
					lookPoint += glm::vec3(right.x, right.y, right.z) * tickMovement * rotationSpeedModifier;
				}
				if (kbd->held[Input::Key::Code::Left]) {
					lookPoint -= glm::vec3(right.x, right.y, right.z) * tickMovement * rotationSpeedModifier;
				}

				//Up down rotation
				if (kbd->held[Input::Key::Code::Down]) {
					lookPoint += glm::vec3(up.x, up.y, up.z) * tickMovement * rotationSpeedModifier;
				}
				if (kbd->held[Input::Key::Code::Up]) {
					lookPoint -= glm::vec3(up.x, up.y, up.z) * tickMovement * rotationSpeedModifier;
				}

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

	void CollisionDetectionApp::Exit() {
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
		CollisionDetectionApp::RenderUI()
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

			Core::CVar* r_draw_EPA_results = Core::CVarGet("r_draw_EPA_results");
			int drawEPA = Core::CVarReadInt(r_draw_EPA_results);
			if (ImGui::Checkbox("Draw EPA Results", (bool*)&drawEPA))
				Core::CVarWriteInt(r_draw_EPA_results, drawEPA);

			ImGui::End();

			ImGui::Begin("Physics");

			Core::CVar* r_runEPA = Core::CVarGet("r_runEPA");
			int runEPA = Core::CVarReadInt(r_runEPA);
			if (ImGui::Checkbox("Run EPA", (bool*)&runEPA))
				Core::CVarWriteInt(r_runEPA, runEPA);

			Core::CVar* r_damping = Core::CVarGet("r_damping");
			int enableDamping = Core::CVarReadInt(r_damping);
			if (ImGui::Checkbox("Damping", (bool*)&enableDamping))
				Core::CVarWriteInt(r_damping, enableDamping);

			Core::CVar* r_gravity = Core::CVarGet("r_gravity");
			int enableGravity = Core::CVarReadInt(r_gravity);
			if (ImGui::Checkbox("Gravity", (bool*)&enableGravity))
				Core::CVarWriteInt(r_gravity, enableGravity);

			Core::CVar* r_force_power = Core::CVarGet("r_force_power");
			float forcePower = Core::CVarReadFloat(r_force_power);
			if (ImGui::DragFloat("Force Power", (float*)&forcePower, 1.0f, 0.0f, 5000.f))
				Core::CVarWriteFloat(r_force_power, forcePower);

			ImGui::End();

			ImGui::Begin("Replay");

			Core::CVar* r_pause = Core::CVarGet("r_pause");
			int pause = Core::CVarReadInt(r_pause);
			if (ImGui::Checkbox("Pause", (bool*)&pause))
				Core::CVarWriteInt(r_pause, pause);

			//Only access rewind if paused
			if (pause) {

				Core::CVar* r_world_frame = Core::CVarGet("r_world_frame");
				int  worldFrame = Core::CVarReadInt(r_world_frame);
				if (ImGui::SliderInt("World Frame", (int*)&worldFrame, 1.f, worldStates.size()))
					Core::CVarWriteInt(r_world_frame, worldFrame);

				Core::CVar* r_step_forward = Core::CVarGet("r_step_forward");
				int stepFwd = Core::CVarReadInt(r_step_forward);
				if (ImGui::Button("Step Forward"))
					Core::CVarWriteInt(r_step_forward, 1);

				Core::CVar* r_step_backward = Core::CVarGet("r_step_backward");
				int stepBkwd = Core::CVarReadInt(r_step_backward);
				if (ImGui::Button("Step Backward"))
					Core::CVarWriteInt(r_step_backward, 1);


			}
			ImGui::End();

			Debug::DispatchDebugDrawing(this->cam);
			Debug::DispatchDebugTextDrawing(this->cam);
		}
	}

	//Lifted from implementation done in multiplayer game for S0012E
	unsigned long long CollisionDetectionApp::GetCurrentEpochTime() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto epoch_ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	return (unsigned long long)epoch_ms;
}

}