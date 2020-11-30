#include "stdafx.h"
#include "eri.h"
#include"matrix.h"
#include <cmath>
#include <iostream>


eri::eri(int _w, int _h) {

	w = _w;
	h = _h;

}

int eri::ERI2Conv(matrix& source_image_matrix, matrix& output_image_matrix, PPC* camera1)
{
	int pixelI, pixelJ = 0;

	for (int v = 0; v < camera1->h; v++)
	{
		for (int u = 0; u < camera1->w; u++)
		{

			EachPixelConv2ERI(camera1, u, v, pixelI, pixelJ);
			if (pixelI < source_image_matrix.rows && pixelJ < source_image_matrix.cols && v < output_image_matrix.rows && u < output_image_matrix.cols)
			{
				output_image_matrix.imageVec[v][u] = source_image_matrix.imageVec[pixelI][pixelJ];
				//source_image_matrix.imageVec[pixelI][pixelJ] = 0;
				//output_image_matrix.imageVec[v][u] = 0;
			}
		}
	}


	return 0;
}

int eri::EachPixelConv2ERI(PPC* camera1, int u, int v, int& pixelI, int& pixelJ)
{

	V3 p = camera1->GetUnitRay(0.5f + u, 0.5f + v);	//this focul length needs to go way	
	p = p.UnitVector();
	pixelI = Lat2PixI(GetXYZ2Latitude(p));
	pixelJ = Lon2PixJ(GetXYZ2Longitude(p));

	return 0;
}

float eri::GetXYZ2Latitude(V3 p)
{
	p = p.UnitVector();

	float lat = 90.0f - asin(p[1]) * 180.0f / PI;
	return lat;

}

float eri::GetXYZ2Longitude(V3 p)
{
	p = p.UnitVector();
	float x = p[0];
	float y = p[1];
	float z = p[2];
	float lon;
	if (x >= 0 && z >= 0) {    // use nested if
		lon = (atan(x / z)) * 180.0f / PI;
	}
	else if (x < 0 && z >= 0) {

		lon = 360.0f + (atan(x / z)) * 180.0f / PI;

	}
	else if (x >= 0 && z < 0) {
		lon = (atan(x / z)) * 180.0f / PI;
		lon = 180 + lon;
	}

	else if (x < 0 && z < 0) {
		lon = (atan(x / z)) * 180.0f / PI;
		lon = 180.0f + lon;
	}

	else {
		cout << "this not handled yet" << endl;
		exit(0);
	}
	return lon;
}


int eri::Lat2PixI(float lat)
{
	int pixI = (int)(lat * (float)h / 180.0f);
	/*
	if (pixI > h - 1)
	{
		pixI = h - 1;
	}
	else if (pixI < 0)
		pixI = 0;
		*/
	return pixI;

}

int eri::Lon2PixJ(float lon)
{
	int pixJ = (int)(lon * (float)w / 360.0f);
	pixJ = w - 1 - pixJ;
	return pixJ;
}



int eri:: ERI2Conv2x(matrix& source_image_mat, matrix& output_image_mat, PPC* camera1) {
		
	for (int i = 0; i < source_image_mat.rows; ++i)
	{
		for (int j = 0; j < source_image_mat.cols; ++j)
		{

			V3 p = Unproject(i, j);
			V3 pp;
			if (!camera1->Project(p, pp))
				continue;
			int pixelI = pp[1];
			int pixelJ = pp[0];
			if (pixelI < output_image_mat.rows && pixelJ < output_image_mat.cols)
			//if (pp[0] < camera1->w && pp[0] >= 0 && pp[1] >= 0 && pp[1] < camera1->h)
			{
				//output_image_mat.imageVec[pixelI][pixelJ] = source_image_mat.imageVec[i][j];  //pp[0]=column
				source_image_mat.imageVec[i][j] = 0;
			}
		}
		cout << "row: " << i << "     \r";
	}

	return 0;
}

V3 eri::Unproject(int i, int j) {

	// convert pixel index to angles
	float lt = GetLatitude(i);
	float lg = GetLongitude(j);

	// define 3D point B on equator with given lt
	V3 A(0.0f, 0.0f, 1.0f);
	V3 O(0.0f, 0.0f, 0.0f);
	V3 y(0.0f, 1.0f, 0.0f);
	V3 B = A.RotateThisPointAboutArbitraryAxis(O, y, lg);

	// define output 3D point by rotating B up (paning) to lat;
	V3 rotAxis = ((B - O) ^ y).UnitVector();
	V3 ret = B.RotateThisPointAboutArbitraryAxis(O, rotAxis, 90.0f - lt);

	return ret;

	// ERI pixel coordinates  of A are (h/2, 0)
	//                        of N are (0, 0), (0, 1), ... , (0, w-1)
	//                        of S are (h-1, 0), (h-1, 1), ... , (h-1, w-1)

}

float eri::GetLongitude(int j) {

	if (j < 0 || j > w - 1)
		return FLT_MAX;

	float ret = ((float)j + 0.5f) / (float)w * 360.0f;

	return ret;

}

float eri::GetLatitude(int i) {

	if (i < 0 || i > h - 1)
		return FLT_MAX;

	float ret = ((float)i + 0.5f) / (float)h * 180.0f;

	return ret;


}
