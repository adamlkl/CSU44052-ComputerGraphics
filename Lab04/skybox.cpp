#include "skybox.h"
#include "model.h"

std::vector<std::string> cubeFaces{
	"SkyBoxImages/right.png",
	"SkyBoxImages/left.png",
	"SkyBoxImages/top.png",
	"SkyBoxImages/bottom.png",	
	"SkyBoxImages/back.png",
	"SkyBoxImages/front.png"
};

float skyBoxVertices[] = {
	-500.0f,  500.0f, -500.0f,
	-500.0f, -500.0f, -500.0f,
	 500.0f, -500.0f, -500.0f,
	 500.0f, -500.0f, -500.0f,
	 500.0f,  500.0f, -500.0f,
	-500.0f,  500.0f, -500.0f,

	-500.0f, -500.0f,  500.0f,
	-500.0f, -500.0f, -500.0f,
	-500.0f,  500.0f, -500.0f,
	-500.0f,  500.0f, -500.0f,
	-500.0f,  500.0f,  500.0f,
	-500.0f, -500.0f,  500.0f,

	500.0f, -500.0f, -500.0f,
	500.0f, -500.0f,  500.0f,
	500.0f,  500.0f,  500.0f,
	500.0f,  500.0f,  500.0f,
	500.0f,  500.0f, -500.0f,
	500.0f, -500.0f, -500.0f,

	-500.0f, -500.0f,  500.0f,
	-500.0f,  500.0f,  500.0f,
	 500.0f,  500.0f,  500.0f,
	 500.0f,  500.0f,  500.0f,
	 500.0f, -500.0f,  500.0f,
	-500.0f, -500.0f,  500.0f,

	-500.0f,  500.0f, -500.0f,
	 500.0f,  500.0f, -500.0f,
	 500.0f,  500.0f,  500.0f,
	 500.0f,  500.0f,  500.0f,
	-500.0f,  500.0f,  500.0f,
	-500.0f,  500.0f, -500.0f,

	-500.0f, -500.0f, -500.0f,
	-500.0f, -500.0f,  500.0f,
	 500.0f, -500.0f, -500.0f,
	 500.0f, -500.0f, -500.0f,
	-500.0f, -500.0f,  500.0f,
	 500.0f, -500.0f,  500.0f
};

// unsigned int loadCubeMap(std::vector<std::string> cubeFaces)

unsigned int loadingCubeMap() {
	return loadCubeMap(cubeFaces);
}

void generateSkyBoxBufferMesh(SkyBox * skyBox)
{
	unsigned int vp_vbo = 0;
	GLuint shaderProgramID = skyBox->shaderProgramID;
	GLuint loc1 = glGetAttribLocation(shaderProgramID, "position");
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	skyBox->mPointCount = sizeof(skyBoxVertices) / sizeof(vec3);
	glBufferData(GL_ARRAY_BUFFER, skyBox->mPointCount * sizeof(vec3), &skyBoxVertices, GL_STATIC_DRAW);

	GLuint vao = skyBox->vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	skyBox->vao = vao;
}

