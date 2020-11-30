#include "stdafx.h"

#include "ppc.h"

#include "M33.h"

#include <iostream>

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.14159236f;
	c = V3(-(float)w / 2.0f, (float)h / 2, -(float)w / (2 * tanf(hfovd / 2.0f)));

}

int PPC::Project(V3 P, V3 &p) {

	M33 M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	V3 q = M.Inverted()*(P - C);
	float w = q[2];
	if (w <= 0.0f)
		return 0;

	p[0] = q[0] / q[2];
	p[1] = q[1] / q[2];
	p[2] = 1.0f / w;
	return 1;
}

V3 PPC::UnProject(V3 p) {

	V3 ret;
	ret = C + (a*p[0] + b*p[1] + c)/p[2];
	return ret;

}


void PPC::TranslateRightLeft(float tstep) {

	V3 rightDir = a.Normalized();
	C = C + rightDir*tstep;

}

void PPC::TranslateFrontBack(float tstep) {

	V3 tDir = (a^b).Normalized();
	C = C + tDir*tstep;

}


void PPC::PanLeftRight(float rstep) {

	V3 adir = b.Normalized()*-1.0f;
	a = a.RotateVector(adir, rstep);
	c = c.RotateVector(adir, rstep);

}


void PPC::TiltUpDown(float rstep) {

	V3 adir = a.Normalized();
	b = b.RotateVector(adir, rstep);
	c = c.RotateVector(adir, rstep);

}

void PPC::SetPose(V3 newC, V3 lookAtPoint, V3 upGuidance) {

	V3 newvd = (lookAtPoint - newC).Normalized();
	V3 newa = (newvd ^ upGuidance).Normalized();
	V3 newb = (newvd^newa).Normalized();
	float focalLength = (a^b).Normalized()*c;
	V3 newc = newvd*focalLength - newa*(float)w / 2.0f - newb*(float)h / 2.0f;


	// commit changes to camera
	a = newa;
	b = newb;
	c = newc;
	C = newC;

}

void PPC::Interpolate(PPC *ppc0, PPC *ppc1, int i, int n) {

	cerr << "INFO: not yet implemented" << endl;
	PPC &ppc = *this;
	float scf = (float)i / (float)(n - 1);
	ppc.C = ppc0->C + (ppc1->C - ppc0->C)*scf;
	// build lookatpoint and upguidance by interpolation, call setpose to create interpolated camera

}

// vf determines how far the image plane is in the visualization
void PPC::Visualize(FrameBuffer *vfb, PPC *vppc, float vf) {

	unsigned int color = 0xFF000000;
	V3 c1; c1.SetFromColor(color);
	vfb->Draw3DPoint(C, vppc, color, 7);

	V3 imageCorners[4];
	float _w = GetF() / vf;
	imageCorners[0] = UnProject(V3(0.5f, 0.5f, _w));
	imageCorners[1] = UnProject(V3(0.5f, -0.5f + (float)h, _w));
	imageCorners[2] = UnProject(V3(-0.5f + (float)w, -0.5f + (float)h, _w));
	imageCorners[3] = UnProject(V3(-0.5f + (float)w, 0.5f, _w));
	V3 c0; c0.SetFromColor(0xFF0000FF);
	for (int i = 0; i < 4; i++) {
		vfb->Draw3DSegment(imageCorners[i], imageCorners[(i + 1) % 4], vppc, c0, c0);
	}
	vfb->Draw3DSegment(C, imageCorners[0], vppc, c1, c0);

}

float PPC::GetF() {

	return c * (a^b).Normalized();

}

void PPC::Visualize(FrameBuffer *vfb, PPC *vppc, float vf, FrameBuffer *fb) {

	float f = GetF();
	for (int v = 0; v < fb->h; v++) {
		for (int u = 0; u < fb->w; u++) {
			if (fb->GetZ(u, v) == 0.0f)
				continue;
			float _w = f / vf;
			V3 pix3D = UnProject(V3(.5f + (float)u, .5f+ (float)v, _w));
			vfb->Draw3DPoint(pix3D, vppc, fb->Get(u, v), 1);
		}
	}

}


void PPC::Visualize(FrameBuffer *vfb, PPC *vppc, FrameBuffer *fb) {

	for (int v = 0; v < fb->h; v++) {
		for (int u = 0; u < fb->w; u++) {
			float currz = fb->GetZ(u, v);
			if (currz == 0.0f)
				continue;
			V3 pix3D = UnProject(V3(.5f + (float)u, .5f + (float)v, currz));
			vfb->Draw3DPoint(pix3D, vppc, fb->Get(u, v), 1);
		}
	}

}


