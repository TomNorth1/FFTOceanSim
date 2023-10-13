#pragma once
#include "common.h"
#include "SDL2/SDL.h"

class Camera 
{
public:
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	int width; 
	int height;

	bool firstClick = true; 

	float speed = 5.f;
	float sensitivity = 100.0f;

	Camera(int _width, int _height, glm::vec3 _position);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix(float FOV, float nearPlane, float farPlane);
	void handleInput(SDL_Event *event);


};