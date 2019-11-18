#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #include"helper.h"
#pragma region MESH LOADING

/*************************************************************************************
HELPER FUNCTIONS
*************************************************************************************/
inline mat4 ConvertAiMat4ToMat4(const aiMatrix4x4* matA)
{
	mat4 matB = mat4(
		(GLfloat)matA->a1, (GLfloat)matA->a2, (GLfloat)matA->a3, (GLfloat)matA->a4,
		(GLfloat)matA->b1, (GLfloat)matA->b2, (GLfloat)matA->b3, (GLfloat)matA->b4,
		(GLfloat)matA->c1, (GLfloat)matA->c2, (GLfloat)matA->c3, (GLfloat)matA->c4,
		(GLfloat)matA->d1, (GLfloat)matA->d2, (GLfloat)matA->d3, (GLfloat)matA->d4);
	return matB;
}

inline mat3 ConvertAiMat3ToMat3(const aiMatrix3x3* matA)
{
	mat3 matB = mat3(
		(GLfloat)matA->a1, (GLfloat)matA->a2, (GLfloat)matA->a3,
		(GLfloat)matA->b1, (GLfloat)matA->b2, (GLfloat)matA->b3,
		(GLfloat)matA->c1, (GLfloat)matA->c2, (GLfloat)matA->c3);
	return matB;
}

mat4 ConvertMat3toMat4(mat3 matrix)
{
	float* temp = matrix.m;
	mat4 res = mat4(
		temp[0], temp[3], temp[6], 0.0f,
		temp[1], temp[4], temp[7], 0.0f,
		temp[2], temp[5], temp[8], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	return res;
}


/*************************************************************************************
HELPER FUNCTIONS FOR ANIMATIONS
*************************************************************************************/

const aiNodeAnim * FindAnimationNode(const aiAnimation * animation, const std::string NodeName)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++) {
		aiNodeAnim* pNodeAnim = animation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

unsigned int FindScalingKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation)
{
	if (!pNodeAnimation->mNumScalingKeys > 0) {
		printf("Error no scaling key frames");
	}

	for (unsigned int i = 0; i < pNodeAnimation->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)pNodeAnimation->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	printf("Animation time longer than maximum keyframe");
	return 0;
}

