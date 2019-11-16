#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "skybox.h"

std::vector<std::string> cubeFaces{
	"SkyBoxImages/right.png",
	"SkyBoxImages/left.png",
	"SkyBoxImages/top.png",
	"SkyBoxImages/bottom.png",
	"SkyBoxImages/front.png",
	"SkyBoxImages/back.png"
};

float skyBoxVertices[] = {
	-20.0f,  20.0f, -20.0f,
	-20.0f, -20.0f, -20.0f,
	 20.0f, -20.0f, -20.0f,
	 20.0f, -20.0f, -20.0f,
	 20.0f,  20.0f, -20.0f,
	-20.0f,  20.0f, -20.0f,

	-20.0f, -20.0f,  20.0f,
	-20.0f, -20.0f, -20.0f,
	-20.0f,  20.0f, -20.0f,
	-20.0f,  20.0f, -20.0f,
	-20.0f,  20.0f,  20.0f,
	-20.0f, -20.0f,  20.0f,

	20.0f, -20.0f, -20.0f,
	20.0f, -20.0f,  20.0f,
	20.0f,  20.0f,  20.0f,
	20.0f,  20.0f,  20.0f,
	20.0f,  20.0f, -20.0f,
	20.0f, -20.0f, -20.0f,

	-20.0f, -20.0f,  20.0f,
	-20.0f,  20.0f,  20.0f,
	 20.0f,  20.0f,  20.0f,
	 20.0f,  20.0f,  20.0f,
	 20.0f, -20.0f,  20.0f,
	-20.0f, -20.0f,  20.0f,

	-20.0f,  20.0f, -20.0f,
	 20.0f,  20.0f, -20.0f,
	 20.0f,  20.0f,  20.0f,
	 20.0f,  20.0f,  20.0f,
	-20.0f,  20.0f,  20.0f,
	-20.0f,  20.0f, -20.0f,

	-20.0f, -20.0f, -20.0f,
	-20.0f, -20.0f,  20.0f,
	 20.0f, -20.0f, -20.0f,
	 20.0f, -20.0f, -20.0f,
	-20.0f, -20.0f,  20.0f,
	 20.0f, -20.0f,  20.0f
};

// unsigned int loadCubeMap(std::vector<std::string> cubeFaces)
unsigned int loadCubeMap()
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

void generateSkyBoxBufferMesh(SkyBox * skyBox)
{
	unsigned int vp_vbo = 0;
	GLuint shaderProgramID = skyBox->shaderProgramID;
	GLuint loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
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

