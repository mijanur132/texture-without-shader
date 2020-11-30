#include "stdafx.h"

#include <fstream>
#include <iostream>
//#include "stb_image_write.h"
#include "M33.h"
#include "TMesh.h"
#include "AABB.h"
#include "vector"
#include "scene.h"
//#include "SOIL.h"

using namespace std;


void TMesh::Allocate(int _vertsN, int _trisN) {

	vertsN = _vertsN;
	trisN = _trisN;
	verts = new V3[vertsN];
	colors = new V3[vertsN];
	normals = new V3[vertsN];
	tris = new unsigned int[trisN * 3];
}

void TMesh::SetToCube(V3 cc, float sideLength, unsigned int color0, unsigned int color1) {

	vertsN = 8;
	trisN = 6 * 2;
	Allocate(vertsN, trisN);

	for (int vi = 0; vi < 4; vi++) {
		colors[vi].SetFromColor(color0);
		colors[vi+4].SetFromColor(color1);
	}

	int vi = 0;
	verts[vi] = cc + V3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + V3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + V3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + V3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;

	verts[vi] = verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;

	int tri = 0;
	tris[3 * tri + 0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 7;
	tris[3 * tri + 1] = 6;
	tris[3 * tri + 2] = 5;
	tri++;
	tris[3 * tri + 0] = 5;
	tris[3 * tri + 1] = 4;
	tris[3 * tri + 2] = 7;
	tri++;
	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 1;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 4;
	tri++;
	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 4;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 1;
	tri++;

}

void TMesh::DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color) {

	V3 c0;
	c0.SetFromColor(color);
	for (int si = 0; si < 4; si++) {
		fb->Draw3DSegment(verts[si], verts[(si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[4+si], verts[4 + (si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[si], verts[si + 4], ppc, c0, c0);
	}

}

void TMesh::DrawWireFrame(FrameBuffer *fb, PPC *ppc, unsigned int color) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 V0 = verts[tris[3 * tri + 0]];
		V3 V1 = verts[tris[3 * tri + 1]];
		V3 V2 = verts[tris[3 * tri + 2]];
		V3 c0 = colors[tris[3 * tri + 0]];
		V3 c1 = colors[tris[3 * tri + 1]];
		V3 c2 = colors[tris[3 * tri + 2]];
		fb->Draw3DSegment(V0, V1, ppc, c0, c1);
		fb->Draw3DSegment(V1, V2, ppc, c1, c2);
		fb->Draw3DSegment(V2, V0, ppc, c2, c0);
	}

}

void TMesh::LoadBin(char *fname) {

	ifstream ifs(fname, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}

	ifs.read((char*)&vertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (verts)
		delete verts;
	verts = new V3[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
	if (colors)
		delete colors;
	colors = 0;
	if (yn == 'y') {
		colors = new V3[vertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete normals;
	normals = 0;
	if (yn == 'y') {
		normals = new V3[vertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertsN * 2];
	}

	ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

	if (colors) {
		ifs.read((char*)colors, vertsN * 3 * sizeof(float)); // load cols
	}

	if (normals)
		ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}

V3 TMesh::GetCenter() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++) {
		ret = ret + verts[vi];
	}
	ret = ret / (float)vertsN;
	return ret;

}

void TMesh::Translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}

}

void TMesh::SetCenter(V3 center) {

	V3 currCenter = GetCenter();
	Translate(center - currCenter);

}



void TMesh::Rotate(V3 aO, V3 aDir, float theta) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi].RotatePoint(aO, aDir, theta);
	}

}

