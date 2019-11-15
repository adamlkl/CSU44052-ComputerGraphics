#include "maths_funcs.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

Quaternion initializeQuaternion(float x, float y, float z, float w);
void normalize(Quaternion * quaternion);
mat4 toRotationMatrix(Quaternion * quaternion);
Quaternion fromMatrix(mat4 matrix);
Quaternion interpolate(Quaternion * a, Quaternion * b, float blend);
