#pragma once
#include "common.h"
#include "Camera.h"

class Skybox 
{

private:
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	unsigned int cubemapTexture;
	GLuint program, vertex_shader, frag_shader;
public:
	Skybox();
	~Skybox();
	void render(Camera* cam, glm::mat4 proj);
	unsigned int GetCubemapTexture() { return cubemapTexture; }
};