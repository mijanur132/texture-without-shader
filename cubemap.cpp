#include "stdafx.h"
#include "cubemap.h"
#include"matrix.h"
#include"framebuffer.h"

cubemap::cubemap(int _w, int _h)
{
	w = _w;
	h = _h;	
	facel = w / 3;
	faceh = h / 4;
	defaultCam =new PPC(hfov, w/3, h / 4);
	cam0=new PPC(hfov, w / 3, h / 4);
	cam1 = new PPC(hfov, w / 3, h / 4);
	cam2 = new PPC(hfov, w / 3, h / 4);
	cam3 = new PPC(hfov, w / 3, h / 4);
	cam4 = new PPC(hfov, w / 3, h / 4);
	cam5 = new PPC(hfov, w / 3, h / 4);


	cam0->Tilt(90.0f);
	cam4->Tilt(-90.0f);
	cam1->PanLeftRight(90.0f);
	cam3->PanLeftRight(-90.0f);
	cam4->PanLeftRight(180.0f);
	for (int i = 0; i < 6; i++)
	{	

		matrix face(w/3,h/4);
		cubefaces.push_back(face);
		
	}

	
}

cubemap cubemap::create4m6Images(char* top, char* left, char* front, char* right, char* bottom, char* back)
{
	FrameBuffer* fb0 = new FrameBuffer(0, 0, 100, 100, 0);
	fb0->LoadTiff(top);
	int _w=fb0->w;
	int _h =fb0->h;
	w = _w* 3;
	h = _h * 4;
	cubemap cm(w, h);
	cm.loadTiff2Face(top, 0);
	cm.loadTiff2Face(left, 1);
	cm.loadTiff2Face(front, 2);
	cm.loadTiff2Face(right, 3);
	cm.loadTiff2Face(bottom, 4);
	cm.loadTiff2Face(back, 5);

	return cm;
}

void cubemap::loadTiff2Face(char* fname, int faceI)
{
	FrameBuffer* fb0 = new FrameBuffer(0, 0, 100, 100, 0);
	fb0->LoadTiff(fname);
	matrix temp(fb0->w, fb0->h);
	temp.fbPix2mat(fb0);
	cubefaces[faceI].imageVec = temp.imageVec;
}

void cubemap::printFaces()
{
	matrix temp(w, h);
	face2cubemap(&temp, w / 3, 0, 0);
	face2cubemap(&temp, 0, h/4, 1);
	face2cubemap(&temp, w/3, h / 4, 2);
	face2cubemap(&temp, 2*w/3, h / 4, 3);
	face2cubemap(&temp, w / 3, 2*h / 4, 4);
	face2cubemap(&temp, w / 3, 3 * h / 4, 5);
	FrameBuffer* fb0 = new FrameBuffer(0, 0, w, h, 0);
	temp.mat2fbPix(fb0);
	fb0->SaveAsTiff("CubemapGenerated.tiff");
}



void cubemap::face2cubemap(matrix* temp,int startw, int starth, int faceI)
{
	
	for (int ih = 0; ih < faceh; ih++)
	{
		for (int iw = 0; iw < facel; iw++)
		{
			temp->imageVec[starth+ih][startw+iw] = cubefaces[faceI].imageVec[ih][iw];
		}

	}
}

cubemap::cubemap(FrameBuffer* fb)
{
	w = fb->w;
	h = fb->h;
	facel = w / 3;
	faceh = h / 4;
	defaultCam = new PPC(hfov, w / 3, h / 4);
	cam0 = new PPC(hfov, w / 3, h / 4);
	cam1 = new PPC(hfov, w / 3, h / 4);
	cam2 = new PPC(hfov, w / 3, h / 4);
	cam3 = new PPC(hfov, w / 3, h / 4);
	cam4 = new PPC(hfov, w / 3, h / 4);
	cam5 = new PPC(hfov, w / 3, h / 4);




	cam0->Tilt(90.0f);
	cam4->Tilt(-90.0f);
	cam1->PanLeftRight(90.0f);
	cam3->PanLeftRight(-90.0f);
	cam5->Tilt(180.0f);
	matrix temp(w, h);
	temp.fbPix2mat(fb);
	int index = 0;
	int index2 = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (index==1 || index==3|| index==4 || index==5 || index==7 || index==10)
			{
				
				matrix face = temp.split(4, 3, i, j);
				/*
				if (index2 == 5) {
				
					matrix reartemp(facel, faceh);
					reartemp.copyDatafrom(&face);
					for (int ih = 0; ih < faceh; ih++)
					{
						face.imageVec[ih] = reartemp.imageVec[faceh-1 - ih];
						
					}

				
				} */
				cubefaces.push_back(face);
				
				index2++;
			}
			index++;
			
		}

	}	


}
void cubemap::cmface2fb(FrameBuffer* fb, int faceI) {

	cubefaces[faceI].mat2fbPix(fb);

}


