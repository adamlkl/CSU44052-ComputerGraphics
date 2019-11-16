// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "maths_funcs.h"
#include "model.h"

mat4 rotate(mat4 modelMatrix, GLfloat rotation[]);
mat4 applyTransformation(Model model);
void copy_array(GLfloat matA[3], GLfloat matB[3]);