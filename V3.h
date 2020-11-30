
#pragma once

#include <ostream>

using namespace std;

class V3 {
public:
	V3() {};
	V3(float x, float y, float z) { xyz[0] = x; xyz[1] = y; xyz[2] = z; };
	float xyz[3];
	float& operator[](int i);
	V3 operator+(V3 v1);
	V3 operator-(V3 v1);
	// cerr << v;
	friend ostream& operator<<(ostream& ostr, V3 v);
	float operator*(V3 v1);
	V3 operator*(float scf);
	float Length();
	V3 Normalized();
	V3 operator/(float scf);
	V3 operator^(V3 v2);
	void SetFromColor(unsigned int color);
	unsigned int GetColor();
	V3 RotatePoint(V3 aO, V3 adir, float theta);
	V3 RotateVector(V3 adir, float theta);
	V3 Light(V3 lv, V3 nv, float ka);
	V3 Reflect(V3 ray);
	V3 reflection(V3 normal_n); //my reflection

	V3 UnitVector();


	V3 RotateThisPointAboutArbitraryAxis(V3 O, V3 a, float angled);     //i
	V3 RotateThisVectorAboutDirection(V3 a, float angled);				//k
};
