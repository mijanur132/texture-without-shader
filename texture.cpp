#include "stdafx.h"
#include "texture.h"
#include "math.h"
#include <iostream>
#include "scene.h"




// load ************************************a tiff image to pixel buffer



void texture::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	w = width;
	h = height;
	pix = new unsigned int[w * h];
	

	if (TIFFReadRGBAImageOriented(in, w, h, pix, ORIENTATION_LEFTTOP, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}
	cout << "imageLoaded-w,h:" << w <<" "<< h << endl;
	TIFFClose(in);
	
}

int texture::getPixelIndex(int u, int v)
{
	if (u >= w) {
	//	cout << u << endl;
		u = u % w;
		//cout << "new U:" << u << endl;
	}
	if (v >= h)
	{
	//	cout << v << endl;
		v = v % h;
	//	cout << "new U:" << v << endl;
	}

	int id = u + v * w;
	return id;

}

void texture::init()
{

	w = 500;
	h = 500;
	pix = new unsigned int[w * h];
	V3 C = V3(0, 0, 1);
	for (int uv = 0; uv < w * h; uv++)
		pix[uv] = C.GetColor();	

}


void texture::init(int _w, int _h)
{
	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
	V3 C = V3(0, 0, 1);
	for (int uv = 0; uv < w * h; uv++)
		pix[uv] = 0;

}

