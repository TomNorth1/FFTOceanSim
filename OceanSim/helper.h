#ifndef HELPER_H
#define HELPER_H

//helper class for complex number containing both imaginary and real numbers

class complex
{
public:
	float a, b;
	complex();
	complex(float a, float b);
	complex conjugate();

	complex operator*(const complex& c) const;
	complex operator+(const complex& c) const;
	complex operator-(const complex& c) const;
	complex operator*(const float c) const;
	complex& operator=(const complex& c);
};

complex gaussianRandomVariable();




#endif // !HELPER_H
