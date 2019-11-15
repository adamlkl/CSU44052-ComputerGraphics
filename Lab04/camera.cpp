#include "camera.h"

void calculateZoom(Camera & camera, GLuint yoffset)
{
	GLfloat zoomLevel = yoffset * 0.1f;
	camera.distanceFromPlayer -= zoomLevel;
}
