#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include "ShaderLoader.h"
#include "mesh.h"
#include "light.h"
#include "view.h"
#include "scene.h"

#define MAX_NUM_TEXTURE 100
#define NUMBER 1
using namespace std;

mesh *object;
view v1;
scene s1;
light *lighting = new light("Peter.light");

float angle = 0.0f;
float zoom = 1.0f;
int light_num = 0;
int program;
int windowSize[2];
int  selected_obj = -1;
double oldx, oldy;
string receiver;
GLhandleARB MyShader;
GLfloat segment_length = 0.5f;
float gravity1 = -0.1f;
int segment_num = 15;

void light_func();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void Mouse(int, int, int, int);
void MotionMouse(int, int);
char *textFileRead(char *fn);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);
void setShaders();
void LoadShaders();

int main(int argc, char** argv)
{

	
	v1.Loadview(v1, NUMBER);//Load view

	glutInit(&argc, argv);
	glutInitWindowSize(v1.viewport_w, v1.viewport_h);
	glutInitWindowPosition(v1.viewport_x, v1.viewport_y);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("0316038_SHADING");

	GLenum glew_error;
	if ((glew_error = glewInit()) != GLEW_OK)return -1;
	LoadShaders();	//Phong shading program
	//prepare Texture:Load Image files
	FreeImage_Initialise();	//Initialize the FreeImage library
	glGenTextures(MAX_NUM_TEXTURE, s1.texObject); //generate texture object
	s1.Loadscene(s1, NUMBER); //Loadscene,LoadTexture
	FreeImage_DeInitialise(); //release FreeImage library
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(MotionMouse);

	setShaders();	//Hair simulation program
	glutMainLoop();

	return 0;
}

void LoadShaders()
{
	MyShader = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "PhongShading.vert", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "PhongShading.frag", GL_FRAGMENT_SHADER);
	}
}

//Read in a textfile (GLSL program)
// we need to pass it as a string to the GLSL driver
char *textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {

		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char)* (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);

		}
	}
	return content;
}

// it prints out shader info (debugging!)
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Shader Info Log: OK\n");
	}
}

// it prints out shader info (debugging!)
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Program Info Log: OK\n");
	}
}

void setShaders()
{
	int vertShaderID, geomShaderID, fragShaderID;
	char *vertSource = NULL, *geomSource = NULL, *fragSource = NULL;

	//First, create our shaders		
	//glCreateShader(type):首先我們要告訴OpenGL我們現在要compile的是vertex shader , geometry shader , 還是fragment shader。
	vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	geomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//Read in the programs : 讀檔案
	vertSource = textFileRead("VertexNormalVisualizer.vert");
	geomSource = textFileRead("VertexNormalVisualizer.geom");
	fragSource = textFileRead("VertexNormalVisualizer.frag");

	//Setup a few constant pointers for below
	const char *vv = vertSource;
	const char *gg = geomSource;
	const char *ff = fragSource;

	//glShaderSource( shader_id, how_many_string_array, string_array, string_array_length );
	//how_many_string_array : 一個shader才用一份程式碼就寫1。
	//再來我們要把剛剛載入的程式碼送進去OpenGL的shader object裡面。
	glShaderSource(vertShaderID, 1, &vv, NULL);
	glShaderSource(geomShaderID, 1, &gg, NULL);
	glShaderSource(fragShaderID, 1, &ff, NULL);

	free(vertSource);
	free(geomSource);
	free(fragSource);

	//編譯shader : 用跟glShaderSource一樣的ID就可以compile那個shader。 
	glCompileShader(vertShaderID);
	glCompileShader(geomShaderID);
	glCompileShader(fragShaderID);

	//創建一個program，算是乘載著各種shader的東西。 
	program = glCreateProgram();
	
	//把你compile好的shader連上program，可以連上數個shader。 
	glAttachShader(program, vertShaderID);	// 把vertex shader跟program連結上 
	glAttachShader(program, geomShaderID);  // 把geometry shader跟program連結上 
	glAttachShader(program, fragShaderID);	// 把fragment shader跟program連結上 

	glLinkProgram(program);					// 根據被連結上的shader, link出各種processor 

	//You can use glUseProgram(program) whenever you want to render something with the program.
	//If you want to render with the fixed pipeline, use glUseProgram(0).

	printShaderInfoLog(vertShaderID);
	printShaderInfoLog(geomShaderID);
	printShaderInfoLog(fragShaderID);
	printProgramInfoLog(program);
}


