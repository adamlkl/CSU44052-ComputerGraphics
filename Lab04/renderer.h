#ifndef RENDERER_HEADER
#define RENDERER_HEADER

#include "maths_funcs.h"
#include "model.h"
#include "skybox.h"

mat4 render(Model model, const char* file_name, mat4 view, mat4 projection, DWORD currentAnimationTime, vec3 cameraPosition);
void renderSkyBox(SkyBox skyBox, mat4 view, mat4 projection);
#endif