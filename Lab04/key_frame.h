#ifndef KEYFRAME_HEADER
#define KEYFRAME_HEADER

#include <map>
#include <vector>

#include "joint_transform.h"

struct KeyFrame {
	float timestamp;
	std::map<std::string, JointTransform*> pose;
};
#endif