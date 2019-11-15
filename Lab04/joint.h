#ifndef JOINT_HEADER
#define JOINT_HEADER
#include <GL/freeglut.h>
#include <GL/GL.h>

#include "maths_funcs.h"
#include <vector> // STL dynamic memory.

struct Joint {
	GLuint index;
	std::string name;
	std::vector<Joint*> children;
	mat4 animatedTransform;
	mat4 localBindTransform;
	mat4 inverseBindTransform;
};

void setAnimationTransform(Joint * joint, mat4 animationTransform);
void addChild(Joint * parent, Joint * child);
void calculateInverseBindTransform(Joint * joint, mat4 parentBindTransform);
#endif
