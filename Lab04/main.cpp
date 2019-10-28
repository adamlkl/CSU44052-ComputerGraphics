#include <windows.h>
#include <mmsystem.h>
#include <iostream>
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
#include "maths_funcs.h"
#include "Model.h"

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define ABSOL_MESH_NAME "3d_Models/AbsolMega_ColladaMax.dae"
#define WELL_MESH_NAME "3d_Models/well/well3.dae"
#define HILLY_MAP_MESH_NAME "3d_Models/hilly/hilly.dae"


/*----------------------------------------------------------------------------
INITIALIZE VARIABLES 
----------------------------------------------------------------------------*/
int width = 800;
int height = 600;

Model absol, well, terrain; 

float deltaTime; 
DWORD startTime;
DWORD duration = 0;

// mouse 
int mouse_x, mouse_y = -100;
int mouse_dx, mouse_dy = -100; 

// setup variables for model
GLfloat modelTranslationSpeed[], modelRotationSpeed[], modelScalingSpeed[] = { 10.0f, 10.0f, 10.0f };

// setup variables for camera
GLfloat cameraPosition[] = { 0.0f, 0.0f, -10.0f }, cameraRotation[] = { 0.0f, 0.0f, 0.0f }, cameraOrbitRotation[] = { 0.0f, 0.0f, 0.0f };
GLfloat cameraTranslationSpeed[] = { 10.0f, 10.0f, 10.0f }, cameraRotationSpeed[] = { 1.0f, 1.0f, 1.0f };
boolean orbit = false;

/*----------------------------------------------------------------------------
TRANSFORMATION CALCULATION
----------------------------------------------------------------------------*/
mat4 rotate(mat4 modelMatrix, GLfloat rotation[]) {
	mat4 tempMatrix = modelMatrix;
	tempMatrix = rotate_x_deg(tempMatrix, rotation[0]);
	tempMatrix = rotate_y_deg(tempMatrix, rotation[1]);
	tempMatrix = rotate_z_deg(tempMatrix, rotation[2]);
	return tempMatrix;
}

mat4 transformation(Model model) {
	mat4 transformation = identity_mat4();
	transformation = scale(transformation, vec3(model.scaling[0], model.scaling[1], model.scaling[2]));
	transformation = rotate(transformation, model.rotation);
	transformation = translate(transformation, vec3(model.position[0], model.position[1], model.position[2]));
	return transformation;
}

void copy_array(GLfloat matA[3], GLfloat matB[3]) {
	for (int i = 0; i < 3; i++)
		matA[i] = matB[i];
}

/*----------------------------------------------------------------------------
MODEL, VIEW, PROJECTION CALCULATION
----------------------------------------------------------------------------*/


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
	glutSwapBuffers();
}

void init() {
	GLuint shaderProgramID = CompileShaders("simpleVertexShader.txt", "simpleFragmentShader.txt");
	
	absol.mesh = load_mesh(ABSOL_MESH_NAME);
	absol.shaderProgramID = shaderProgramID;
	generateObjectBufferMesh(&absol);

	well.mesh = load_mesh(WELL_MESH_NAME);
	well.shaderProgramID = shaderProgramID;
	generateObjectBufferMesh(&well);

	terrain.mesh = load_mesh(HILLY_MAP_MESH_NAME);
	terrain.shaderProgramID = shaderProgramID;
	generateObjectBufferMesh(&terrain);

	startTime = timeGetTime();

	absol.position[1] -= 5;
	absol.scaling[0], absol.scaling[1], absol.scaling[2] *= 0.03; 
	absol.rotation[0] += 90;

	terrain.scaling[0], terrain.scaling[1], terrain.scaling[2] *= 1;
	terrain.position[1] -= 10;

	well.scaling[0], well.scaling[1], well.scaling[2] *= 1;
	well.position[0] = 10, well.position[1] -= 5, well.position[2] = 10;

	cameraPosition[1] -= 5.0f;
	cameraPosition[2] += 20.0f;
	cameraRotation[1] += 180.f;
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

#pragma region USER_INPUT_FUNCTIONS
void keypress(unsigned char key, int x, int y) {
	if (key = 'a') absol.position[0] += modelTranslationSpeed[0] * deltaTime;
	if (key = 'd') absol.position[0] -= modelTranslationSpeed[0] * deltaTime;
	if (key = 's') absol.position[2] += modelTranslationSpeed[2] * deltaTime;
	if (key = 'w') absol.position[2] += modelTranslationSpeed[2] * deltaTime;
	if (key = 'x') orbit = !orbit;
	
	// Draw the next frame
	glutPostRedisplay();
}

void specialKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cameraPosition[0] -= cameraTranslationSpeed[0] * deltaTime;
		break;
	case GLUT_KEY_RIGHT:
		cameraPosition[0] += cameraTranslationSpeed[0] * deltaTime;
		break;
	case GLUT_KEY_UP:
		cameraPosition[2] += cameraTranslationSpeed[2] * deltaTime;
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

}

void mouse 
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

