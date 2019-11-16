#include "camera.h"

void move(Camera & camera, int mouse_dx, int mouse_dy)
{
	calculatePitch(camera, mouse_dy);
	calculateAngleAroundPlayer(camera, mouse_dx);
	GLfloat horizontalDistance = calculateHorizontalDistance(camera);
	GLfloat verticalDistance = calculateVerticalDistance(camera);
	calculateCameraPosition(camera, horizontalDistance, verticalDistance);
}

void calculateCameraPosition(Camera &camera, GLfloat horizontalDistance, GLfloat verticalDistance)
{
	GLfloat theta = camera.model.rotation[1] + camera.angleFromPlayer;
	GLfloat offsetX = (GLfloat)(horizontalDistance * sinf(theta * PI / 180.0f));
	GLfloat offsetZ = (GLfloat)(horizontalDistance * cosf(theta * PI / 180.0f));

	camera.Position.v[0] = camera.model.position[0] - offsetX;
	camera.Position.v[1] = camera.model.position[1] + verticalDistance;
	camera.Position.v[2] = camera.model.position[2] - offsetZ;

	camera.yaw = 180 - (camera.model.rotation[1] + camera.angleFromPlayer);
}

GLfloat calculateHorizontalDistance(Camera & camera)
{
	return (GLfloat)(camera.distanceFromPlayer * cosf(camera.pitch * PI / 180.0f));
}

GLfloat calculateVerticalDistance(Camera & camera)
{
	return (GLfloat)(camera.distanceFromPlayer * sinf(camera.pitch * PI / 180.0f));
}

void calculateZoom(Camera & camera, GLuint yoffset)
{
	printf("zoom: %d\n", yoffset);
	GLfloat zoomLevel = yoffset * 0.1f;
	camera.distanceFromPlayer -= zoomLevel;
}

void calculatePitch(Camera & camera, int mouse_dy)
{
	float pitchChange = mouse_dy * 0.1f;
	camera.pitch -= pitchChange;
}

void calculateAngleAroundPlayer(Camera & camera, int mouse_dx)
{
	float angleChange = mouse_dx * 0.3f;
	camera.angleFromPlayer -= angleChange;
}


