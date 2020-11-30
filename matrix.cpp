#include "stdafx.h"
#include<vector>
#include<iostream>
#include "framebuffer.h"
#include "matrix.h"
using namespace std;



matrix::matrix(int col, int row) {	

	rows = row;
	cols = col;
	for (int i = 0; i <rows ; i++)
	{
		vector <unsigned int> imageVecRow;
		
		for (int j = 0; j < cols; j++)
		{
			unsigned int temp = 0;
			imageVecRow.push_back(temp);
		}
		imageVec.push_back(imageVecRow);
	}
}

void matrix::fbPix2mat( FrameBuffer* fb)
{

	for (int v = 0; v < rows; v++) 
	{
		for (int u = 0; u < cols; u++) 
		{
			imageVec[v][u]= fb->Get(u, v);
			
		}
	}

}

void matrix::mat2fbPix(FrameBuffer* fb)
{

	for (int v = 0; v < rows; v++)
	{
		for (int u = 0; u < cols; u++)
		{
			fb->Set(u, v, imageVec[v][u]);

		}
	}

}

matrix matrix::split(int rown, int coln, int indexh,int indexw) {
	
	int ht = rows / rown;
	int wt = cols / coln;
	matrix temp( wt, ht);
	int startw = indexw * wt;
	int endw = (indexw + 1) * wt;

	int starth = indexh * ht;
	int endh = (indexh+ 1) * ht;

	for (int i = starth; i < endh; i++)
	{
		for (int j = startw; j < endw; j++)
		{
			temp.imageVec[i - starth][j - startw] = imageVec[i][j];

		}

	}

	return temp;
}

void matrix::copyDatafrom(matrix* m1) {

	for (int i =0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i < m1->rows && j < m1->cols) 
			{
				imageVec[i][j] = m1->imageVec[i][j];
			}

		}

	}

}