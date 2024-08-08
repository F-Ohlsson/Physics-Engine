#pragma once
#include "config.h"
#include "InputHandler.h"


InputHandler::InputHandler() {

}

InputHandler::~InputHandler() {

}

void InputHandler::CheckKeyboardInput(int32 key, int32 action) {
	switch (key) {
		case GLFW_KEY_W:
			switch (action) {
			case GLFW_PRESS:
				moveNorth = true;
				break;
			case GLFW_RELEASE:
				moveNorth = false;
				break;
			}
			break;
		case GLFW_KEY_A:
			switch (action) {
				case GLFW_PRESS:
					moveWest = true;
					break;
				case GLFW_RELEASE:
					moveWest = false;
					break;
				}
			break;		
		case GLFW_KEY_S:
			switch (action) {
				case GLFW_PRESS:
					moveSouth = true;
					break;
				case GLFW_RELEASE:
					moveSouth = false;
					break;
				}
			break;	
		case GLFW_KEY_D:
			switch (action) {
				case GLFW_PRESS:
					moveEast = true;
					break;
				case GLFW_RELEASE:
					moveEast = false;
					break;
				}
			break;
		case GLFW_KEY_SPACE:
			switch (action) {
			case GLFW_PRESS:
				if (toggle)
					toggle = false;
				else
					toggle = true;
				spacePress = true;
				break;
			case GLFW_RELEASE:
				spacePress = false;
				break;
			}
			break;
	}
}

void InputHandler::CheckMouseInput(int32 button, int32 action) {
	if (button == GLFW_MOUSE_BUTTON_1)
		if (action == GLFW_PRESS)
			rotate = true;
		else if (action == GLFW_RELEASE)
			rotate = false;
}

void InputHandler::CheckMouseMovement(float64 x, float64 y) {
	this->currentMouseCoord = { (float)x,(float)y,this->currentMouseCoord.z };
}
void InputHandler::UpdateMousePosition() {
	this->previousMouseCoord = this->currentMouseCoord;
}

void InputHandler::ApplyInput(GraphicsNode* graphN) {
	//Movement
	if (this->moveNorth)
		graphN->position.z -= 0.01f;
	if (this->moveWest)
		graphN->position.x -= 0.01f;
	if (this->moveSouth)
		graphN->position.z += 0.01f;
	if (this->moveEast)
		graphN->position.x += 0.01f;

	//Rotation
	if (this->rotate) {
		float rotationSpeed = 0.03f;
		if (this->currentMouseCoord != this->previousMouseCoord) {
			//Rotate around Y-axis based on movement along X-axis
			if(this->currentMouseCoord.x > this->previousMouseCoord.x)
				graphN->radiansY += rotationSpeed;
			else if(this->currentMouseCoord.x < this->previousMouseCoord.x)
				graphN->radiansY -= rotationSpeed;

			//Rotate around X-axis based on movement along Y-axis
			if (this->currentMouseCoord.y > this->previousMouseCoord.y)
				graphN->radiansX += rotationSpeed;
			else if (this->currentMouseCoord.y < this->previousMouseCoord.y)
				graphN->radiansX -= rotationSpeed;
		}

		//Trim radians
		//remove 2*pi if larger to prevent counter from expanding infinitely
		if (graphN->radiansX > (atan(1) * 4 * 2))
			graphN->radiansX -= float((atan(1) * 8));
		if (graphN->radiansY > (atan(1) * 4 * 2))
			graphN->radiansY -= float((atan(1) * 8));
	}	

	this->UpdateMousePosition();
}

bool InputHandler::SpaceToggle() {
	return toggle;
}