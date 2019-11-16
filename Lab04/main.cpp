#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <limits.h>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "camera.h"
#include "maths_funcs.h"
#include "model.h"
#include "renderer.h"
#include "utility.h"

#define PI 3.14159265

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define ABSOL_MESH_NAME "3d_Models/AbsolMega_ColladaMax.dae"
#define WELL_MESH_NAME "3d_Models/well/well3.dae"
#define HILLY_MAP_MESH_NAME "3d_Models/hilly/hilly.dae"
#define MODEL_MESH_NAME "3d_Models/model/model.dae"


/*----------------------------------------------------------------------------
TEXTURE TO LOAD
----------------------------------------------------------------------------*/
#define MODEL_TEXTURE_NAME "3d_Models/model/diffuse.png"
#define SKYBOX_TEXTURE_NAME_RIGHT "SkyBoxImages/right.png",
#define SKYBOX_TEXTURE_NAME_LEFT "SkyBoxImages/left.png",
#define SKYBOX_TEXTURE_NAME_TOP "SkyBoxImages/top.png",
#define SKYBOX_TEXTURE_NAME_BOTTOM "SkyBoxImages/bottom.png",
#define SKYBOX_TEXTURE_NAME_FRONT "SkyBoxImages/front.png",
#define SKYBOX_TEXTURE_NAME_BACK "SkyBoxImages/back.png"

/*----------------------------------------------------------------------------
Vertex Shader Files
----------------------------------------------------------------------------*/
#define SKYBOX_VERTEX_SHADER_FILE "SkyBoxVertexShader.txt"


/*----------------------------------------------------------------------------
Fragment Shader Files
----------------------------------------------------------------------------*/
#define SKYBOX_FRAGMENT_SHADER_FILE "SkyBoxFragmentShader.txt"


/*----------------------------------------------------------------------------
INITIALIZE VARIABLES 
----------------------------------------------------------------------------*/
int width = 800;
int height = 600;
const GLfloat RUN_SPEED = 20.0f;
const GLfloat TURN_SPEED = 80.0f;

Model absol;
Model well;
Model terrain; 

Camera camera;
SkyBox skyBox;

float deltaTime; 
DWORD startTime;
DWORD duration = 0;

// mouse 
int mouse_x = 0, mouse_y = 0;
int mouse_dx = -100, mouse_dy = -100;

// setup variables for model
GLfloat modelTranslationSpeed[] = { 10.0f, 10.0f, 10.0f };
GLfloat modelRotationSpeed[] = { 10.0f, 10.0f, 10.0f };
GLfloat modelScalingSpeed[] = { 10.0f, 10.0f, 10.0f };

GLfloat currentSpeed = 0.0f;
GLfloat currentTurnSpeed = 0.0f;

// setup variables for camera
GLfloat cameraPosition[] = { 0.0f, 0.0f, -10.0f }, cameraRotation[] = { 0.0f, 0.0f, 0.0f }, cameraOrbitRotation[] = { 0.0f, 0.0f, 0.0f };
GLfloat cameraTranslationSpeed[] = { 10.0f, 10.0f, 10.0f }, cameraRotationSpeed[] = { 1.0f, 1.0f, 1.0f };
boolean orbit = false;

/*----------------------------------------------------------------------------
MODEL, VIEW, PROJECTION CALCULATION
----------------------------------------------------------------------------*/
mat4 setupCamera(Model pivot) {
	mat4 pivotTransformation = applyTransformation(pivot);
	mat4 view = identity_mat4();
	view = rotate(view, cameraOrbitRotation);
	view = translate(view, vec3(cameraPosition[0], cameraPosition[1], cameraPosition[2]));
	view = rotate(view, cameraRotation);
	view = translate(view, vec3(pivot.position[0], -pivot.position[1], pivot.position[2]));

	return view;
}

//Setup perspective projection
mat4 setupProjection() {
	mat4 persp_proj = perspective(75.0f, (float)width / (float)height, 0.1f, 1000.0f);
	return persp_proj;
}

/*----------------------------------------------------------------------------
GAME OPERATION 
----------------------------------------------------------------------------*/
void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// mvp setup and rendering
	mat4 view = setupCamera(absol);
	mat4 projection = setupProjection();
    
	renderSkyBox(skyBox, view, projection);

	mat4 model = render(absol, view, projection);
	mat4 wellModel = render(well, view, projection);
	mat4 terrainModel = render(terrain, view, projection);

	glutSwapBuffers();
}

void updateScene() {
	//Update Delta time
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	deltaTime = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	duration = (timeGetTime() - startTime) / 1000.0f;
	glutPostRedisplay();
}

