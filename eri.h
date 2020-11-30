#pragma once
#include"stdafx.h"
#include "v3.h"
#include "ppc.h"
#include"matrix.h"


class eri
{
public:
	int w, h;
	float PI = 3.1416;
	float fps;
	int framesN;

	eri(int _w, int _h);	
	int ERI2Conv(matrix& source_image_mat, matrix& output_image_mat, PPC* camera1);
	int ERI2Conv2x(matrix& source_image_matrix, matrix& output_image_matrix, PPC* camera1);
	int EachPixelConv2ERI(PPC* camera1, int u, int v, int& pixelI, int& pixelJ);
	float GetXYZ2Latitude(V3 p);
	float GetXYZ2Longitude(V3 p);
	int Lat2PixI(float lat);
	int Lon2PixJ(float lon);
	float GetLongitude(int j);
	float GetLatitude(int i);
	V3 Unproject(int i, int j);
};

