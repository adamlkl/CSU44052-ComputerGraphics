#ifndef TERRAIN_HEADER
#define TERRAIN_HEADER

#include <iostream>
#include <vector> // STL dynamic memory.

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "maths_funcs.h"


struct Terrain{
	GLfloat x;
	GLfloat z;

	unsigned int textureID;
	GLuint shaderProgramID;
	GLuint vao;
	size_t mPointCount = 0;

	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
};

void generateTerrain(int gridX, int gridZ);

#endif