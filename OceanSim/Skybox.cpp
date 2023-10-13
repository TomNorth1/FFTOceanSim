#include "Skybox.h"
#include <string>
#include "Shader.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


float vertices[] =
{
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f
};

unsigned int indices[] =
{
	1,2,6,
	6,5,1,

	0,4,7,
	7,3,0,

	4,5,6,
	6,7,4,

	0,3,2,
	2,1,0,

	0,1,5,
	5,4,0,

	3,7,6,
	6,2,3
};


std::string faces[] =
{
	"resources/textures/clouds1_east.bmp",
	"resources/textures/clouds1_west.bmp",
	"resources/textures/clouds1_up.bmp",
	"resources/textures/clouds1_down.bmp",
	"resources/textures/clouds1_north.bmp",
	"resources/textures/clouds1_south.bmp"
};

Skybox::Skybox() 
{
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i <6; i++) 
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) 
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	createProgram(program, vertex_shader, frag_shader, "shaders/skybox_vert.shader", "shaders/skybox_frag.shader");

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	
}

Skybox::~Skybox()
{
	releaseProgram(program, vertex_shader, frag_shader);
}

void Skybox::render(Camera* cam, glm::mat4 proj)
{
	glDepthFunc(GL_LEQUAL);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::mat4(glm::mat3(glm::lookAt(cam->position, cam->position+ cam->orientation, cam->upVector)));
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));


	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