unsigned int cubemap::bilinearinterpolation(FrameBuffer* t1, matrix f1, float uf, float vf) //u->col
{
	V3 colorBilinear;

	if (vf < (t1->h - 1.5) && vf> 0.5 && uf > 0.5 && uf < (t1->w - 1.5))
	{
		float u0 = (uf - 0.5f);
		float v0 = (vf - 0.5f);
		int u0f = floor(u0);
		int v0f = floor(v0);

		float dx = u0 - u0f;
		float dy = v0 - v0f;

		V3 c0, c1, c2, c3;
		int i0, i1, i2, i3;
		float a0, a1, a2, a3;
		

		//i0 = t1->getPixelIndex(u0f, v0f);  //cl,rw
		//i1 = t1->getPixelIndex(u0f + 1, v0f);
		//i2 = t1->getPixelIndex(u0f + 1, v0f + 1);
		//i3 = t1->getPixelIndex(u0f, v0f + 1);
		//cout << "u0f, v0f, i0, i1, i2, i3:" << u0f<<" " << v0f<<" " << i0 <<" "<< i1 <<" "<<i2<<" "<<i3<< endl;
		c0.SetFromColor(f1.imageVec[v0f][u0f]);  //imagevec->row, col
		c1.SetFromColor(f1.imageVec[v0f][u0f+1]);
		c2.SetFromColor(f1.imageVec[v0f+1][u0f+1]);
		c3.SetFromColor(f1.imageVec[v0f+1][u0f]);
		//c1.SetFromColor(t1->pix[i1]);
		//c2.SetFromColor(t1->pix[i2]);
		//c3.SetFromColor(t1->pix[i3]);
		//cout << c0<<" " << c1<<" " << c2<<" " << c3 << endl;

		a0 = (1 - dx) * (1 - dy);
		a1 = dx * (1 - dy);
		a2 = dx * dy;
		a3 = (1 - dx) * dy;
		//a0 = a1 = a2 = a3 = 1;
		//cout << a0 << " " << a1<<"  "<<a2<<" "<<a3<<endl;
		for (int i = 0; i < 3; i++)
		{
			colorBilinear[i] = (a0 * c0[i] + a2 * c2[i] + a1 * c1[i] + a3 * c3[i]);

		}

		return colorBilinear.GetColor();

	}
	else
	{
		
		if (vf < (t1->h) && vf>= 0 && uf >= 0 && uf < (t1->w))
		{
			
			return f1.imageVec[vf][uf];
			
		}
		
		

	}

}

matrix cubemap::envmap(PPC* viewCam) {
	matrix temp(viewCam->w, viewCam->h);
	FrameBuffer* fbtemp = new FrameBuffer(0, 0, w/3, h/4, 0);
	
	for (int ih = 0; ih < viewCam->h; ih++)
	{
		for (int iw = 0; iw < viewCam->w; iw++)
		{
			V3 rayVec=viewCam->GetRayVector(iw, ih);
			V3 pp = cam0->lookAtRayVecDir(rayVec);	
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < facel&& pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw]=bilinearinterpolation(fbtemp, cubefaces[0], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[0].imageVec[pp[1]][pp[0]];
				continue;
			}
					
			
			pp = cam1->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < facel && pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw] = bilinearinterpolation(fbtemp, cubefaces[1], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[1].imageVec[pp[1]][pp[0]];
				continue;
			}

			
			pp = cam2->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < facel && pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw] = bilinearinterpolation(fbtemp, cubefaces[2], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[2].imageVec[pp[1]][pp[0]];
				continue;
			}
			
			pp = cam3->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < facel && pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw] = bilinearinterpolation(fbtemp, cubefaces[3], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[3].imageVec[pp[1]][pp[0]];
				continue;
			}
			
			pp = cam4->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < facel && pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw] = bilinearinterpolation(fbtemp, cubefaces[4], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[4].imageVec[pp[1]][pp[0]];
				continue;
			}
			
			pp = cam5->lookAtRayVecDir(rayVec);
			if (pp[0] >= 0 && pp[1] >= 0 && pp[0] < facel && pp[1] < faceh)  //pp0-x,pp1-y
			{
				//temp.imageVec[ih][iw] = bilinearinterpolation(fbtemp, cubefaces[5], pp[0], pp[1]);
				temp.imageVec[ih][iw] = cubefaces[5].imageVec[pp[1]][pp[0]];				
			}

		}

	}

	return temp;

}
unsigned int cubemap::envmap4mRay(V3 rayVec) {
	

	unsigned int temp;
			
			V3 pp = cam0->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[0].imageVec[pp[1]][pp[0]];				
			}


			pp = cam1->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[1].imageVec[pp[1]][pp[0]];
				
			}


			pp = cam2->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[2].imageVec[pp[1]][pp[0]];
			
			}

			pp = cam3->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[3].imageVec[pp[1]][pp[0]];
				
			}

			pp = cam4->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[4].imageVec[pp[1]][pp[0]];
				
			}

			pp = cam5->lookAtRayVecDir(rayVec);
			if (pp[0] > 0 && pp[1] > 0 && pp[0] < faceh && pp[1] < facel)
			{
				temp = cubefaces[5].imageVec[pp[1]][pp[0]];
			}

			return temp;



}
