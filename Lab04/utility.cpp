#include "utility.h"

/*----------------------------------------------------------------------------
TRANSFORMATION CALCULATION
----------------------------------------------------------------------------*/
mat4 rotate(mat4 modelMatrix, GLfloat rotation[]) {
	mat4 tempMatrix = modelMatrix;
	tempMatrix = rotate_x_deg(tempMatrix, rotation[0]);
	tempMatrix = rotate_y_deg(tempMatrix, rotation[1]);
	tempMatrix = rotate_z_deg(tempMatrix, rotation[2]);
	return tempMatrix;
}

mat4 applyTransformation(Model model) {
	mat4 transformation = identity_mat4();
	transformation = scale(transformation, vec3(model.scaling[0], model.scaling[1], model.scaling[2]));
	transformation = rotate(transformation, model.rotation);
	transformation = translate(transformation, vec3(model.position[0], model.position[1], model.position[2]));
	return transformation;
}

void copy_array(GLfloat matA[3], GLfloat matB[3]) {
	for (int i = 0; i < 3; i++)
		matA[i] = matB[i];
}