unsigned int FindRotationKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation)
{
	if (!pNodeAnimation->mNumRotationKeys > 0) {
		printf("Error no rotation key frames");
	}

	for (unsigned int i = 0; i < pNodeAnimation->mNumRotationKeys - 1; i++) {
		if (animationTime < (float)pNodeAnimation->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	printf("Animation time longer than maximum keyframe");

	return 0;
}

unsigned int FindPositionKeyFrame(float animationTime, const aiNodeAnim* pNodeAnimation)
{
	for (unsigned int i = 0; i < pNodeAnimation->mNumPositionKeys - 1; i++) {
		if (animationTime < (float)pNodeAnimation->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	printf("Animation time longer than maximum keyframe");
	return 0;
}


/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */

	/*
	modelData.scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);*/

	modelData.scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs | aiProcess_LimitBoneWeights | aiProcess_CalcTangentSpace
	);

	const aiScene* scene = modelData.scene;
	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	modelData.m_GlobalInverseTransform = ConvertAiMat4ToMat4(&(scene->mRootNode->mTransformation));

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	unsigned int vertexCount = 0; 
	modelData.mEntries.resize(scene->mNumMeshes);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		printf("    %i bones in mesh\n", mesh->mNumBones);
		modelData.mPointCount += mesh->mNumVertices;

		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                      
				*/
				const aiVector3D* vTangent = &(mesh->mTangents[v_i]);
				modelData.mTangents.push_back(vec3(vTangent->x, vTangent->y, vTangent->z));
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");

			modelData.mTextures.insert(modelData.mTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<ModelTexture> specularMaps = loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
			modelData.mTextures.insert(modelData.mTextures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<ModelTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			modelData.mTextures.insert(modelData.mTextures.end(), normalMaps.begin(), normalMaps.end());

		}
	}

	// TODO: handle rigging
	// each bones and their respective vertex weights
	// now the bones can refer to any connected vertex

	// now adding bones and vertex bone weights
	// Need to do after as any bone can refer to any vertice, even unprocecessed ones maybe
	// So reserve space in advance
	modelData.boneVertices.resize(modelData.mPointCount);

	unsigned int boneCount = 0;
	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {

		modelData.mEntries[m_i] = vertexCount;
		const aiMesh* mesh = scene->mMeshes[m_i];
		vertexCount += mesh->mNumVertices;

		for (unsigned int b_i = 0; b_i < mesh->mNumBones; b_i++) {

			unsigned int boneIndex = 0;
			std::string BoneName(mesh->mBones[b_i]->mName.data);

			if (modelData.mBoneMapping.find(BoneName) == modelData.mBoneMapping.end()) 
			{
				// Allocate an index for a new bone
				boneIndex = boneCount;
				modelData.mBoneMapping[BoneName] = boneIndex;
				boneCount++;
				Bone boneInfo;
				modelData.mBoneInfo.push_back(boneInfo);
				modelData.mBoneInfo[boneIndex].inverseBindPoseTransform = ConvertAiMat4ToMat4(&(mesh->mBones[b_i]->mOffsetMatrix));
			}

			else 
			{
				boneIndex = modelData.mBoneMapping[BoneName];
			}

			for (unsigned int j = 0; j < mesh->mBones[b_i]->mNumWeights; j++) {

				unsigned int VertexID = modelData.mEntries[m_i] + mesh->mBones[b_i]->mWeights[j].mVertexId;
				float boneWeight = mesh->mBones[b_i]->mWeights[j].mWeight;
				VertexBone boneData = modelData.boneVertices[VertexID];
				modelData.boneVertices[VertexID] = addBoneData(boneData, boneIndex, boneWeight);
			}



		}
	}
	modelData.mBoneCount = boneCount;

	// Now, load the animations
	for (int i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* pAnimation = scene->mAnimations[i];
		std::map<std::string, aiNodeAnim*> nodeMapping;

		for (unsigned int j = 0; j < pAnimation->mNumChannels; j++) {
			aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];

			if (nodeMapping.find(std::string(pNodeAnim->mNodeName.data)) == nodeMapping.end()) {
				nodeMapping[std::string(pNodeAnim->mNodeName.data)] = pNodeAnim;
			}
		}
		modelData.mAnimationNodes.push_back(nodeMapping);
	}
	// aiReleaseImport(scene);
	modelData.scene = scene;
	return modelData;
}
#pragma endregion MESH LOADING

// Shader Functions
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(const char* vertexShaderTextFileName, const char* fragmentShaderTextFileName)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, vertexShaderTextFileName, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, fragmentShaderTextFileName, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions 
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh(Model *model) {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	// mesh_data = load_mesh(MESH_NAME);
	ModelData mesh_data = model->mesh;
	
	GLuint shaderProgramID = model->shaderProgramID;
	GLuint loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	GLuint loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	GLuint loc3 = glGetAttribLocation(shaderProgramID, "texture_coordinates");
	GLuint loc4 = glGetAttribLocation(shaderProgramID, "bone_ids");
	GLuint loc5 = glGetAttribLocation(shaderProgramID, "bone_weights");

	// loc for skeletons
	// GLuint loc4, loc5;

	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);

	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	unsigned int vt_vbo = 0;
	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec2), &mesh_data.mTextureCoords[0], GL_STATIC_DRAW);

	unsigned int vb_vbo = 0;
	glGenBuffers(1, &vb_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vb_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_data.boneVertices[0]) * mesh_data.boneVertices.size(), &mesh_data.boneVertices[0], GL_STATIC_DRAW);

	GLuint vao = model->mesh_vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//	This is for bones coordinates
	glBindBuffer(GL_ARRAY_BUFFER, vb_vbo);
	glEnableVertexAttribArray(loc4);
	glVertexAttribIPointer(loc4, 4, GL_INT, sizeof(VertexBone), (const GLvoid*)0);
	glEnableVertexAttribArray(loc5);
	glVertexAttribPointer(loc5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBone), (const GLvoid*)16);

	model->mesh_vao = vao;
}

unsigned int LoadTextureFromFile(const char * file_path, bool gamma)
{
	// read png file 
	std::string filename = file_path;

	// get textureID
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	//load texture png file 	
	unsigned char *data = stbi_load("3d_Models/model/diffuse.png", &width, &height, &nrComponents, 0);
	if (data)
	{
		std::cout << "path: " << file_path << std::endl;
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << file_path << std::endl;
		stbi_image_free(data);
	}
	std::cout << "path: " << file_path << std::endl;
	return textureID;
}

