#ifndef VERTEX_HEADER
#define VERTEX_HEADER

#include "maths_funcs.h"

struct VertexBone {
	unsigned int id[4];
	float weights[3];
};

struct Bone {
	mat4 inverseBindPoseTransform;
	mat4 finalTransform;

	Bone()
	{
		inverseBindPoseTransform = zero_mat4();
		finalTransform = zero_mat4();
	}
};
#endif 