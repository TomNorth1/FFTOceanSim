#include "Camera.h"
#include <iostream>

Camera::Camera(int _width, int _height, glm::vec3 _position)
{
	width = _width;
	height = _height;
	position = _position;
}

glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(position, position + orientation, upVector);

	return view;
}

glm::mat4 Camera::getProjectionMatrix(float FOV, float nearPlane, float farPlane)
{
	glm::mat4 projection = glm::perspective(glm::radians(FOV), float(width / height), nearPlane, farPlane);
	return projection;
}

void Camera::handleInput(SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN) 
	{
		if (event->key.keysym.sym == SDLK_w) 
		{
			position += speed * orientation;
		}
		if (event->key.keysym.sym == SDLK_a)
		{
			position += speed * -glm::normalize(glm::cross(orientation, upVector));
		}
		if (event->key.keysym.sym == SDLK_s)
		{
			position += speed * -orientation;
		}
		if (event->key.keysym.sym == SDLK_d)
		{
			position += speed * glm::normalize(glm::cross(orientation, upVector));
		}
		if (event->key.keysym.sym == SDLK_LCTRL)
		{
			position += speed * -upVector;
		}
		if (event->key.keysym.sym == SDLK_SPACE)
		{
			position += speed * upVector;
		}
	}
	int mouseX, mouseY;
	if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON_RMASK)
	{
		SDL_ShowCursor(SDL_DISABLE);
		if (firstClick) 
		{
			SDL_WarpMouseInWindow(NULL, (width / 2), (height / 2));
			firstClick = false;
		}
		SDL_GetMouseState(&mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, upVector)));

		if (!((glm::angle(newOrientation, upVector))) <= glm::radians(5.0f) || (glm::angle(newOrientation, -upVector)) <= glm::radians(5.0f))
		{
			orientation = newOrientation;
		}
		orientation = glm::rotate(orientation, glm::radians(-rotY), upVector);
		SDL_WarpMouseInWindow(NULL, (width / 2), (height / 2));
	}

	if (event->type == SDL_MOUSEBUTTONUP)
	{
		if (event->button.button == SDL_BUTTON_RIGHT)
		{
			SDL_ShowCursor(SDL_ENABLE);
			firstClick = true;
		}
	}


}


