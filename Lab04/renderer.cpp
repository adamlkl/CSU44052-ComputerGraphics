/*----------------------------------------------------------------------------
RENDERING
----------------------------------------------------------------------------*/
#include "renderer.h"
#include "utility.h"
#include <iostream> 
#include <string>
#include <vector>

mat4 render(Model model, const char* file_name, mat4 view, mat4 projection, DWORD currentAnimationTime, vec3 cameraPosition) {
	ModelData mesh_data = model.mesh;
	GLuint shaderProgramID = model.shaderProgramID;
	glUseProgram(shaderProgramID);
	glBindVertexArray(model.mesh_vao);

	std::vector<ModelTexture> modelTextures;
	ModelTexture modelTexture;
	modelTexture.textureID = LoadTextureFromFile(file_name, "");
	modelTexture.path = file_name;
	modelTexture.type = "texture_diffuse";
	modelTextures.push_back(modelTexture);
	mesh_data.mTextures = modelTextures;

	unsigned int diffuseNormal = 1;
	unsigned int specularNormal = 1;

	for (unsigned int count = 0; count < mesh_data.mTextures.size(); count++)
	{
		// activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		glActiveTexture(GL_TEXTURE0 + count);

		std::string code;
		std::string renderingType = mesh_data.mTextures[count].type;

		if (renderingType == "texture_diffuse" && diffuseNormal <= 3)
			code = std::to_string(diffuseNormal++);

		else if (renderingType == "texture_specular" && specularNormal <= 3)
		{
			code = std::to_string(specularNormal++);
		}

		int texture_location = glGetUniformLocation(shaderProgramID, (renderingType + code).c_str());
		glUniform1f(texture_location, count);
		glBindTexture(GL_TEXTURE_2D, mesh_data.mTextures[count].textureID);
	}
	glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, model.textureID);
	 
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	int bones_mat_location = glGetUniformLocation(shaderProgramID, "bones");
	int view_position_mat_location = glGetUniformLocation(shaderProgramID, "viewPosition");

	// vec3 cameraPos = camera.pos;
	glUniform3fv(view_position_mat_location, sizeof(cameraPosition.v), cameraPosition.v);

	mat4 transformation = identity_mat4();
	transformation = scale(transformation, vec3(model.scaling[0], model.scaling[1], model.scaling[2]));
	transformation = rotate(transformation, model.rotation);
	transformation = translate(transformation, vec3(model.position[0], model.position[1], model.position[2]));

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transformation.m);

	std::vector<mat4> boneTransforms;
	TransformBone(&(model.mesh), currentAnimationTime, boneTransforms);

	GLuint m_boneLocation[200];
	for (unsigned int i = 0; i < 200; i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "bones[%d]", i);
		m_boneLocation[i] = glGetUniformLocation(shaderProgramID, Name);
	}
	for (unsigned int i = 0; i < boneTransforms.size() && i < 200; i++) {
		glUniformMatrix4fv(m_boneLocation[i], 1, GL_FALSE, boneTransforms[i].m);
		//printf("%d\n",boneTransforms[i]);
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
	return transformation;
}

void renderSkyBox(SkyBox skyBox, mat4 view, mat4 projection)
{
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	GLuint shaderProgramID = skyBox.shaderProgramID;

	glUseProgram(shaderProgramID);
	glBindVertexArray(skyBox.vao);
	//printf("Vao %d Shader_ID %d Texture_ID %d \n", sky.vao,sky.shaderProgramID, sky.textureID);
	glActiveTexture(GL_TEXTURE0);
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view_matrix");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "projection_matrix");
	int texture_location = glGetUniformLocation(shaderProgramID, "skyBox");
	mat4 view_untranslated = mat4(
		view.m[0], view.m[4], view.m[8], 0.0f,
		view.m[1], view.m[5], view.m[9], 0.0f,
		view.m[2], view.m[6], view.m[10], 0.0f,
		0.0f, 0.0f, 0.0f, view.m[15]);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_untranslated.m);
	glUniform1i(texture_location, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.textureID);

	glDrawArrays(GL_TRIANGLES, 0, skyBox.mPointCount);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}