void init() {
	GLuint shaderProgramID = CompileShaders("simpleVertexShader.txt", "simpleFragmentShader.txt");
	
	absol.mesh = load_mesh(MODEL_MESH_NAME);
	absol.shaderProgramID = shaderProgramID;
	//glGenTextures(1, &absol.textureID);
	generateObjectBufferMesh(&absol);
	// loadTextures(&absol, MODEL_TEXTURE_NAME, GL_TEXTURE0, "modelTexture", 0);

	startTime = timeGetTime();

	absol.position[2] -= 20; absol.position[1] = -20;
	absol.scaling[0] *= 1, absol.scaling[1] *= 1, absol.scaling[2] *= 1;

	well.mesh = load_mesh(WELL_MESH_NAME);
	well.shaderProgramID = shaderProgramID;
	generateObjectBufferMesh(&well);

	terrain.mesh = load_mesh(HILLY_MAP_MESH_NAME);
	terrain.shaderProgramID = shaderProgramID;
	generateObjectBufferMesh(&terrain);

	terrain.scaling[0] *= 2, terrain.scaling[1] *= 1.5, terrain.scaling[2] *= 2;
	terrain.position[1] -= 20.0f;

	well.scaling[0] *= 0.5, well.scaling[1] *= 0.5, well.scaling[2] *= 0.5;
	well.position[0] -= 10, well.position[1] -= 19, well.position[2] = 10;

	skyBox.textureID = loadCubeMap();
	GLuint skyShaderId = CompileShaders(SKYBOX_VERTEX_SHADER_FILE, SKYBOX_FRAGMENT_SHADER_FILE);
	skyBox.shaderProgramID = skyShaderId;
	generateSkyBoxBufferMesh(&skyBox);

	camera.model = absol; 

	cameraPosition[1] -= 5.0f;
	cameraPosition[2] += 20.0f;
	cameraRotation[1] += 180.f;
} 

/*----------------------------------------------------------------------------
USER INTERACTION FUNCTIONS
----------------------------------------------------------------------------*/
#pragma region USER_INPUT_FUNCTIONS
void keypress(unsigned char key, int x, int y) {
	if (key == 'a') {
		// absol.position[0] += modelTranslationSpeed[0] * deltaTime;
		// absol.rotation[1] = 90;
		currentTurnSpeed = TURN_SPEED;
		
	}
	else if (key == 'd') {
		// absol.position[0] -= modelTranslationSpeed[0] * deltaTime;
		// absol.rotation[1] = -90;
		currentTurnSpeed = -TURN_SPEED;
	}
	else {
		currentTurnSpeed = 0.0f;
	}

	if (key == 'w') {
		// absol.position[2] += modelTranslationSpeed[2] * deltaTime;
		// absol.rotation[1] = 0;
		currentSpeed = RUN_SPEED;
	}
	else if (key == 's') {
		// absol.position[2] -= modelTranslationSpeed[2] * deltaTime;
		// absol.rotation[1] = 180;
		currentSpeed = - RUN_SPEED;
	}
	else {
		currentSpeed = 0.0f;
	}
	if (key == 'x') orbit = !orbit;

	absol.rotation[1] += currentTurnSpeed * deltaTime;
	GLfloat distance = currentSpeed * deltaTime;
	GLfloat dx = (GLfloat)(distance * sinf(absol.rotation[1] * PI/ 180.0f));
	GLfloat dz = (GLfloat)(distance * cosf(absol.rotation[1] * PI / 180.0f));
	absol.position[0] += dx;
	absol.position[2] += dz;

	// Draw the next frame
	glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cameraPosition[0] -= cameraTranslationSpeed[0] * deltaTime;
		// cameraRotation[1] += cameraRotationSpeed[1] * deltaTime;
		// cameraRotation[1] = fmodf(cameraRotation[1], 360.0f);
		break;
	case GLUT_KEY_RIGHT:
		cameraPosition[0] += cameraTranslationSpeed[0] * deltaTime;
		break;
	case GLUT_KEY_UP:
		cameraPosition[2] -= cameraTranslationSpeed[2] * deltaTime;
		break;
	case GLUT_KEY_DOWN:
		cameraPosition[2] += cameraTranslationSpeed[2] * deltaTime;
		break;
	default:
		break;
	}
	// Draw the next frame
	glutPostRedisplay();
}

void mouseMotion(int x, int y) {
	if (mouse_x != -100) mouse_dx = mouse_x - x;
    if (mouse_y != -100) mouse_dy = mouse_y - y;

	mouse_x = x;
	mouse_y = y;

	GLfloat* last_camera_rotation;

	if (orbit) last_camera_rotation = cameraOrbitRotation;
	else last_camera_rotation = cameraRotation;

    printf("dx: %d, res: %f", mouse_dx, mouse_dx * cameraRotationSpeed[1] * deltaTime);
	last_camera_rotation[1] -= mouse_dx * cameraRotationSpeed[1] * deltaTime;
	// last_camera_rotation[0] -= mouse_dy * cameraRotationSpeed[0] * deltaTime;
	
	// Draw the next frame
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse_x = x;
			mouse_y = y;
		}
		else {
			mouse_x = -100;
			mouse_y = -100;
		}
	}
}

void passiveMouseMotion(int x, int y) {
	mouse_x = x;
	mouse_y = y;
}

#pragma endregion KEYBOARD_INPUT_FUNCTIONS

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(specialKeyboard);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(passiveMouseMotion);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}

