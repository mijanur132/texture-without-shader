#pragma once
#include <tiffio.h>




class texture
{
public:
	unsigned int* pix; // pixel array
	int w, h,id;
	void init();	
	void init(int w, int h);
	void LoadTiff(char* fname);
	int getPixelIndex(int u, int v);
	void Set(int u, int v, unsigned int color);

};

