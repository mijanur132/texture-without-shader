#pragma once
#include "matrix.h"
#include"framebuffer.h"
#include "ppc.h"
#include<vector>
class cubemap
{
public:
	int w, h;
	PPC* defaultCam;
	int facel, faceh;
	float hfov = 90.0f;
	PPC* cam0, *cam1, *cam2, *cam3, *cam4, *cam5;
	vector<matrix>cubefaces;
	cubemap(int _w, int _h);
	cubemap(FrameBuffer* fb);
	void cmface2fb(FrameBuffer* fb, int faceI);
	matrix envmap(PPC* viewCam);
	unsigned int envmap4mRay(V3 rayVec);
	unsigned int bilinearinterpolation(FrameBuffer* t1, matrix f1, float uf, float vf);
	void loadTiff2Face(char* fname, int faceI);
	void face2cubemap(matrix* temp, int w, int h, int faceI);
	cubemap create4m6Images(char* top, char* left, char* front, char* right, char* bottom, char* back);
	void printFaces();
};

