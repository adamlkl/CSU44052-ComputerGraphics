#ifndef MODEL_TEXTURE_HEADER
#define MODEL_TEXTURE_HEADER

#include <vector>

struct ModelTexture {
	unsigned int textureID;
	std::string type;
	std::string path;
	float shineDamper = 1;
	float reflectivity = 0;
};
#endif