void PPC::SetInterpolated(PPC *ppc0, PPC *ppc1, int stepsN, int stepi) {

	float intf = (float)stepi / (float)(stepsN - 1);
	w = ppc0->w;
	h = ppc0->h;
	V3 newC = ppc0->C + (ppc1->C - ppc0->C)*intf;
	V3 vd0 = (ppc0->a^ppc0->b).Normalized();
	V3 vd1 = (ppc1->a^ppc1->b).Normalized();
	V3 vd = vd0 + (vd1 - vd0)*intf;
	vd = vd.Normalized();
	V3 qup = ppc0->b + (ppc1->b - ppc0->b)*intf; qup = qup *-1.0f; qup = qup.Normalized();
	*this = *ppc0;
	SetPose(newC, newC + vd, qup);

	float f0 = ppc0->GetF();
	float f1 = ppc1->GetF();
	float f = f0 + (f1 - f0)*intf;
	ScaleFocalLength(f / ppc0->GetF());

}

void PPC::ScaleFocalLength(float flscf) {
	
	V3 vd = (a^b).Normalized();
	float f = c*vd;
	f *= flscf;
	c = vd*f - a*(float)w / 2.0f - b*(float)h / 2.0f;
}


M33 PPC::CameraMatrix() {

	M33 ret;
	ret.SetColumn(0, a);
	ret.SetColumn(1, b);
	ret.SetColumn(2, c);
	return ret;

}

void PPC::SetIntrinsicsHW() {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float f = GetF();
	float hither = 0.25f;
	float yon = 1000.0f;
	float scalef = hither / f;
	float wf = a.Length() * w;
	float hf = b.Length() * h;
	glFrustum(-wf / 2.0f*scalef, wf / 2.0f*scalef, -hf / 2.0f*scalef, 
		hf / 2.0f*scalef, hither, yon);
	glMatrixMode(GL_MODELVIEW); // default matrix mode
}

void PPC::SetIntrinsicsHWgpu() {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float f = GetF();
	float hither = 0.25f;
	float yon = 1000.0f;
	float scalef = hither / f;
	float wf = a.Length() * w;
	float hf = b.Length() * h;
	glFrustum(-wf / 2.0f * scalef, wf / 2.0f * scalef, -hf / 2.0f * scalef,
		hf / 2.0f * scalef, hither, yon);
	glMatrixMode(GL_MODELVIEW); // default matrix mode
}

void PPC::SetExtrinsicsHW(V3 eye, V3 look, V3 down) {
	//V3 eye, look, down;
	//eye = C;
	//look = C + (a^b)*100.0f;
	//down = b.Normalized();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], 
		look[0], look[1], look[2], 
		-down[0], -down[1], -down[2]);
}

void PPC::SetExtrinsicsHW() {
	V3 eye, look, down;
	eye = C;
	look = C + (a^b)*100.0f;
	down = b.Normalized();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		look[0], look[1], look[2],
		-down[0], -down[1], -down[2]);
}



void PPC::Roll(float angled) {
	V3 dv = GetVD().Normalized();
	a = a.RotateThisVectorAboutDirection(dv, angled);
	b = b.RotateThisVectorAboutDirection(dv, angled);
	c = c.RotateThisVectorAboutDirection(dv, angled);
}
V3 PPC::GetVD() {

	return (a ^ b).Normalized();

}

V3 PPC::GetUnitRay(float uf, float vf) {

	V3 ray = a * uf + b * vf + c;
	return ray.UnitVector();
}

V3 PPC::GetRayVector(float uf, float vf)
{
	V3 sceneP(uf, vf, 1);
	V3 rayVec = UnProject(sceneP) - C;
	return rayVec;
}

V3 PPC::lookAtRayVecDir(V3 rayVec)
{
	V3 envP(0, 0, 0);
	V3 newRayVec = C + rayVec;
	Project(newRayVec, envP);
	return envP;
}

void PPC::Tilt(float angled) {


	V3 dv = a.UnitVector();
	b = b.RotateThisVectorAboutDirection(dv, angled);
	c = c.RotateThisVectorAboutDirection(dv, angled);


}

void PPC::printPPC() {

	cout << "C:" << c << " a:" << a << " b:" << b << " c:" << c << " vd:" << GetVD() << " w:" << w << " h:" << h << endl;

}