void TMesh::RenderFilled(FrameBuffer *fb, PPC *ppc, V3 C, V3 L, float ka) {

	V3 *pverts = new V3[vertsN];
	for (int vi = 0; vi < vertsN; vi++) {
		if (!ppc->Project(verts[vi], pverts[vi]))
			pverts[vi] = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	for (int tri = 0; tri < trisN; tri++) {
		unsigned int vinds[3] = { tris[3 * tri + 0], tris[3 * tri + 1], tris[3 * tri + 2]};
		if (
			pverts[vinds[0]][0] == FLT_MAX ||
			pverts[vinds[1]][0] == FLT_MAX ||
			pverts[vinds[2]][0] == FLT_MAX
			)
			continue;

		AABB aabb(pverts[vinds[0]]);
		aabb.AddPoint(pverts[vinds[1]]);
		aabb.AddPoint(pverts[vinds[2]]);
		// clipping
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;
		
		int left = (int)(aabb.corners[0][0] + .5f);
		int right = (int)(aabb.corners[1][0] - .5f);
		int top = (int)(aabb.corners[0][1] + .5f);
		int bottom = (int)(aabb.corners[1][1] - .5f);

		M33 eeqsm = SetEEQs(pverts[vinds[0]], pverts[vinds[1]], pverts[vinds[2]]);
		M33 ssim = SetSSIM(pverts[vinds[0]], pverts[vinds[1]], pverts[vinds[2]]);
		M33 msimQ;
		msimQ.SetColumn(0, verts[vinds[0]] - ppc->C);
		msimQ.SetColumn(1, verts[vinds[1]] - ppc->C);
		msimQ.SetColumn(2, verts[vinds[2]] - ppc->C);
		msimQ = msimQ.Inverted()*ppc->CameraMatrix();
		V3 msiDen = msimQ[0] + msimQ[1] + msimQ[2];

		V3 zv(pverts[vinds[0]][2], pverts[vinds[1]][2], pverts[vinds[2]][2]);
		V3 zLE = ssim * zv;
		M33 cm;
		cm.SetColumn(0, colors[vinds[0]]);
		cm.SetColumn(1, colors[vinds[1]]);
		cm.SetColumn(2, colors[vinds[2]]);
		M33 cLEm;
		cLEm[0] = ssim * cm[0];
		cLEm[1] = ssim * cm[1];
		cLEm[2] = ssim * cm[2];
		M33 msiRGB;
		msiRGB[0] = msimQ.Transposed()*cm[0];
		msiRGB[1] = msimQ.Transposed()*cm[1];
		msiRGB[2] = msimQ.Transposed()*cm[2];
		V3 msiZ = msimQ.Transposed()*zv;

		M33 nm;
		nm.SetColumn(0, normals[vinds[0]]);
		nm.SetColumn(1, normals[vinds[1]]);
		nm.SetColumn(2, normals[vinds[2]]);
		M33 nLEm;
		nLEm[0] = ssim * nm[0];
		nLEm[1] = ssim * nm[1];
		nLEm[2] = ssim * nm[2];


		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 currPix(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 sid = eeqsm * currPix;
				if (sid[0] < 0.0f || sid[1] < 0.0f || sid[2] < 0.0f)
					continue; // outside of triangle
				float currz = zLE * currPix; // correct SSI of 1/w
				float currMsiD = msiDen * currPix;

//				currz = msiZ * currPix / currMsiD; // incorrect MSI of 1/w

				if (fb->Farther(u, v, currz))
					continue; // hidden

				V3 currColor = cLEm*currPix; // screen space interpolation
				if (msiFlag) {
					currColor = (msiRGB*currPix) / currMsiD; // model space interpolation
					if (texture) {
						float s = currColor[0];
						float t = currColor[1];
						int tu = (int)(s*(float)texture->w);
						int tv = (int)(t*(float)texture->h);
						currColor.SetFromColor(texture->Get(tu, tv));
					}
				}
				fb->Set(u, v, currColor.GetColor());
				continue;

				// lighting
				// normal at current pixel
				V3 currNormal = nLEm*currPix;
				V3 nv = currNormal.Normalized();

				// 3D surface point at current pixel
				V3 currP = ppc->UnProject(V3(currPix[0], currPix[1], currz));

				// light vector
				V3 lv = (L - currP).Normalized();
				currColor = C.Light(lv, nv, ka);
				fb->Set(u, v, currColor.GetColor());
			}
		}

	}

	delete []pverts;

}

