#include "stdafx.h"

#include "AABB.h"

AABB::AABB() {

	corners[0][0] = FLT_MAX;

}


AABB::AABB(V3 firstPoint) {

	corners[0] = corners[1] = firstPoint;

}

void AABB::AddPoint(V3 P) {

	if (corners[0][0] == FLT_MAX) {
		corners[0] = corners[1] = P;
		return;
	}

	for (int i = 0; i < 3; i++) {
		if (corners[0][i] > P[i])
			corners[0][i] = P[i];
		if (corners[1][i] < P[i])
			corners[1][i] = P[i];
	}

}

int AABB::ClipWithFrame(int w, int h) {

	if (corners[0][0] > (float)w)
		return 0;
	if (corners[1][0] < 0.0f)
		return 0;
	if (corners[0][1] > (float)h)
		return 0;
	if (corners[1][1] < 0.0f)
		return 0;

	if (corners[0][0] < 0.0f)
		corners[0][0] = 0.0f;
	if (corners[1][0] > w)
		corners[1][0] = (float)w;
	if (corners[0][1] < 0.0f)
		corners[0][1] = 0.0f;
	if (corners[1][1] > h)
		corners[1][1] = (float)h;

	return 1;

}
