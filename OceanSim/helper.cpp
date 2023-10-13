#include "helper.h"
#include <math.h>
#include <stdlib.h>

complex::complex() : a(0.0f), b(0.0f) { }
complex::complex(float a, float b) : a(a), b(b) { }
complex complex::conjugate() { return complex(this->a, -this->b); }

complex gaussianRandomVariable()
{
	float x1, x2, w;
	do {
		x1 = 2.f * (float)rand() / RAND_MAX - 1.f;
		x2 = 2.f * (float)rand() / RAND_MAX - 1.f;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.f);
	w = sqrt((-2.f * log(w)) / w);
	return complex(x1 * w, x2 * w);
	//return complex();
}


complex complex::operator*(const complex& c) const {
	return complex(this->a * c.a - this->b * c.b, this->a * c.b + this->b * c.a);
}

complex complex::operator+(const complex& c) const {
	return complex(this->a + c.a, this->b + c.b);
}

complex complex::operator-(const complex& c) const {
	return complex(this->a - c.a, this->b - c.b);
}

complex complex::operator*(const float c) const {
	return complex(this->a * c, this->b * c);
}

complex& complex::operator=(const complex& c) {
	this->a = c.a; this->b = c.b;
	return *this;
}