/*----------------------------------------------------------------------------
RENDERING
----------------------------------------------------------------------------*/
#include "renderer.h"
#include "utility.h"

mat4 render(Model model, mat4 view, mat4 projection) {
	ModelData mesh_data = model.mesh;
	GLuint shaderProgramID = model.shaderProgramID;
	glUseProgram(shaderProgramID);
	glBindVertexArray(model.mesh_vao);
	glBindTexture(GL_TEXTURE_2D, model.textureID);
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	mat4 transformation = identity_mat4();
	transformation = scale(transformation, vec3(model.scaling[0], model.scaling[1], model.scaling[2]));
	transformation = rotate(transformation, model.rotation);
	transformation = translate(transformation, vec3(model.position[0], model.position[1], model.position[2]));

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transformation.m);
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
