#define _USE_MATH_DEFINES
#define M_PI 3.1415926
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack> 
#include <math.h> 

using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else 
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <GL/gl.h> 
#endif

#include<glm/glm.hpp> 
#include<glm/gtx/transform.hpp> 

typedef struct
{
	float location[4];
	float normal[4];
	float color[4];
} Vertex;

GLubyte tindices[6 * 6];

int nindices;
Vertex *cyverts;   // cylinder vertices
GLuint *cindices;

int sindices;
Vertex *sphverts;   // sphere vertices
GLuint *sphindices;

GLuint vboHandle[3];	//Cube vbohandle
GLuint indexVBO[3];

float angle;

glm::mat4 modelM = glm::mat4(1.0f);
glm::mat4 turret = glm::mat4(1.0f);

int treeLoc[10];

GLuint programObject;
GLuint SetupGLSL(char*);

bool WIRE_FRAME = false;

/////////////////////////////////
// glut mouse control 
// 
int xform_mode = 0;
#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2 

int press_x, press_y;
int release_x, release_y;
float z_angle = 0.0;
float x_angle = 0.0;
float scale_size = 0.9;

/////////////////////////////////
// Define Light Properties -  Ia, Id, Is, and light position 
//////////////////////////////////

GLfloat light_ambient[4] = { 0.8,0.8,0.8,1 };  //Ia 
GLfloat light_diffuse[4] = { 0.8,0.8,0.8,1 };  //Id
GLfloat light_specular[4] = { 1,1,1,1 };  //Is
GLfloat light_pos[4] = { 7, 10, 0, 1 };

/////////////////////////////////
// Define Default Material Properties -  Ka, Kd, Ks, Shininess 
//////////////////////////////////

GLfloat mat_ambient[4] = { 0.1,0.1,0.1,1 };  //Ka 
GLfloat mat_diffuse[4] = { 0.8,0.8,0,1 };  //Kd
GLfloat mat_specular[4] = { 1,1,1,1 };  //Ks
GLfloat mat_shine[1] = { 10 };

/////////////////////////////////