V3 TMesh::SetEEQ(V3 v0, V3 v1, V3 v2) {

#if 0

	(x - x0) / (x1 - x0) = (y - y0) / (y1 - y0)
	(x - x0)*(y1 - y0) = (y - y0)*(x1 - x0);
	x*(y1 - y0) + y * (x0 - x1) + x0(y0 - y1) + y0(x1 - x0) = 0
	xA + yB + C = 0
#endif

	float x0, x1, y0, y1;
	x0 = v0[0];
	x1 = v1[0];
	y0 = v0[1];
	y1 = v1[1];
	V3 ret;
	ret[0] = y1 - y0;
	ret[1] = x0 - x1;
//	ret[2] = x0*(y0 - y1) + y0*(x1 - x0);
	ret[2] = -x0*y1 + y0*x1;
	V3 v2p(v2); v2p[2] = 1.0f;

	if (ret*v2p < 0.0f) // ret * v2p = Au+Bv+C
		ret = ret * -1.0f;

	return ret;

}

M33 TMesh::SetEEQs(V3 pv0, V3 pv1, V3 pv2) {

	M33 ret;
	V3 pvs[3];
	pvs[0] = pv0;
	pvs[1] = pv1;
	pvs[2] = pv2;
	for (int ei = 0; ei < 3; ei++) {
		ret[ei] = SetEEQ(pvs[ei], pvs[(ei + 1) % 3], pvs[(ei + 2) % 3]);
	}
	return ret;
}

// LE(u, v) = Au + Bv + c;

// LE(u + 1, v) = LE(u, v) + A;

M33 TMesh::SetSSIM(V3 pv0, V3 pv1, V3 pv2) {

	M33 ret;
	ret[0] = pv0;
	ret[1] = pv1;
	ret[2] = pv2;
	ret.SetColumn(2, V3(1.0f, 1.0f, 1.0f));
	return ret.Inverted();

}

void TMesh::Light(V3 C, V3 L, float ka) {

	for (int vi = 0; vi < vertsN; vi++) {
		V3 lv = (L - verts[vi]).Normalized();
		V3 nv = normals[vi].Normalized();
		colors[vi] = C.Light(lv, nv, ka);
	}

}

void TMesh::SetQuad(V3 *qverts, V3 *qcolors) {

	Allocate(4, 2);
	V3 normal = ((qverts[1] - qverts[0]) ^ (qverts[2] - qverts[0])).Normalized();
	for (int vi = 0; vi < 4; vi++) {
		verts[vi] = qverts[vi];
		colors[vi] = qcolors[vi];
		normals[vi] = normal;
	}


	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
	tris[3] = 2;
	tris[4] = 3;
	tris[5] = 0;

}


void TMesh::RayTrace(FrameBuffer *fb, PPC *ppc) {

	for (int v = 0; v < fb->h; v++) {
		for (int u = 0; u < fb->w; u++) {
			V3 dir = ppc->a*(.5f + (float)u) + ppc->b*(.5f + (float)v) + ppc->c;
			dir = dir.Normalized();
			V3 O = ppc->C;
			for (int tri = 0; tri < trisN; tri++) {
				V3 v0 = verts[tris[3 * tri + 0]];
				V3 v1 = verts[tris[3 * tri + 1]];
				V3 v2 = verts[tris[3 * tri + 2]];
				V3 barycc;
				float t;
				if (!IntersectTriangleWithRay(v0, v1, v2, O, dir, t, barycc))
					continue;
				if (fb->Farther(u, v, 1.0f / t))
					continue;
				M33 triColorM;
				triColorM.SetColumn(0, colors[tris[3 * tri + 0]]);
				triColorM.SetColumn(1, colors[tris[3 * tri + 1]]);
				triColorM.SetColumn(2, colors[tris[3 * tri + 2]]);
				V3 intColor = triColorM*barycc;
				fb->Set(u, v, intColor.GetColor());
			}
		}
		fb->redraw();
		Fl::check();
	}
}

