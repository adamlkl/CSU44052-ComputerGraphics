#ifndef SKYBOX_HEADER
#define SKYBOX_HEADER

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <array>
#include <iostream>
#include <vector>
#include "maths_funcs.h"


struct SkyBox {
	unsigned int textureID;
	GLuint shaderProgramID;
	GLuint vao;
	size_t mPointCount = 0;
};


// unsigned int loadCubeMap(std::vector<std::string> cubeFaces);
unsigned int loadCubeMap();
void generateSkyBoxBufferMesh(SkyBox *skyBox);
#endif