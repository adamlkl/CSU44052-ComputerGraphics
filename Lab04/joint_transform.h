#include "maths_funcs.h"
#include "quaternion.h"

struct JointTransform {
	vec3 position;
	Quaternion quaternion;
};

mat4 getLocalTransform(JointTransform * jointTransform);
JointTransform interpolate(JointTransform * a, JointTransform * b, float progression);
vec3 interpolate(vec3 start, vec3 end, float progression);