#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"
#include "CGInterface.h"


class Scene {
public:

	CGInterface * cgi;
	ShaderOneInterface *soi;
	int envReflec = 0; //0 not reflectionBackground
	GUI *gui;
	GLuint textureID[2],texs;
	FrameBuffer *fb, *fb3, *hwfb, *gpufb, *fbEnv,*fb0,*fb1;
	PPC *ppc, *ppcBB,*ppc3,*gpuppc;
	TMesh *tmeshes;
	int tmeshesN;
	float qs;
	float qtexCord[8]={ 0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0 };
	int hwPerSessionInit;
	Scene();
	void DBG();
	void NewButton();
	void Render();
	void Render(FrameBuffer *rfb, PPC *rppc); // projection followed by rasterization
	void RenderHW();
	void RenderHWBB();
	void RenderGPU();
	void RenderRayTracing(FrameBuffer *rfb, PPC *rppc); // ray tracing
	int RayTrace(V3 rO, V3 rdir, int rayOrder, V3& rc, float &currz);
	float vf; // ppc visualization focal length
	V3 L; // point light source
	float ka; // ambient lighting coefficient
	float p1; // test parameter for fragment shader
	void PerSessionHWInit();


	float morphFraction; // morphing parameters
	void LoadResources();
	V3 midpoint;
};

extern Scene *scene;