class SceneObjects {

public:
	void initCube() {
		float vertices[] = { -0.5, -0.5, -0.5, 1.0,
			0, 0, 1, 0,
			0,0.6,0, 1,
			0.5, -0.5, -0.5, 1.0,
			0, 0, 1, 0,
			0.6,0.6,0, 1,
			0.5, 0.5, -0.5, 1.0,
			0, 0, 1, 0,
			0,0.6,0, 1,
			-0.5, 0.5, -0.5,1.0,
			0, 0, 1, 0,
			0,0.6,0, 1,
			-0.5, -0.5, 0.5, 1.0,
			0, 0, -1, 0,
			0,0.6,0, 1,
			0.5, -0.5, 0.5, 1.0,
			0, 0, -1, 0,
			0,0.6,0, 1,
			0.5, 0.5, 0.5, 1.0,
			0, 0, -1, 0,
			0,0, 0.6, 1,
			-0.5, 0.5, 0.5, 1.0,
			0, 0, -1, 0,
			0,0, 0.6, 1
		};

		tindices[0] = 0;   tindices[1] = 1;   tindices[2] = 2;
		tindices[3] = 0;   tindices[4] = 2;   tindices[5] = 3;

		tindices[6] = 4;   tindices[7] = 5;   tindices[8] = 6;
		tindices[9] = 4;   tindices[10] = 6;   tindices[11] = 7;

		tindices[12] = 0;   tindices[13] = 3;   tindices[14] = 7;
		tindices[15] = 0;   tindices[16] = 7;   tindices[17] = 4;

		tindices[18] = 1;   tindices[19] = 2;   tindices[20] = 6;
		tindices[21] = 1;   tindices[22] = 6;   tindices[23] = 5;

		tindices[24] = 0;   tindices[25] = 1;   tindices[26] = 5;
		tindices[27] = 0;   tindices[28] = 5;   tindices[29] = 4;

		tindices[30] = 3;   tindices[31] = 2;   tindices[32] = 6;
		tindices[33] = 3;   tindices[34] = 6;   tindices[35] = 7;

		//Init Cube
		glGenBuffers(1, &vboHandle[0]);   // create an interleaved VBO object
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);   // bind the first handle 

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 96, vertices, GL_STATIC_DRAW); // allocate space and copy the position data over
		glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

		glGenBuffers(1, &indexVBO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, tindices, GL_STATIC_DRAW);  // load the index data 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
	}

	void initCylinder(int nslices, int nstacks, float r, float g, float b) {
		int nvertices = nslices * nstacks;
		cyverts = new Vertex[nvertices];

		printf(" M PI = %f\n", M_PI);
		float Dangle = 2 * M_PI / (float)(nslices - 1);

		for (int j = 0; j<nstacks; j++)
			for (int i = 0; i<nslices; i++) {
				int idx = j*nslices + i; // mesh[j][i] 
				float angle = Dangle * i;
				cyverts[idx].location[0] = cyverts[idx].normal[0] = cos(angle);
				cyverts[idx].location[1] = cyverts[idx].normal[1] = sin(angle);
				cyverts[idx].location[2] = j*1.0 / (float)(nstacks - 1);
				cyverts[idx].normal[2] = 0.0;
				cyverts[idx].location[3] = 1.0;  cyverts[idx].normal[3] = 0.0;
				cyverts[idx].color[0] = r; cyverts[idx].color[1] = g; cyverts[idx].color[2] = b;
				cyverts[idx].color[3] = 1.0;
			}
		// now create the index array 

		nindices = (nstacks - 1) * 2 * (nslices + 1);
		cindices = new GLuint[nindices];
		int n = 0;
		for (int j = 0; j<nstacks - 1; j++)
			for (int i = 0; i <= nslices; i++) {
				int mi = i % nslices;
				int idx = j*nslices + mi; // mesh[j][mi] 
				int idx2 = (j + 1) * nslices + mi;
				cindices[n++] = idx;
				cindices[n++] = idx2;
			}

		//Init Cylinder

		glGenBuffers(1, &vboHandle[1]);   // create an interleaved VBO object
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);   // bind the first handle 

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*nvertices, cyverts, GL_STATIC_DRAW); // allocate space and copy the position data over
		glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

		glGenBuffers(1, &indexVBO[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*nindices, cindices, GL_STATIC_DRAW);  // load the index data 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
	}

	void initSphere(int nSlices, int nStacks, int radius, float r, float g, float b) {
		
		int nvertices = nSlices * nStacks;
		sphverts = new Vertex[nvertices];

		float const R = 1. / (float)(nStacks - 1);
		float const S = 1. / (float)(nSlices - 1);

		int n = 0;
		for (int r = 0; r < nStacks; r++)
			for (int s = 0; s < nSlices; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
			sphverts[n].location[0] = x* radius;
			sphverts[n].normal[0] = x;
			sphverts[n].location[1] = y * radius;
			sphverts[n].normal[1] = y;
			sphverts[n].location[2] = z * radius;
			sphverts[n].normal[2] = z;
			sphverts[n].location[3] = 1.0;  sphverts[n].normal[3] = 0.0;
			sphverts[n].color[0] = r; sphverts[n].color[1] = g; sphverts[n].color[2] = b;
			sphverts[n].color[3] = 1.0;
			n++;
		}

		sindices = (nStacks) * 4 * (nSlices);
		sphindices = new GLuint[sindices];
		int c = 0;
		for (int r = 0; r < nStacks-1 ; r++) 
			for (int s = 0; s < nSlices-1; s++) {
			sphindices[c++]= r * nSlices + s;
			sphindices[c++] = r * nSlices + (s + 1);
			sphindices[c++] = (r + 1) * nSlices + (s + 1);
			sphindices[c++] = (r + 1) * nSlices + s;
		}

		//Init Sphere

		glGenBuffers(1, &vboHandle[2]);   // create an interleaved VBO object
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);   // bind the first handle 

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*nvertices, sphverts, GL_STATIC_DRAW); // allocate space and copy the position data over
		glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

		glGenBuffers(1, &indexVBO[2]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*sindices, sphindices, GL_STATIC_DRAW);  // load the index data 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
		
	}

	void generateCube(const glm::mat4 &local2clip, const glm::mat4 &local2eye, float* world2eye, float color[3], GLuint c0, GLuint c1,
		GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4)
	{

		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[0]);

		glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, 48, (char*)NULL + 0);  // position 
		glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, 48, (char*)NULL + 32); // color 
		glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, 48, (char*)NULL + 16); // normal 

		glm::mat4 normal_matrix = glm::inverse((glm::mat4)local2eye);
		normal_matrix = glm::transpose(normal_matrix);

		glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip);   // pass the local2clip matrix
		glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye);   // pass the local2eye matrix
		glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
		glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)world2eye);   // pass the local2eye matrix

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
	}

	void generateCylinder(const glm::mat4 &local2clip, const glm::mat4 &local2eye, float* world2eye,
		float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1,
		GLuint m2, GLuint m3, GLuint m4)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[1]);

		glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
		glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
		glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

		glm::mat4 normal_matrix = glm::inverse(local2eye);
		normal_matrix = glm::transpose(normal_matrix);

		glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
		glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
		glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
		glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)world2eye);   // pass the local2eye matrix 

		glDrawElements(GL_TRIANGLE_STRIP, nindices, GL_UNSIGNED_INT, (char*)NULL + 0);

	}


	void generateSphere(const glm::mat4 &local2clip, const glm::mat4 &local2eye, float* world2eye,
		float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1,
		GLuint m2, GLuint m3, GLuint m4)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[2]);

		glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
		glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
		glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

		glm::mat4 normal_matrix = glm::inverse(local2eye);
		normal_matrix = glm::transpose(normal_matrix);

		glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
		glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
		glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
		glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)world2eye);   // pass the local2eye matrix 

		glDrawElements(GL_QUADS, sindices, GL_UNSIGNED_INT, (char*)NULL + 0);

	}
};