int TMesh::IntersectTriangleWithRay(V3 v0, V3 v1, V3 v2, V3 O, V3 dir, float &t, 
	V3 &barycc) {

	M33 tM;
	tM.SetColumn(0, v0);
	tM.SetColumn(1, v1);
	tM.SetColumn(2, v2);
	tM = tM.Inverted();
	V3 q = tM*O;
	V3 r = tM*dir;
	cout << "tM0"<<tM[0] << tM[1] << tM[2] << endl;
	cout << "dir o" << dir <<O<< endl;
	cout << "q" << q << endl;
	cout << "R" << r << endl;
	t = (1.0f - (q[0] + q[1] + q[2])) / (r[0] + r[1] + r[2]);
	if (t < 0.0f)
		return 0;
	barycc = q + r*t;
	cout << "T" << t << endl;
	cout << "barycc:" << barycc << endl;
	if (barycc[0] < 0.0f || barycc[1] < 0.0f || barycc[2] < 0.0f)
		return 0;
	return 1;

}


int TMesh::RayTrace(V3 rO, V3 rdir, V3 &rc, float &rz, V3 &rrO, V3 &rrdir) {

	rz = 0.0f;
	for (int tri = 0; tri < trisN; tri++) {
		float currz;
		V3 V0 = verts[tris[3 * tri + 0]];
		V3 V1 = verts[tris[3 * tri + 1]];
		V3 V2 = verts[tris[3 * tri + 2]];
		V3 abc;
		if (!RayTriangleIntersection(rO, rdir, V0, V1, V2, abc, currz))
			continue;
		if (currz > rz) {
			rz = currz;
			V3 c0 = colors[tris[3 * tri + 0]];
			V3 c1 = colors[tris[3 * tri + 1]];
			V3 c2 = colors[tris[3 * tri + 2]];
			M33 cM;
			cM.SetColumn(0, c0);
			cM.SetColumn(1, c1);
			cM.SetColumn(2, c2);
			rc = cM*abc;
			if (texture) {
				int u = (int)(rc[0] * (float)texture->w);
				int v = (int)(rc[1] * (float)texture->h);
				rc.SetFromColor(texture->Get(u, v));
			}
			// for reflective meshes, compute reflected ray
			if (reflectorFlag) {
				M33 vM;
				vM.SetColumn(0, V0);
				vM.SetColumn(1, V1);
				vM.SetColumn(2, V2);
				rrO = vM*abc;
				M33 nM;
				nM.SetColumn(0, normals[tris[3 * tri + 0]]);
				nM.SetColumn(1, normals[tris[3 * tri + 1]]);
				nM.SetColumn(2, normals[tris[3 * tri + 2]]);
				V3 currNormal = (nM*abc).Normalized();
				rrdir = currNormal.Reflect(rdir);
			}
			else {
				rrO[0] = FLT_MAX;
			}
		}
	}

	if (rz == 0.0f)
		return 0;

	return 1;

}

int TMesh::RayTriangleIntersection(V3 rO, V3 rdir, V3 V0, V3 V1, V3 V2,
	V3 &abc, float &currz) {

	M33 M;
	M.SetColumn(0, V0);
	M.SetColumn(1, V1);
	M.SetColumn(2, V2);
	M = M.Inverted();
	V3 q = M*rO;
	V3 r = M*rdir;
	float t = (1 - (q[0] + q[1] + q[2])) / (r[0] + r[1] + r[2]);
	if (t < 0.001f || t > 1e5f)
		return 0;
	abc = q + r*t;
	if (abc[0] < 0.0f || abc[1] < 0.0f || abc[2] < 0.0f)
		return 0;
	currz = 1.0f / t;
	return 1;

}


void TMesh::SetAABB(AABB &aabb) {

	for (int vi = 0; vi < vertsN; vi++) {
		aabb.AddPoint(verts[vi]);
	}

}

