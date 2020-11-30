#pragma once
#include<vector>
#include"framebuffer.h"

class  matrix
{
public:
	int rows, cols;
	
	vector<vector<unsigned int>> imageVec;
	
	matrix(int w, int h);
	void fbPix2mat(FrameBuffer* fb);
	void mat2fbPix(FrameBuffer* fb);
	matrix split(int m, int n, int indexi, int indexj); //which part to extrat is defined by indexi and indexj;
	void copyDatafrom(matrix* m1);
};