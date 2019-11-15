#include <math.h>
#include "maths_funcs.h"
#include "quaternion.h"

Quaternion initializeQuaternion(float x, float y, float z, float w)
{
	Quaternion quaternion = Quaternion();
	quaternion.x = x;
	quaternion.y = y;
	quaternion.z = z;
	quaternion.w = w;
	normalize(&quaternion);
	return quaternion;
}

/**
* Normalizes the quaternion.
*/
void normalize(Quaternion * quaternion)
{
	float magnitude = (float)sqrtf(quaternion->w * quaternion->w + quaternion->x * quaternion->x
		+ quaternion->y * quaternion->y + quaternion->z * quaternion->z);
	quaternion->w /= magnitude;
	quaternion->x /= magnitude;
	quaternion->y /= magnitude;
	quaternion->z /= magnitude;
}

/**
* Converts the quaternion to a 4x4 matrix representing the exact same
* rotation as this quaternion. (The rotation is only contained in the
* top-left 3x3 part, but a 4x4 matrix is returned here for convenience
* seeing as it will be multiplied with other 4x4 matrices).
*
* More detailed explanation here:
* http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/
*
* @return The rotation matrix which represents the exact same rotation as
*         this quaternion.
*/
mat4 toRotationMatrix(Quaternion * quaternion)
{
	mat4 matrix = zero_mat4();
	float xy = quaternion->x * quaternion->y;
    float xz = quaternion->x * quaternion->z;
	float xw = quaternion->x * quaternion->w;
	float yz = quaternion->y * quaternion->z;
	float yw = quaternion->y * quaternion->w;
	float zw = quaternion->z * quaternion->w;
	float xSquared = quaternion->x * quaternion->x;
	float ySquared = quaternion->y * quaternion->y;
	float zSquared = quaternion->z * quaternion->z;

	matrix.m[0] = 1 - 2 * (ySquared + zSquared);
	matrix.m[1] = 2 * (xy - zw);
	matrix.m[2] = 2 * (xz + yw);
	matrix.m[3] = 0; 
	matrix.m[4] = 2 * (xy + zw);
	matrix.m[5] = 1 - 2 * (xSquared + zSquared);
	matrix.m[6] = 2 * (yz - xw);
	matrix.m[7] = 0;
	matrix.m[8] = 2 * (xz - yw);
	matrix.m[9] = 2 * (yz + xw);
	matrix.m[10] = 1 - 2 * (xSquared + ySquared);
	matrix.m[11] = 0;
	matrix.m[12] = 0;
	matrix.m[13] = 0;
	matrix.m[14] = 0;
	matrix.m[15] = 1;

	return matrix;
}

/**
* Extracts the rotation part of a transformation matrix and converts it to
* a quaternion using the magic of maths.
*
* More detailed explanation here:
* http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
*
* @param matrix
*		- the transformation matrix containing the rotation which this
*       quaternion shall represent.
*/
Quaternion fromMatrix(mat4 matrix)
{
	float w, x, y, z;
	float diagonal = matrix.m[0] + matrix.m[5] + matrix.m[10];
	if (diagonal > 0) {
		float w4 = (float)(sqrtf(diagonal + 1.0f) * 2.0f);
		w = w4 / 4.0f;
		x = (matrix.m[9] - matrix.m[6]) / w4;
		y = (matrix.m[2] - matrix.m[8]) / w4;
		z = (matrix.m[4] - matrix.m[1]) / w4;
	}
	else if ((matrix.m[0] > matrix.m[5]) && (matrix.m[0] > matrix.m[10])) {
		float x4 = (float)(sqrtf(1.0f + matrix.m[0] - matrix.m[5] - matrix.m[10]) * 2.0f);
		w = (matrix.m[9] - matrix.m[6]) / x4;
		x = x4 / 4.0f;
		y = (matrix.m[1] + matrix.m[4]) / x4;
		z = (matrix.m[2] + matrix.m[8]) / x4;
	}
	else if (matrix.m[5] > matrix.m[10]) {
		float y4 = (float)(sqrtf(1.0f + matrix.m[5] - matrix.m[0] - matrix.m[10]) * 2.0f);
		w = (matrix.m[2] - matrix.m[8]) / y4;
		x = (matrix.m[1] + matrix.m[4]) / y4;
		y = y4 / 4.0f;
		z = (matrix.m[6] + matrix.m[9]) / y4;
	}
	else {
		float z4 = (float)(sqrtf(1.0f + matrix.m[10] - matrix.m[0] - matrix.m[5]) * 2.0f);
		w = (matrix.m[4] - matrix.m[1]) / z4;
		x = (matrix.m[2] + matrix.m[8]) / z4;
		y = (matrix.m[6] + matrix.m[9]) / z4;
		z = z4 / 4.0f;
	}

	Quaternion quaternion = initializeQuaternion(x, y, z, w);
	return quaternion;
}

Quaternion interpolate(Quaternion * a, Quaternion * b, float blend)
{
	Quaternion result = initializeQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

	float dot = a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
	float blendI = 1.0f - blend;
	if (dot < 0) {
		result.w = blendI * a->w + blend * -b->w;
		result.x = blendI * a->x + blend * -b->x;
		result.y = blendI * a->y + blend * -b->y;
		result.z = blendI * a->z + blend * -b->z;
	}
	else {
		result.w = blendI * a->w + blend * b->w;
		result.x = blendI * a->x + blend * b->x;
		result.y = blendI * a->y + blend * b->y;
		result.z = blendI * a->z + blend * b->z;
	}
	normalize(&result);
	return result;
}