void light_func()
{
	GLfloat light_specular[2][4];
	GLfloat light_diffuse[2][4];
	GLfloat light_ambient[2][4];
	GLfloat light_position[2][4];
	GLfloat light_environment[3];

	for (int i = 0; i < lighting->number; i++){
		for (int j = 0; j < 3; j++){
			light_specular[i][j] = lighting->specular[i][j];
			light_diffuse[i][j] = lighting->diffuse[i][j];
			light_ambient[i][j] = lighting->ambient[i][j];
			light_position[i][j] = lighting->position[i][j];
		}
		light_specular[i][3] = 1.0f;
		light_diffuse[i][3] = 1.0f;
		light_ambient[i][3] = 1.0f;
		light_position[i][3] = 1.0f;
	}
	for (int i = 0; i < 3; i++){
		light_environment[i] = lighting->environment[i];
	}

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	for (int i = 0; i < lighting->number; i++){
		glEnable(GL_LIGHT0 + i); //每道光給個編號
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position[i]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse[i]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular[i]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient[i]);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_environment); //global ambient

}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glEnable(GL_LINE_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	// viewport transformation
	glViewport(v1.viewport_x, v1.viewport_y, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	//Set current matrix to the 4x4 identity matrix
	gluPerspective(v1.fovy*zoom, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v1.dnear, v1.dfar);	//fovy: view angle, aspect=w/h

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(v1.eye.x, v1.eye.y, v1.eye.z,	 // eye
			  v1.vat.x, v1.vat.y, v1.vat.z,	 // center
			  v1.vup.x, v1.vup.y, v1.vup.z); // up
	glRotatef((GLfloat)angle, 0.0, 1.0, 0.0);//用這個function來達到旋轉的目的
	//注意light位置的設定，要在gluLookAt之後
	light_func();
	
	glUseProgram(MyShader);
	glUniform1i(glGetUniformLocation(MyShader, "lightNumber"), 2);
	GLint location = glGetUniformLocation(MyShader, "colorTexture");
	if (location == -1){	
		printf("Cant find texture name: colorTexture\n");
	}
	else{
		glUniform1i(location, 0);
	}
	
	for (int k = 0; k < 1; k++)          //////////////set each object
	{
		int lastMaterial = -1;
		glPushMatrix();										//put the current matrix into stack (modelview)
		glTranslatef(s1.transfer_vector[k].x, s1.transfer_vector[k].y, s1.transfer_vector[k].z);    //////translate
		glRotatef(s1.angle[k], s1.rotation_axis_vctor[k].x, s1.rotation_axis_vctor[k].y, s1.rotation_axis_vctor[k].z);    //////rotate
		glScalef(s1.scale_value[k].x, s1.scale_value[k].y, s1.scale_value[k].z);    //////scale

		for (size_t i = 0; i < s1.object[k]->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != s1.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s1.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s1.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s1.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s1.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s1.object[k]->mList[lastMaterial].Ns);

				//you can obtain the texture name by object->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			// bind texture
			switch (s1.texture_mode[k])
			{
			case 0: //no-texture
				break;
			case 1: //single-texture
				glActiveTexture(GL_TEXTURE0);
				//glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k]]);
				//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				//glEnable(GL_ALPHA_TEST);
				//glAlphaFunc(GL_GREATER, 0.5f);
				break;

			case 2: //multi-texture
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k] - 1]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);		//color0 * color1
				break;

			case 3: //cube-map
				glActiveTexture(GL_TEXTURE0);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, s1.texObject[s1.texture_ID[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				break;
			}
			// Render models with texture coordinates
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				if (s1.texture_mode[k] == 1 || s1.texture_mode[k] == 3){
					glTexCoord2fv(s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);
				}
				if (s1.texture_mode[k] == 2) {	//Assign texture coordinates for each vertex
					glMultiTexCoord2fv(GL_TEXTURE0, s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);	
					glMultiTexCoord2fv(GL_TEXTURE1, s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);
				}
				glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
			// unbind texture
			switch (s1.texture_mode[k])
			{
			case 0:
				break;
			case 1:
				glActiveTexture(GL_TEXTURE0);
				//glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 2:
				//unbind texture 1
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);

				//unbind texture 2
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 3:
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_S);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				break;
			}
		}
		
		glPopMatrix();
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, "length"), segment_length);
	glUniform1f(glGetUniformLocation(program, "gravity1"), gravity1);
	glUniform1i(glGetUniformLocation(program, "number"), segment_num);
	
	
	for (int k = 1; k < 2; k++)          //////////////set each object
	{
		int lastMaterial = -1;
		glPushMatrix();										//put the current matrix into stack (modelview)
		glTranslatef(s1.transfer_vector[k].x, s1.transfer_vector[k].y, s1.transfer_vector[k].z);    //////translate
		glRotatef(s1.angle[k], s1.rotation_axis_vctor[k].x, s1.rotation_axis_vctor[k].y, s1.rotation_axis_vctor[k].z);    //////rotate
		glScalef(s1.scale_value[k].x, s1.scale_value[k].y, s1.scale_value[k].z);    //////scale

		for (size_t i = 0; i < s1.object[k]->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != s1.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s1.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s1.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s1.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s1.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s1.object[k]->mList[lastMaterial].Ns);

				//you can obtain the texture name by object->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			// bind texture
			switch (s1.texture_mode[k])
			{
			case 0: //no-texture
				break;
			case 1: //single-texture
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				break;

			case 2: //multi-texture
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k] - 1]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, s1.texObject[s1.texture_ID[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);		//color0 * color1
				break;

			case 3: //cube-map
				glActiveTexture(GL_TEXTURE0);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, s1.texObject[s1.texture_ID[k]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				break;
			}
			// Render models with texture coordinates
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				if (s1.texture_mode[k] == 1 || s1.texture_mode[k] == 3){
					glTexCoord2fv(s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);
				}
				if (s1.texture_mode[k] == 2) {	//Assign texture coordinates for each vertex
					glMultiTexCoord2fv(GL_TEXTURE0, s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);
					glMultiTexCoord2fv(GL_TEXTURE1, s1.object[k]->tList[s1.object[k]->faceList[i][j].t].ptr);
				}
				glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
			// unbind texture
			switch (s1.texture_mode[k])
			{
			case 0:
				break;
			case 1:
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 2:
				//unbind texture 1
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);

				//unbind texture 2
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;

			case 3:
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_S);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				break;
			}
		}

		glPopMatrix();
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glUseProgram(0);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}
void keyboard(unsigned char key, int x, int y)
{
	float unit = 1;
	float x_rotate, z_rotate;
	switch (key)
	{
	case 'w':
		zoom = zoom - 0.05f;
		glutPostRedisplay();
		break;
	case 's':
		zoom = zoom + 0.05f;
		glutPostRedisplay();
		break;
	case 'a':
		angle = angle - 5.0f;
		glutPostRedisplay();
		break;
	case 'd':
		angle = angle + 5.0f;
		glutPostRedisplay();
		break;
	case 'r':
		segment_length += 0.1f;
		cout << "segment_length : " << segment_length << endl;
		glutPostRedisplay();
		break;
	case 'f':
		if (segment_length > 0 && segment_length < 0.1f)
			segment_length = 0;
		else if (segment_length == 0)
			segment_length = 0;
		else 
			segment_length -= 0.1f;
		cout << "segment_length : " << segment_length << endl;
		glutPostRedisplay();
		break;
	case 't':
		segment_num += 1;
		cout << "segment_num : " << segment_num << endl;
		glutPostRedisplay();
		break;
	case 'g':
		if (segment_num > 1) segment_num -= 1;
		cout << "segment_num : " << segment_num << endl;
		glutPostRedisplay();
		break;
	case 'y':
		gravity1 += 0.1f;
		cout << "gravity : " << gravity1 << endl;
		glutPostRedisplay();
		break;
	case 'h':
		if (gravity1 > 0 && gravity1 < 0.1f) gravity1 = 0;
		gravity1 -= 0.1f;
		cout << "gravity : " << gravity1 << endl;
		glutPostRedisplay();
		break;
	}
}

int old_x = 0;
int old_y = 0;

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		oldx = x;
		oldy = y;
	}
}
void MotionMouse(int x, int y)
{
	if (selected_obj >= 0 && selected_obj < s1.obj_num)
	{
		s1.transfer_vector[selected_obj].x += (x - oldx)*0.02;
		s1.transfer_vector[selected_obj].y -= (y - oldy)*0.02;
		oldx = x; oldy = y;
		glutPostRedisplay();
	}
}
