#pragma once
#include "common.h"
#include "helper.h"



struct OceanVertex
{
	GLfloat x, y, z; //pos
	GLfloat nx, ny, nz; // vertex pos
	GLfloat a, b, c; // h~
	GLfloat _a, _b, _c; // h~ conjugate
	GLfloat ox, oy, oz; // original pos
};

//used with DFT
struct ComplexVectorNormal 
{
	complex h; //wave height at time t
	glm::vec2 D; //displacement
	glm::vec3 n; //normal
};

class FFT 
{
private:
	unsigned int N, which;
	unsigned int log_2_N;

	float pi2;
	unsigned int* reversed; 
	complex** T;
	complex* c[2];
public:
	FFT(unsigned int _N);
	~FFT();
	unsigned int reverse(unsigned int i);
	complex t(unsigned int  x, unsigned int _N);
	void fft(complex* input, complex* output, int stride, int offset);
};


class Ocean 
{
private:
	float g; // gravitational constant = 9.81
	int N, Nplus1; // grid size
	float A; //phillips
	glm::vec2 w; //wind direction;
	float length; //L (length of patch)

	complex* h_tilde, * h_tilde_slopex, * h_tilde_slopez, * h_tilde_dx, * h_tilde_dz;
	FFT* fft;

	OceanVertex* vertices;
	unsigned int* indices;
	unsigned int indices_count;
	GLuint vbo_vertices, vbo_indices, vao;

	GLuint program, vertex_shader, frag_shader;
	GLint vertex, normal, texture, light_position, projection, view, model, cam_position; //shader attribs

public:
	Ocean(const int _N, const float _A, const glm::vec2 _w, const float _length);
	~Ocean();
	void cleanup();

	float dispersion(int n, int m); //dispersion relation
	float phillips(int n, int m); //phillips spectrum

	complex hTilde_0(int n, int m);
	complex hTilde(float t, int n, int m);
	ComplexVectorNormal h_D_n(glm::vec2 x, float t);

	void evaluateWaves(float t);
	void render(float t, glm::vec3 light_pos, glm::vec3 cam_pos,glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, int tiling);
};
