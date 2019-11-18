#ifndef MODEL_HEADER
#define MODEL_HEADER

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <array>
#include <iostream>
#include <map>
#include <math.h>
#include <vector> // STL dynamic memory.

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

#include "bone.h"
#include "maths_funcs.h"
#include "model_texture.h"

struct ModelData {
	const aiScene* scene;
	size_t mPointCount = 0;
	size_t mBoneCount = 0;

	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
	std::vector<ModelTexture> mTextures; 
	std::vector<vec3> mTangents;

	// TODO: for rigging
	std::vector<VertexBone> boneVertices;
	std::map<std::string, unsigned int> mBoneMapping; // maps a bone name to its index
	mat4 m_GlobalInverseTransform;
	std::vector<unsigned int> mEntries;
	std::vector<Bone> mBoneInfo;
	std::vector<std::map<std::string, aiNodeAnim*>> mAnimationNodes;
};

struct Model {
	ModelData mesh;
	GLuint textureID;
	GLuint shaderProgramID;
	GLuint mesh_vao = 0;
	GLfloat position[3] = { 0.0f , 0.0f, 0.0f };
	GLfloat rotation[3] = { 0.0f , 0.0f, 0.0f };
	GLfloat scaling[3] = { 1.0f , 1.0f, 1.0f };

	// TODO: for rigging
	std::string texturePath = "";
	std::vector<std::string> tag;
};

ModelData load_mesh(const char* file_name);
char* readShaderSource(const char* shaderFile);
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint CompileShaders(const char* vertexShaderTextFileName, const char* fragmentShaderTextFileName);
void generateObjectBufferMesh(Model *model);

// Loading Textures
// void loadTextures(Model * model, const char * file_name, int active_arg, const GLchar * textString, int textNumber);
unsigned int LoadTextureFromFile(const char *file_path, bool gamma = false);
std::vector<ModelTexture> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName);


// TODO: handle rigging and texturing 
VertexBone addBoneData(VertexBone boneData, unsigned int boneID, float weight);

// Animations
const aiNodeAnim* FindAnimationNode(const aiAnimation* animation, const std::string NodeName);
void TransformBone(ModelData *modelData, float time, std::vector<mat4>& transformations);
void ReadNodeHierarchy(ModelData *modelData, float animationTime, const aiNode* pNode, const mat4& parentTransformation);

void CalculateInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnimation);
void CalculateInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnimation);
void CalculateInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnimation);

// helper functions
unsigned int FindScalingKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation);
unsigned int FindRotationKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation);
unsigned int FindPositionKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation);
mat4 ConvertMat3toMat4(mat3 matrix);

unsigned int loadCubeMap(std::vector<std::string> cubeFaces);
#endif