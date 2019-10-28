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
#include "maths_funcs.h"
#include "model.h"

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

Model absol;
Model well;
Model terrain; 

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

mat4 applyTransformation(Model model) {
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
RENDERING
----------------------------------------------------------------------------*/
mat4 render(Model model, mat4 view, mat4 projection) {
	ModelData mesh_data = model.mesh;
	GLuint shaderProgramID = model.shaderProgramID;
	glUseProgram(shaderProgramID);
	glBindVertexArray(model.mesh_vao);

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

	absol.position[2] -= 20; absol.position[1] -= 2;
	absol.scaling[0] *= 0.03, absol.scaling[1] *= 0.03, absol.scaling[2] *= 0.03;

	terrain.scaling[0] *= 2, terrain.scaling[1] *= 1.5, terrain.scaling[2] *= 2;
	terrain.position[1] -= 10;

	well.scaling[0] *= 0.5, well.scaling[1] *= 0.5, well.scaling[2] *= 0.5;
	well.position[0] -= 10, well.position[1] -= 9, well.position[2] = 10;

	cameraPosition[1] -= 5.0f;
	cameraPosition[2] += 20.0f;
	cameraRotation[1] += 180.f;
} 

/*----------------------------------------------------------------------------
USER INTERACTION FUNCTIONS
----------------------------------------------------------------------------*/
#pragma region USER_INPUT_FUNCTIONS
void keypress(unsigned char key, int x, int y) {
	if (key = 'a') absol.position[0] += modelTranslationSpeed[0] * deltaTime;
	if (key = 'd') absol.position[0] -= modelTranslationSpeed[0] * deltaTime;
	if (key = 'w') absol.position[2] += modelTranslationSpeed[2] * deltaTime;
	if (key = 's') absol.position[2] -= modelTranslationSpeed[2] * deltaTime;
	if (key = 'x') orbit = !orbit;

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
//mouseScroll
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

