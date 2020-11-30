#include "stdafx.h"

#include "V3.h"
#include "M33.h"

float& V3::operator[](int i) {

	return xyz[i];

}

V3 V3::operator+(V3 v1) {

	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] + v1[0];
	ret[1] = v0[1] + v1[1];
	ret[2] = v0[2] + v1[2];
	return ret;

}

V3 V3::operator-(V3 v1) {

	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] - v1[0];
	ret[1] = v0[1] - v1[1];
	ret[2] = v0[2] - v1[2];
	return ret;

}



ostream& operator<<(ostream& ostr, V3 v) {

	return ostr << v[0] << " " << v[1] << " " << v[2] << endl;

}

float V3::operator*(V3 v1) {

	V3 &v0 = *this;
	float ret = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
	return ret;

}

float V3::Length() {
	
	V3 &v0 = *this;
	return sqrtf(v0*v0);

}

V3 V3::operator/(float scf) {

	V3 ret(*this);
	ret[0] = ret[0] / scf;
	ret[1] = ret[1] / scf;
	ret[2] = ret[2] / scf;
	return ret;

}

V3 V3::Normalized() {

	V3 ret(*this);
	return ret / ret.Length();

}

V3 V3::operator^(V3 v2) {

	V3 &v1 = *this;
	V3 ret(v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]);
	return ret;
}


V3 V3::operator*(float scf) {

	V3 ret(*this);
	ret[0] *= scf;
	ret[1] *= scf;
	ret[2] *= scf;
	return ret;

}

void V3::SetFromColor(unsigned int color) {

	unsigned char *rgb = (unsigned char*)&color;
	V3 &v = *this;
	v[0] = (float)(rgb[0]) / 255.0f;
	v[1] = (float)(rgb[1]) / 255.0f;
	v[2] = (float)(rgb[2]) / 255.0f;

}

unsigned int V3::GetColor() {

	unsigned int ret;
	V3 &v = *this;
	unsigned char rgb[3];
	rgb[0] = (unsigned char)(255.0f * v[0]);
	rgb[1] = (unsigned char)(255.0f * v[1]);
	rgb[2] = (unsigned char)(255.0f * v[2]);
	ret = 0xFF000000 + rgb[2] * 256 * 256 + rgb[1] * 256 + rgb[0];
	return ret;

}

V3 V3::RotateVector(V3 adir, float theta) {

	return RotatePoint(V3(0.0f, 0.0f, 0.0f), adir, theta);

}

V3 V3::RotatePoint(V3 aO, V3 adir, float theta) {


	V3 aux;
	if (fabsf(adir[0]) < fabsf(adir[1]))
		aux = V3(1.0f, 0.0f, 0.0f);
	else
		aux = V3(0.0f, 1.0f, 0.0f);

	M33 lcs;
	lcs[0] = (aux ^ adir).Normalized();
	lcs[1] = adir;
	lcs[2] = (lcs[0] ^ lcs[1]).Normalized();

	V3 &P = *this;
	V3 lP = lcs*(P-aO);

	M33 roty; roty.SetRotationAboutY(theta);
	V3 rlP = roty * lP;

	V3 rP = lcs.Inverted()*rlP + aO;
	return rP;

}

V3 V3::Light(V3 lv, V3 nv, float ka) {
	float kd = lv*nv; kd = (kd < 0.0f) ? 0.0f : kd;
	V3 &C = *this;
	return C*(ka + (1.0f - ka)*kd);
}


V3 V3::Reflect(V3 r) {

	V3 &n = *this;
	V3 rn = n*(n*r);
	V3 rr = r - rn*2.0f;
	return rr;

}


V3 V3::reflection(V3 normal_n) {
	V3 incidentRay_d = *this;
	normal_n = normal_n.Normalized();
	//	cout << *this << " " << normal_n << endl;
	V3 r(0, 0, 0);
	float d_dot_n = incidentRay_d * normal_n;
	//	cout << " ddotn:" << d_dot_n << endl;
	d_dot_n = d_dot_n * 2;
	V3 normal_n_d_dot_n = normal_n * d_dot_n;
	//cout << "normal_n * d_dot_n: " << normal_n_d_dot_n << endl;
	r = incidentRay_d - normal_n_d_dot_n;
	//cout <<"reflection: "<< r << endl;
	return r;
}

V3 V3::UnitVector() {

	return (*this) * (1.0f / Length());

}

V3 V3::RotateThisPointAboutArbitraryAxis(V3 O, V3 a, float angled) {

	// find auxiliary axis
	V3 aux;
	if (fabsf(a[0]) > fabsf(a[1])) {
		aux = V3(0.0f, 1.0f, 0.0f);
	}
	else {
		aux = V3(1.0f, 0.0f, 0.0f);
	}

	V3 a0 = (aux ^ a).UnitVector();
	V3 a2 = (a0 ^ a).UnitVector();
	M33 lcs;
	lcs[0] = a0;
	lcs[1] = a;
	lcs[2] = a2;

	V3& p = *this;
	// change to local coordinate system O, a0, a, a2
	V3 p1 = lcs * (p - O);
	// rotate about "Second axis" in local coordinate system;
	M33 mr; mr.SetRotationAboutY(angled);
	V3 p2 = mr * p1;
	V3 p3 = lcs.Inverted() * p2 + O;
	return p3;

}


V3 V3::RotateThisVectorAboutDirection(V3 a, float angled) {

	// find auxiliary axis
	V3 aux;
	if (fabsf(a[0]) > fabsf(a[1])) {
		aux = V3(0.0f, 1.0f, 0.0f);
	}
	else {
		aux = V3(1.0f, 0.0f, 0.0f);
	}

	V3 a0 = (aux ^ a).Normalized();
	V3 a2 = (a0 ^ a).Normalized();
	M33 lcs;
	lcs[0] = a0;
	lcs[1] = a;
	lcs[2] = a2;

	V3& p = *this;
	// change to local coordinate system O, a0, a, a2
	V3 p1 = lcs * p;
	// rotate about "Second axis" in local coordinate system;
	M33 mr; mr.SetRotationAboutY(angled);
	V3 p2 = mr * p1;
	V3 p3 = lcs.Inverted() * p2;
	return p3;

}