SceneObjects scene;

/////////////////////////////////////////////////////////////

#define SetMaterialColor(d, r, g, b, a)  glUniform4f(d, r, g, b, a); 

/////////////////////////////////////////////////////////////

void display()
{

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.6, 1.0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (WIRE_FRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glUseProgram(programObject);

	GLuint c0 = glGetAttribLocation(programObject, "position");
	GLuint c1 = glGetAttribLocation(programObject, "color");
	GLuint c2 = glGetAttribLocation(programObject, "normal");
	GLuint m1 = glGetUniformLocation(programObject, "local2clip");
	GLuint m2 = glGetUniformLocation(programObject, "local2eye");
	GLuint m3 = glGetUniformLocation(programObject, "normal_matrix");
	GLuint m4 = glGetUniformLocation(programObject, "world2eye");

	GLuint Ia = glGetUniformLocation(programObject, "light_ambient");
	GLuint Id = glGetUniformLocation(programObject, "light_diffuse");
	GLuint Is = glGetUniformLocation(programObject, "light_specular");
	GLuint Lpos = glGetUniformLocation(programObject, "light_pos");

	GLuint Ka = glGetUniformLocation(programObject, "mat_ambient");
	GLuint Kd = glGetUniformLocation(programObject, "mat_diffuse");
	GLuint Ks = glGetUniformLocation(programObject, "mat_specular");
	GLuint Shine = glGetUniformLocation(programObject, "mat_shine");

	glUniform4f(Ia, light_ambient[0], light_ambient[1], light_ambient[2], light_ambient[3]);
	glUniform4f(Id, light_diffuse[0], light_diffuse[1], light_diffuse[2], light_diffuse[3]);
	glUniform4f(Is, light_specular[0], light_specular[1], light_specular[2], light_specular[3]);
	glUniform4f(Lpos, light_pos[0], light_pos[1], light_pos[2], light_pos[3]);

	glUniform4f(Ka, mat_ambient[0], mat_ambient[1], mat_ambient[2], mat_ambient[3]);
	glUniform4f(Kd, mat_diffuse[0], mat_diffuse[1], mat_diffuse[2], mat_diffuse[3]);
	glUniform4f(Ks, mat_specular[0], mat_specular[1], mat_specular[2], mat_specular[3]);
	glUniform1f(Shine, mat_shine[0]);

	glEnableVertexAttribArray(c0);
	glEnableVertexAttribArray(c1);
	glEnableVertexAttribArray(c2);

	// define/get the projection matrix 
	glm::mat4 projection = glm::perspective(5.0f, 1.0f, 0.01f, 200.0f);

	// define/get the viewing matrix 
	glm::mat4 view = glm::lookAt(glm::vec3(10.0,5.0,0.0),
		glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, z_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale_size, scale_size, scale_size));

	float color[3];

	glm::mat4 mvp; // model-view-projection matrix
	glm::mat4 mv;  // model-view matrix 

	//Light
	color[0] = 0.2; color[1] = 0.6; color[2] = 0.2;
	glm::mat4 lightM = glm::mat4(1.0f);
	lightM = glm::translate(model, glm::vec3(light_pos[0], light_pos[1], light_pos[2]));
	lightM = glm::scale(lightM, glm::vec3(0.5, 0.5, 0.5));

	mvp = projection *view *lightM;
	mv = view*lightM;

	SetMaterialColor(Kd, 1, 1, 1, 1);

	//Floor
	color[0] = 0.5; color[1] = 0.4; color[2] = 0.3;
	glm::mat4 floorM = glm::mat4(1.0f);
	floorM = glm::translate(model, glm::vec3(0.0, 0.0, -1));
	floorM = glm::scale(floorM, glm::vec3(25.0,0.1,25.0));	

	mvp = projection *view *floorM;
	mv = view*floorM;
	SetMaterialColor(Kd, 0, 0.7, 0, 1);
	scene.generateCube(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	//Trees
	float treeAngle = 4.70;
	for (int c = 0; c < 5; c++) {
		glm::mat4 poleM = glm::mat4(1.0f);
		poleM = model*poleM;

		poleM = glm::translate(poleM, glm::vec3(treeLoc[c], 0.5, treeLoc[c+1]));
		poleM = glm::scale(poleM, glm::vec3(0.4, 4.0, 0.4));
		poleM = glm::rotate(poleM, treeAngle, glm::vec3(1.0f, 0.0f, 0.0f));

		mvp = projection *view * poleM;
		mv = view*poleM;
		SetMaterialColor(Kd, 0.6, 0.4, 0.2, 1);
		scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

		glm::mat4 sphereM = glm::mat4(1.0f);
		sphereM = model*sphereM;
		sphereM = glm::translate(sphereM, glm::vec3(treeLoc[c], 6.0, treeLoc[c+1]));

		mvp = projection *view *sphereM;
		mv = view*sphereM;
		SetMaterialColor(Kd, 0, 0.7, 0, 1);
		scene.generateSphere(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);
	}


	//Tank/////////////////////////////////////////////////////////////

	//Body
	//scene.generateCube();
	color[0] = 0.2; color[1] = 0.6; color[2] = 0.2;
	glm::mat4 upperCube = glm::mat4(1.0f);
	upperCube = model*modelM*upperCube;

	upperCube = glm::translate(upperCube, glm::vec3(1.0, 1.35, 0.0));
	upperCube = glm::scale(upperCube, glm::vec3(1.0, 0.5, 1.0));
	upperCube = glm::rotate(upperCube, angle, glm::vec3(0.0, 1.0, 0.0));

	mvp = projection *view * upperCube;
	mv = view*upperCube;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCube(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);


	//turret = model * modelM *turret;

	glm::mat4 lowerCube = glm::mat4(1.0f);
	lowerCube = model*modelM*lowerCube;
	lowerCube = glm::translate(lowerCube, glm::vec3(1.0, .85, 0.0));
	lowerCube = glm::scale(lowerCube, glm::vec3(2.0, 0.5, 1.5));

	mvp = projection *view * lowerCube;
	mv = view*lowerCube;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCube(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	//Tires
	//scene.generateCylinder();
	glm::mat4 tireFL = glm::mat4(1.0f);
	tireFL = model *modelM * tireFL;
	tireFL = glm::translate(tireFL, glm::vec3(0.4, 0.35, 0.20));
	tireFL = glm::scale(tireFL, glm::vec3(0.3, 0.3, 0.3));

	mvp = projection *view * tireFL;
	mv = view*tireFL;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	glm::mat4 tireFR = glm::mat4(1.0f);
	tireFR = model *modelM * tireFR;
	tireFR = glm::translate(tireFR, glm::vec3(0.4, 0.35, -.50));
	tireFR = glm::scale(tireFR, glm::vec3(0.3, 0.3, 0.3));

	mvp = projection *view * tireFR;
	mv = view*tireFR;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	glm::mat4 tireBL = glm::mat4(1.0f);
	tireBL = model *modelM * tireBL;
	tireBL = glm::translate(tireBL, glm::vec3(1.5, 0.35, 0.20));
	tireBL = glm::scale(tireBL, glm::vec3(0.3, 0.3, 0.3));
	mvp = projection *view * tireBL;
	mv = view*tireBL;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	glm::mat4 tireBR = glm::mat4(1.0f);
	tireBR = model *modelM * tireBR;
	tireBR = glm::translate(tireBR, glm::vec3(1.5, 0.35, -.50));
	tireBR = glm::scale(tireBR, glm::vec3(0.3, 0.3, 0.3));
	mvp = projection *view * tireBR;
	mv = view*tireBR;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	//Turret
	//scene.generateCylinder();
	glm::mat4 turretM = glm::mat4(1.0f);
	turretM = upperCube *turretM;
	turretM = glm::scale(turretM, glm::vec3(0.1, 0.1, 1.5));
	
	mvp = projection *view * turretM;
	mv = view*turretM;
	SetMaterialColor(Kd, 0.3, 0.7, 0.3, 1);
	scene.generateCylinder(mvp, mv, &view[0][0], color, c0, c1, c2, m1, m2, m3, m4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glutSwapBuffers();


}

///////////////////////////////////////////////////////////////

void mymotion(int x, int y)
{
	if (xform_mode == XFORM_ROTATE) {
		z_angle += (x - press_x) / 200.0;
		if (z_angle > 180) z_angle -= 360;
		else if (z_angle <-180) z_angle += 360;
		press_x = x;

		x_angle -= (y - press_y) / 200.0;
		if (x_angle > 180) x_angle -= 360;
		else if (x_angle <-180) x_angle += 360;
		press_y = y;
	}
	else if (xform_mode == XFORM_SCALE) {
		float old_size = scale_size;
		scale_size *= (1 + (y - press_y) / 60.0);
		if (scale_size <0) scale_size = old_size;
		press_y = y;
	}
	glutPostRedisplay();
}
void mymouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		press_x = x; press_y = y;
		if (button == GLUT_LEFT_BUTTON)
			xform_mode = XFORM_ROTATE;
		else if (button == GLUT_RIGHT_BUTTON)
			xform_mode = XFORM_SCALE;
	}
	else if (state == GLUT_UP) {
		xform_mode = XFORM_NONE;
	}
}

///////////////////////////////////////////////////////////////

void mykey(unsigned char key, int x, int y)
{
	//Tank controls
	float d_angle = 10;
	float lTurn_angle = 0.1, rTurn_angle = -0.1;
	if (key == '1') exit(1);
	if (key == 's')
		modelM = glm::translate(modelM, glm::vec3(0.1f, 0.0f, 0.0f));
	if (key == 'w')
		modelM = glm::translate(modelM, glm::vec3(-0.1f, 0.0f, 0.0f));
	if (key == 'a')
		modelM = glm::rotate(modelM, lTurn_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	if (key == 'd')
		modelM = glm::rotate(modelM, rTurn_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	if (key == 'q')
		angle += 0.1;
	if (key == 'e')
		angle -= 0.1;

	//Wire Frame Control
	if (key == 'z') {
		WIRE_FRAME = !WIRE_FRAME;
	}


	//Light Key Bindings
	if (key == '-') {
		mat_shine[0] -= 1;
		if (mat_shine[0] < 1) mat_shine[0] = 1;
	}
	if (key == '=')
		mat_shine[0] += 1;
	if (key == 'k')
		light_pos[0] += 0.5;
	if (key == 'i')
		light_pos[0] -= 0.5;
	if (key == 'u')
		light_pos[1] += 0.5;
	if (key == 'o')
		light_pos[1] -= 0.5;
	if (key == 'j')
		light_pos[2] += 0.5;
	if (key == 'l')
		light_pos[2] -= 0.5;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Branden Tenbrink Lab3");
	glutDisplayFunc(display);
	glutMouseFunc(mymouse);
	glutMotionFunc(mymotion);
	glutKeyboardFunc(mykey);

	glewInit();

	int cySlices = 60, cyStacks = 20;
	int sphSlices = 32, sphStacks = 16;

	scene.initCube();
	scene.initCylinder(cySlices,cyStacks, 1.0f, 0.0f, 1.0f);
	scene.initSphere(sphSlices, sphStacks, 2, 0.0f, 1.0f, 1.0f);

	for (int c = 0; c < 5; c++) {
		treeLoc[c] = rand() % 20 + (-10);
		treeLoc[c + 1] = rand() % 20 + (-10);
	}

	programObject = SetupGLSL("TankScene");
	glutMainLoop();

}