#include "Ocean.h"
#include "Shader.h"
#include <iostream>


FFT::FFT(unsigned int _N) 
{
	N = _N;
	reversed = 0;
	T = 0;
	pi2 = 2 * M_PI;
	c[0] = c[1] = 0;

	log_2_N = log(N) / log(2);

	reversed = new unsigned int[N];

	for (int i =0; i < N; i++) 
	{
		reversed[i] = reverse(i);
	}

	int pow2 = 1;
	T = new complex * [log_2_N];      
	for (int i = 0; i < log_2_N; i++) {
		T[i] = new complex[pow2];
		for (int j = 0; j < pow2; j++) T[i][j] = t(j, pow2 * 2);
		pow2 *= 2;
	}

	c[0] = new complex[N];
	c[1] = new complex[N];
	which = 0;
}


FFT::~FFT() 
{
	if (c[0]) delete[] c[0];
	if (c[1]) delete[] c[1];
	if (T) {
		for (int i = 0; i < log_2_N; i++) if (T[i]) delete[] T[i];
		delete[] T;
	}
	if (reversed) delete[] reversed;
}


unsigned int FFT::reverse(unsigned int i) 
{
	unsigned int res = 0;
	for (int j = 0; j < log_2_N; j++) {
		res = (res << 1) + (i & 1);
		i >>= 1;
	}
	return res;
}