std::vector<ModelTexture> loadMaterialTextures(aiMaterial * material, aiTextureType type, std::string typeName)
{
	std::vector<ModelTexture> modelTextures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		ModelTexture modelTexture;
		modelTexture.textureID = LoadTextureFromFile(str.C_Str(), "");
		modelTexture.type = typeName;
		modelTexture.path = str.C_Str();;

		modelTextures.push_back(modelTexture);
	}
	return modelTextures;
}

VertexBone addBoneData(VertexBone boneData, unsigned int boneID, float weight)
{
	float minWeight = -1;
	int minWeightIndex = -1;

	// only want 3 most influential bones
	for (unsigned int i = 0; i < 3; i++) {
		//checking for an empty slot
		if (boneData.weights[i] == 0) {
			boneData.weights[i] = weight;
			boneData.id[i] = boneID;
			return boneData;
		}
		else if (minWeight < 0 || boneData.weights[i] < minWeight) {
			minWeight = boneData.weights[i];
			minWeightIndex = i;
		}
	}

	// we dont't want to take in too much weights, so remove the ones than are less than the current bone weight 
	if (minWeight < weight) {
		boneData.weights[minWeightIndex] = weight;
		boneData.id[minWeightIndex] = boneID;
	}
	return boneData;
}
#pragma endregion VBO_FUNCTIONS

void TransformBone(ModelData * modelData, float time, std::vector<mat4>& transformations)
{
	mat4 matA = identity_mat4();
	if (modelData->scene->HasAnimations()) {
		float ticksPerSecond = modelData->scene->mAnimations[0]->mTicksPerSecond != 0 ?
			modelData->scene->mAnimations[0]->mTicksPerSecond : 24.0f;
		float timeInTicks = time * ticksPerSecond;
		float animationTime = fmod(timeInTicks, modelData->scene->mAnimations[0]->mDuration);

		ReadNodeHierarchy(modelData, animationTime, modelData->scene->mRootNode, matA);
	}
	else {
		printf("No animations found \n");
	}

	transformations.resize(modelData->mBoneCount);

	for (unsigned int i = 0; i < modelData->mBoneCount; i++) {
		transformations[i] = modelData->mBoneInfo[i].finalTransform;
	}
}

void ReadNodeHierarchy(ModelData * modelData, float animationTime, const aiNode * pNode, const mat4 & parentTransformation)
{
	std::string nodeName(pNode->mName.data);

	std::map<std::string, aiNodeAnim*> nodeAnimationMapping = modelData->mAnimationNodes[0];

	mat4 nodeTransformation = ConvertAiMat4ToMat4(&(pNode->mTransformation));

	const aiNodeAnim* pNodeAnim = nodeAnimationMapping[nodeName];//FindNodeAnimation(pAnimation, nodeName);

	// calculate the related transformation of the joint in relative to its parents
	mat4 translation = identity_mat4();
	mat4 scaling = identity_mat4();
	mat4 rotation = identity_mat4();

	if (pNodeAnim) {
		// scaling interpolation
		aiVector3D Scaling;
		CalculateInterpolatedScaling(Scaling, animationTime, pNodeAnim);
		scaling = scale(scaling, vec3(Scaling.x, Scaling.y, Scaling.z));

		// rotation interpolation using Quanternions
		aiQuaternion quaternionRotation;
		CalculateInterpolatedRotation(quaternionRotation, animationTime, pNodeAnim);
		aiMatrix3x3 rotationM = quaternionRotation.GetMatrix();
		mat3 tmpRotation = ConvertAiMat3ToMat3(&rotationM);
		rotation = ConvertMat3toMat4(tmpRotation);

		// translation interpolation
		aiVector3D Translation;
		CalculateInterpolatedPosition(Translation, animationTime, pNodeAnim);
		translation = translate(translation, vec3(Translation.x, Translation.y, Translation.z));
	}
	// now multiply all the transformations together
	nodeTransformation = translation * rotation * scaling;

	mat4 globalTransformation = ((mat4)parentTransformation) * nodeTransformation;

	if (modelData->mBoneMapping.find(nodeName) != modelData->mBoneMapping.end()) {
		unsigned int boneIndex = modelData->mBoneMapping[nodeName];

		modelData->mBoneInfo[boneIndex].finalTransform = modelData->m_GlobalInverseTransform *
			globalTransformation * modelData->mBoneInfo[boneIndex].inverseBindPoseTransform;

		//Bug Impact reduction code
		modelData->mBoneInfo[boneIndex].finalTransform = translate(
			modelData->mBoneInfo[boneIndex].finalTransform, vec3(0.0f, 6.0f, -1.5f));
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(modelData, animationTime, pNode->mChildren[i], globalTransformation);
	}
}

