#pragma once

#include "V3.h"

class AABB {
public:
	V3 corners[2];
	AABB(V3 firstPoint);
	AABB();
	void AddPoint(V3 P);
	int ClipWithFrame(int w, int h);
};