complex FFT::t(unsigned int x, unsigned int N) 
{
	return complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void FFT::fft(complex* input, complex* output, int stride, int offset) {
	for (int i = 0; i < N; i++) c[which][i] = input[reversed[i] * stride + offset];

	int loops = N >> 1;
	int size = 1 << 1;
	int size_over_2 = 1;
	int w_ = 0;
	for (int i = 1; i <= log_2_N; i++) {
		which ^= 1;
		for (int j = 0; j < loops; j++) {
			for (int k = 0; k < size_over_2; k++) {
				c[which][size * j + k] = c[which ^ 1][size * j + k] +
					c[which ^ 1][size * j + size_over_2 + k] * T[w_][k];
			}

			for (int k = size_over_2; k < size; k++) {
				c[which][size * j + k] = c[which ^ 1][size * j - size_over_2 + k] -
					c[which ^ 1][size * j + k] * T[w_][k - size_over_2];
			}
		}
		loops >>= 1;
		size <<= 1;
		size_over_2 <<= 1;
		w_++;
	}

	for (int i = 0; i < N; i++) output[i * stride + offset] = c[which][i];
}

Ocean::Ocean(const int _N, const float _A, const glm::vec2 _w, const float _length)
{
	g = 9.81;
	N = _N;
	Nplus1 = _N + 1;
	A = _A;
	w = _w;
	length = _length;
	

	h_tilde = new complex[N * N];
	h_tilde_slopex = new complex[N * N];
	h_tilde_slopez = new complex[N * N];
	h_tilde_dx = new complex[N * N];
	h_tilde_dz = new complex[N * N];

	fft = new FFT(N);


	vertices = new OceanVertex[Nplus1 * Nplus1]; 
	indices = new unsigned int[Nplus1 * Nplus1 * 10];
	
	int index;

	complex htilde0, htilde0_conj;
	for (int m = 0; m < Nplus1; m++)
	{
		for (int n = 0; n < Nplus1; n++) 
		{
			index = m * Nplus1 + n;

			htilde0 = hTilde_0(n, m);
			htilde0_conj = hTilde_0(-n, -m).conjugate(); 

			vertices[index].a = htilde0.a;
			vertices[index].b = htilde0.b;
			vertices[index]._a = htilde0_conj.a;
			vertices[index]._b = htilde0_conj.b;

			vertices[index].ox = (n - N / 2.0f) * length / N;
			vertices[index].x = (n - N / 2.0f) * length / N;
			vertices[index].oy = 0.0f;
			vertices[index].y = 0.0f;
			vertices[index].oz = (m - N / 2.0f) * length / N;
			vertices[index].z = (m - N / 2.0f) * length / N;

			vertices[index].nx = 0.0f;
			vertices[index].ny = 1.0f;
			vertices[index].nz = 0.0f;
		}
	}

	indices_count = 0;
	for (int m = 0; m < N; m++) 
	{
		for (int n = 0; n < N; n++) 
		{
			index = m * Nplus1 + n;

			
			indices[indices_count++] = index;
			indices[indices_count++] = index + Nplus1;
			indices[indices_count++] = index + Nplus1 + 1;
			indices[indices_count++] = index;
			indices[indices_count++] = index + Nplus1 + 1;
			indices[indices_count++] = index + 1;
			
		}
	}

	createProgram(program, vertex_shader, frag_shader, "shaders/basic_vert.shader", "shaders/ocean_frag.shader");

	vertex = glGetAttribLocation(program, "vertex");
	normal = glGetAttribLocation(program, "normal");
	texture = glGetAttribLocation(program, "texture");
	light_position = glGetUniformLocation(program, "light_position");
	projection = glGetUniformLocation(program, "proj");
	view = glGetUniformLocation(program, "view");
	model = glGetUniformLocation(program, "model");
	cam_position = glGetUniformLocation(program, "camera_position");


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(OceanVertex) * (Nplus1) * (Nplus1), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &vbo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Ocean::~Ocean() 
{
	if (vertices) delete[] vertices;
	if (indices) delete[] indices;
}

void Ocean::cleanup()
{
	glDeleteBuffers(1, &vbo_indices);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vao);
	releaseProgram(program, vertex_shader, frag_shader);
}



float Ocean::dispersion(int n, int m) 
{
	float w_0 = 2.0f * M_PI / 200.0f;
	float kx = M_PI * (2 * n - N) / length;
	float kz = M_PI * (2 * m - N) / length;
	return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float Ocean::phillips(int n, int m)
{
	glm::vec2 k(M_PI * (2 * n - N) / length, M_PI * (2 * m - N) / length);
	float k_length = glm::length(k);
	if (k_length < 0.000001) return 0.0;

	float k_length2 = k_length * k_length;
	float k_length4 = k_length2 * k_length2;

	float k_dot_w = glm::dot(glm::normalize(k), glm::normalize(w));
	float k_dot_w2 = k_dot_w * k_dot_w;

	float w_length = glm::length(w);
	float L = w_length * w_length / g;
	float L2 = L * L;

	float damping = 0.001;
	float l2 = L2 * damping * damping;

	return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

complex Ocean::hTilde_0(int n, int m)
{
		complex r = gaussianRandomVariable();
		return r * sqrt(phillips(n, m) / 2.0f);
}

complex Ocean::hTilde(float t, int n, int m)
{
	
	int index = m * Nplus1 + n;

	complex htilde0(vertices[index].a, vertices[index].b);
	complex htilde0conj(vertices[index]._a, vertices[index]._b);

	float omega_t = dispersion(n, m) * t;

	float cos_ = cos(omega_t);
	float sin_ = sin(omega_t);

	complex c0(cos_, sin_);
	complex c1(cos_, -sin_);

	complex res = htilde0 * c0 + htilde0conj * c1; // might cause problems
	
	return res;
}

ComplexVectorNormal Ocean::h_D_n(glm::vec2 x, float t) 
{
	complex height(0.0f, 0.0f);
	glm::vec2 displacement(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 0.0f, 0.0f);

	complex c, res, htilde_c;


	glm::vec2 k;
	float kx, kz, k_length, k_dot_x;

	for (int m = 0; m < N; m++) 
	{
		kz = 2.0f * M_PI * (m - N / 2.0f) / length;
		for (int n =0; n < N; n++)
		{
			kx = 2.0f * M_PI * (n - N / 2.0f) / length;
			k = glm::vec2(kx, kz);

			k_length = glm::length(k);
			k_dot_x = glm::dot(k, x);

			c = complex(cos(k_dot_x), sin(k_dot_x));
			htilde_c = hTilde(t, n, m) * c;

			height = height + htilde_c;

			normal = normal + glm::vec3(-kx * htilde_c.b, 0.0f, kz * htilde_c.b);

			if (k_length < 0.000001) continue;
			displacement = displacement + glm::vec2(kx / k_length * htilde_c.b, kz /k_length * htilde_c.b);
		}

	}

	normal = (glm::vec3(0.0f, 1.0f, 0.0f) - glm::normalize(normal));

	ComplexVectorNormal result; 
	result.h = height;
	result.D = displacement;
	result.n = normal;
	return result;
}

void Ocean::evaluateWaves(float t) {
	float kx, kz, len, lambda = -1.0f;
	int index, index1;

	for (int m_prime = 0; m_prime < N; m_prime++) {
		kz = M_PI * (2.0f * m_prime - N) / length;
		for (int n_prime = 0; n_prime < N; n_prime++) {
			kx = M_PI * (2 * n_prime - N) / length;
			len = sqrt(kx * kx + kz * kz);
			index = m_prime * N + n_prime;

			h_tilde[index] = hTilde(t, n_prime, m_prime);
			h_tilde_slopex[index] = h_tilde[index] * complex(0, kx);
			h_tilde_slopez[index] = h_tilde[index] * complex(0, kz);
			if (len < 0.000001f) {
				h_tilde_dx[index] = complex(0.0f, 0.0f);
				h_tilde_dz[index] = complex(0.0f, 0.0f);
			}
			else {
				h_tilde_dx[index] = h_tilde[index] * complex(0, -kx / len);
				h_tilde_dz[index] = h_tilde[index] * complex(0, -kz / len);
			}
		}
	}

	for (int m_prime = 0; m_prime < N; m_prime++) {
		fft->fft(h_tilde, h_tilde, 1, m_prime * N);
		fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
		fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
		fft->fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
		fft->fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
	}
	for (int n_prime = 0; n_prime < N; n_prime++) {
		fft->fft(h_tilde, h_tilde, N, n_prime);
		fft->fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
		fft->fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
		fft->fft(h_tilde_dx, h_tilde_dx, N, n_prime);
		fft->fft(h_tilde_dz, h_tilde_dz, N, n_prime);
	}

	int sign;
	float signs[] = { 1.0f, -1.0f };
	glm::vec3 n;
	for (int m_prime = 0; m_prime < N; m_prime++) {
		for (int n_prime = 0; n_prime < N; n_prime++) {
			index = m_prime * N + n_prime;    
			index1 = m_prime * Nplus1 + n_prime;   

			sign = signs[(n_prime + m_prime) & 1];

			h_tilde[index] = h_tilde[index] * sign;

			// height
			vertices[index1].y = h_tilde[index].a;

			// displacement
			h_tilde_dx[index] = h_tilde_dx[index] * sign;
			h_tilde_dz[index] = h_tilde_dz[index] * sign;
			vertices[index1].x = vertices[index1].ox + h_tilde_dx[index].a * lambda;
			vertices[index1].z = vertices[index1].oz + h_tilde_dz[index].a * lambda;

			// normal
			h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
			h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
			n = glm::normalize(glm::vec3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a));
			vertices[index1].nx = n.x;
			vertices[index1].ny = n.y;
			vertices[index1].nz = n.z;

			
			if (n_prime == 0 && m_prime == 0) {
				vertices[index1 + N + Nplus1 * N].y = h_tilde[index].a;

				vertices[index1 + N + Nplus1 * N].x = vertices[index1 + N + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
				vertices[index1 + N + Nplus1 * N].z = vertices[index1 + N + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

				vertices[index1 + N + Nplus1 * N].nx = n.x;
				vertices[index1 + N + Nplus1 * N].ny = n.y;
				vertices[index1 + N + Nplus1 * N].nz = n.z;
			}
			if (n_prime == 0) {
				vertices[index1 + N].y = h_tilde[index].a;

				vertices[index1 + N].x = vertices[index1 + N].ox + h_tilde_dx[index].a * lambda;
				vertices[index1 + N].z = vertices[index1 + N].oz + h_tilde_dz[index].a * lambda;

				vertices[index1 + N].nx = n.x;
				vertices[index1 + N].ny = n.y;
				vertices[index1 + N].nz = n.z;
			}
			if (m_prime == 0) {
				vertices[index1 + Nplus1 * N].y = h_tilde[index].a;

				vertices[index1 + Nplus1 * N].x = vertices[index1 + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
				vertices[index1 + Nplus1 * N].z = vertices[index1 + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

				vertices[index1 + Nplus1 * N].nx = n.x;
				vertices[index1 + Nplus1 * N].ny = n.y;
				vertices[index1 + Nplus1 * N].nz = n.z;
			}
		}
	}
}

void Ocean::render(float t, glm::vec3 light_pos, glm::vec3 cam_pos, glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, int tiling)
{

	
	evaluateWaves(t);
	

	glUseProgram(program);
	glUniform3f(light_position, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(cam_position, cam_pos.x, cam_pos.y, cam_pos.z);
	glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(OceanVertex) * Nplus1 * Nplus1, vertices);
	glEnableVertexAttribArray(vertex);
	glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), 0);
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (char*)NULL + 12);
	glEnableVertexAttribArray(texture);
	glVertexAttribPointer(texture, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (char*)NULL + 24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	for (int j = 0; j < tiling; j++) {
		for (int i = 0; i < tiling; i++) {
			Model = glm::scale(glm::mat4(1.0f), glm::vec3(5.f, 5.f, 5.f));
			Model = glm::translate(Model, glm::vec3(length * i, 0, length * -j));
			glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));
			glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
		}
	}
}


