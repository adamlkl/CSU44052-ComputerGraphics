#include <vector>
#include "joint.h"

void addChild(Joint *parent, Joint *child) {
	std::vector<Joint*> children = parent->children;
	children.push_back(child);
}

void setAnimationTransform(Joint * joint, mat4 animationTransform) {
	joint->animatedTransform = animationTransform;
}

void calculateInverseBindTransform(Joint * joint, mat4 parentBindTransform) {
	mat4 localBindTransform = joint->localBindTransform;
	mat4 bindTransform = parentBindTransform * localBindTransform;
	joint->inverseBindTransform = inverse(bindTransform);
	
	std::vector<Joint*> children = joint->children;
	for (Joint * child : children) {
		calculateInverseBindTransform(child, bindTransform);
	}
}