void CalculateInterpolatedPosition(aiVector3D & out, float animationTime, const aiNodeAnim * pNodeAnimation)
{
	if (pNodeAnimation->mNumPositionKeys == 1) {
		out = pNodeAnimation->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPositionKeyFrame(animationTime, pNodeAnimation);

	unsigned int NextPositionIndex = (PositionIndex + 1);
;
	if (!(NextPositionIndex < pNodeAnimation->mNumPositionKeys)) {
		printf("Error:out of position keyframes");
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	float deltaTime = (float)(pNodeAnimation->mPositionKeys[NextPositionIndex].mTime - pNodeAnimation->mPositionKeys[PositionIndex].mTime);
	float factor = (animationTime - (float)pNodeAnimation->mPositionKeys[PositionIndex].mTime) / deltaTime;

	if (factor < 0.0f)
		factor = 0.0f;

	if (!(factor >= 0.0f && factor <= 1.0f)) {
		printf("Error:Bad Time info. Animation time was:%f but next frame was at:%f\n",
			animationTime, (float)pNodeAnimation->mPositionKeys[PositionIndex].mTime);
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	const aiVector3D& Start = pNodeAnimation->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnimation->mPositionKeys[NextPositionIndex].mValue;

	aiVector3D Delta = End - Start;
	out = Start + factor * Delta;
}

void CalculateInterpolatedRotation(aiQuaternion & out, float animationTime, const aiNodeAnim * pNodeAnimation)
{
	// Need at least two values to perform interpolation operation
	if (pNodeAnimation->mNumRotationKeys == 1) {
		out = pNodeAnimation->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotationKeyFrame(animationTime, pNodeAnimation);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	if (!(NextRotationIndex < pNodeAnimation->mNumRotationKeys)) {
		printf("Error:out of rotation keyframes");
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	float DeltaTime = (float)(pNodeAnimation->mRotationKeys[NextRotationIndex].mTime - pNodeAnimation->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - (float)pNodeAnimation->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;
	if (!(Factor >= 0.0f && Factor <= 1.0f)) {
		printf("Error:Bad Time info");
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	const aiQuaternion& StartRotationQ = pNodeAnimation->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnimation->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();
}

void CalculateInterpolatedScaling(aiVector3D & out, float animationTime, const aiNodeAnim * pNodeAnimation)
{
	if (pNodeAnimation->mNumScalingKeys == 1) {
		out = pNodeAnimation->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScalingKeyFrame(animationTime, pNodeAnimation);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	if (!(NextScalingIndex < pNodeAnimation->mNumScalingKeys)) {
		printf("Error:out of scaling keyframes");
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	float DeltaTime = (float)(pNodeAnimation->mScalingKeys[NextScalingIndex].mTime - pNodeAnimation->mScalingKeys[ScalingIndex].mTime);
	float Factor = (animationTime - (float)pNodeAnimation->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;
	if (!(Factor >= 0.0f && Factor <= 1.0f)) {
		printf("Error:Bad Time info");
		out = aiVector3D(1.0, 1.0, 1.0);
		return;
	}
	const aiVector3D& Start = pNodeAnimation->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnimation->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	out = Start + Factor * Delta;
}
/*
void loadTextures(Model * model, const char * file_name, int active_arg, const GLchar * textString, int textNumber)
{
	ModelData mesh_data = model->mesh;
	GLuint shaderProgramID = model->shaderProgramID;

	int x, y, n;
	int force_channels = 4;
	unsigned char *image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);

	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n",
			file_name);
	}

	glActiveTexture(active_arg);
	glBindTexture(GL_TEXTURE_2D, model->textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		image_data);
	glUniform1i(glGetUniformLocation(shaderProgramID, textString), textNumber);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
}*/

unsigned int loadCubeMap(std::vector<std::string> cubeFaces)
{
	unsigned int cubeTextureID;
	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < cubeFaces.size(); i++)
	{
		unsigned char *data = stbi_load(cubeFaces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << cubeFaces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubeTextureID;
}

