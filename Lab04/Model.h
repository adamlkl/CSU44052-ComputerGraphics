#ifndef MODEL_HEADER
#define MODEL_HEADER

#include <array>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

#include "maths_funcs.h"
#include "model_texture.h"

struct ModelData {
	const aiScene* scene;
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;

	// TODO: for rigging
};

struct Model {
	ModelData mesh;
	GLuint shaderProgramID;
	GLuint mesh_vao = 0;
	GLfloat position[3] = { 0.0f , 0.0f, 0.0f };
	GLfloat rotation[3] = { 0.0f , 0.0f, 0.0f };
	GLfloat scaling[3] = { 1.0f , 1.0f, 1.0f };

	// TODO: for rigging
	GLuint textureID;
	ModelTexture texture;
};

ModelData load_mesh(const char* file_name);
// void loadTextures(Model * model, const char * file_name, int active_arg, const GLchar * textString, int textNumber);
char* readShaderSource(const char* shaderFile);
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint CompileShaders(const char* vertexShaderTextFileName, const char* fragmentShaderTextFileName);
void generateObjectBufferMesh(Model *model);

// TODO: handle rigging and texturing #pragma once
#endif