void TMesh::Scale(float scf) {

	V3 c = GetCenter();
	Translate(c*-1.0f);

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] * scf;

	Translate(c);

}
#if 0

unsigned int TMesh::cubeTexture() {


	GLuint textureID[2];

	glGenTextures(2, textureID);	
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID[0]);

	vector<std::string> faces =
	{
			"right_1.png",
			"left_1.png",
			"top_1.png",
			"bottom_1.png",
			"front_1.png",
			"back_11.png"
	};

	int width, height, nrChannels;
	unsigned char* data, *data1;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data == nullptr) {
			std::cerr << "Image reading failed." << std::endl;
			return 0;
		}
		else if (nrChannels != 3 && nrChannels != 4) {
			//std::cerr << "The loaded image doesn't have RGB color components." << std::endl;
			//std::cerr << "The loaded image has " << nrChannels << " channels" << std::endl;
			return 0;
		}
		else {
			//std::cout << "The image loaded has size " << width << "x" << height << std::endl;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
	}

	//glEnable(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textureID[1]);

	 faces =
	{
			"self.JPG",
#if 0
			"left.png",
			"top.png",
			"bottom.png",
			"front.png",
			"back.png"
#endif
	};

	 glBindTexture(GL_TEXTURE_2D, textureID[1]);
	
	for (unsigned int i = 0; i < 1; i++)
	{
		data1 = stbi_load("self.JPG", &width, &height, &nrChannels, 0);
		if (data1 == nullptr) {
			std::cerr << "Image reading failed." << std::endl;
			return 0;
		}
		else if (nrChannels != 3 && nrChannels != 4) {
			//std::cerr << "The loaded image doesn't have RGB color components." << std::endl;
			//std::cerr << "The loaded image has " << nrChannels << " channels" << std::endl;
			return 0;
		}
		else {
			//std::cout << "The image loaded has size " << width << "x" << height << std::endl;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1
		);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
#if 0
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 1);
	
#endif
	return 0;
}

unsigned int TMesh::cubeTexture2() {

	unsigned int textureID;
	glGenTextures(2, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	vector<std::string> faces =
	{
			"right.png",
			"left.png",
			"top.png",
			"bottom.png",
			"front.png",
			"back.png"
};

	int width, height, nrChannels;
	unsigned char* data;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data == nullptr) {
			std::cerr << "Image reading failed." << std::endl;
			return 0;
		}
		else if (nrChannels != 3 && nrChannels != 4) {
			//std::cerr << "The loaded image doesn't have RGB color components." << std::endl;
			//std::cerr << "The loaded image has " << nrChannels << " channels" << std::endl;
			return 0;
		}
		else {
			//std::cout << "The image loaded has size " << width << "x" << height << std::endl;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
	}

	return textureID;

}

#endif

void TMesh::RenderHW() {
	
#if 0
	if (envReflec == 0)	{  // 1 for reflect, tpot
		glDepthMask(GL_FALSE);		
	}
	else {
		glDepthMask(GL_TRUE);
	}
	
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
#endif
	
	
	
	glBindTexture(GL_TEXTURE_2D, scene->texs);



	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, (float*)scene->qtexCord);
	glVertexPointer(3, GL_FLOAT, 0, (float*)verts);
//	glColorPointer(3, GL_FLOAT, 0, (float*)colors);
	glNormalPointer(GL_FLOAT, 0, (float*)normals);
	
	
	glDrawElements(GL_TRIANGLES, 3 * trisN, GL_UNSIGNED_INT, tris);

	

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void TMesh::RenderHWBB() {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (float*)verts);
	glColorPointer(3, GL_FLOAT, 0, (float*)colors);
	glNormalPointer(GL_FLOAT, 0, (float*)normals);
	glDrawElements(GL_TRIANGLES, 3 * trisN, GL_UNSIGNED_INT, tris);
	


	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}
