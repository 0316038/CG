#include "mesh.h"
#include "glut.h"
#include "view.h"
#include "scene.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#define NUMBER 1
using namespace std;

mesh *object;
view v1;
scene s1;

int windowSize[2];
int  selected_obj = -1;
double oldx, oldy;
string receiver;
float reflectance = 0.5f;
float transmittance = 0.5f;

void light();
void display();
void DrawScene();
void DrawBoxAndSitBear();
void DrawSitBear();
void DrawStandBear();
void DrawMirror();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void Mouse(int, int, int, int);
void MotionMouse(int, int);

int main(int argc, char** argv)
{
	//Load scene
	s1.Loadscene(s1, NUMBER);
	//Load view
	v1.Loadview(v1, NUMBER);
		
	
	glutInit(&argc, argv);
	glutInitWindowSize(v1.viewport_w, v1.viewport_h);	
	glutInitWindowPosition(v1.viewport_x, v1.viewport_y);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("0316038");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);     
	glutMouseFunc(Mouse);    
	glutMotionFunc(MotionMouse);     
	glutMainLoop();

	return 0;
}

void light()
{
	
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	
	// variables
	int light_num = 0;
	float temp_x, temp_y, temp_z;
	string str;	
	ifstream lighter;
	GLenum gl_light[8] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

	// Load light
	if (NUMBER == 1){
		lighter.open("CornellBox\\CornellBox.light", ifstream::in);
	}

	// light x y z ar ag ab dr dg db sr sg sb: define the light information.
	while (lighter >> str && str == "light" && light_num < 8){
		lighter >> temp_x >> temp_y >> temp_z;	//x,y,z
		GLfloat light_position[] = { temp_x, temp_y, temp_z, 1.0f };

		lighter >> temp_x >> temp_y >> temp_z;	//ar ag ab
		GLfloat light_ambient[] = { temp_x, temp_y, temp_z, 1.0f };

		lighter >> temp_x >> temp_y >> temp_z;	//dr dg db
		GLfloat light_diffuse[] = { temp_x, temp_y, temp_z, 1.0f };

		lighter >> temp_x >> temp_y >> temp_z;	//sr sg sb
		GLfloat light_specular[] = { temp_x, temp_y, temp_z, 1.0f };

		// set light property
		glEnable(gl_light[light_num]);
		glLightfv(gl_light[light_num], GL_POSITION, light_position);
		glLightfv(gl_light[light_num], GL_DIFFUSE, light_diffuse);
		glLightfv(gl_light[light_num], GL_SPECULAR, light_specular);
		glLightfv(gl_light[light_num], GL_AMBIENT, light_ambient);

		light_num++;
	}
	//ambient r g b: define the ambient light from environment.
	lighter >> temp_x >> temp_y >> temp_z;	
	GLfloat ambient[] = { temp_x, temp_y, temp_z, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	lighter.close();
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // 清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glClearStencil(1.0f);						   // stencil 一開始設為1
	glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT); //這行把畫面清成黑色並且清除z buffer
	
	glEnable(GL_CULL_FACE);                    // enable back face culling
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glEnable(GL_STENCIL_TEST);
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	
	// viewport transformation
	glViewport(v1.viewport_x, v1.viewport_y, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	//Set current matrix to the 4x4 identity matrix
	gluPerspective(v1.fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v1.dnear, v1.dfar);	//fovy: view angle, aspect=w/h

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	v1.eye.x, v1.eye.y, v1.eye.z,	// eye
				v1.vat.x, v1.vat.y, v1.vat.z,	// center
				v1.vup.x, v1.vup.y, v1.vup.z);  // up
	glFrontFace(GL_CCW);	/////要設定逆時針為正面/////
	//注意light位置的設定，要在gluLookAt之後
	light();

	//Draw Stand Bear
	glStencilFunc(GL_ALWAYS, 1, 1);	//一開始站著的熊讓他ALWAYS通過stencil test		//void glStencilFunc(GLenum func, GLint ref, GLuint mask); ( ref & mask ) ( stencil & mask ). 
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);	
	DrawStandBear();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//void glBlendFunc(GLenum sfactor, GLenum dfactor); //Color = source_color * sfactor + destination_color * dfactor 

	//Draw Mirror
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO); //鏡子stencil值改0
	DrawMirror();

	glAccum(GL_LOAD, transmittance);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);

	////////Reflection///////////
	glClear(GL_DEPTH_BUFFER_BIT);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	//Set current matrix to the 4x4 identity matrix
	gluPerspective(v1.fovy, -(GLfloat)(windowSize[0]) / (GLfloat)windowSize[1], v1.dnear, v1.dfar);	//fovy: view angle, aspect=w/h

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(v1.eye.x - 2 * (v1.eye.x + 20), v1.eye.y, v1.eye.z,	// eye	//把camera的x座標移到mirror對面 //鏡子的X座標是-20，所以用eye[x]-(-20) 就是距離，然後減兩倍就會到對面
		v1.vat.x - 2 * (v1.vat.x+20), v1.vat.y, v1.vat.z,	// center
		v1.vup.x, v1.vup.y, v1.vup.z);  // up
	glFrontFace(GL_CW);//要設定順時針方向為正面

	glStencilFunc(GL_EQUAL, 0, 1);	//	只畫stencil = 0 的地方	//  ( 0 & 1 ) = ( stencil & 1 )
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	DrawBoxAndSitBear();

	glAccum(GL_ACCUM, reflectance);
	glClear(GL_COLOR_BUFFER_BIT);
	glAccum(GL_RETURN, 1.0f);

	//draw sit bear head : 畫坐著的熊在鏡子範圍的頭的那個部分
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	//Set current matrix to the 4x4 identity matrix
	gluPerspective(v1.fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v1.dnear, v1.dfar);	//fovy: view angle, aspect=w/h

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(v1.eye.x , v1.eye.y, v1.eye.z,	// eye	//把camera的x座標移到mirror對面 //鏡子的X座標是-20，所以用eye[x]-(-20) 就是距離，然後減兩倍就會到對面
		v1.vat.x , v1.vat.y, v1.vat.z,	// center
		v1.vup.x, v1.vup.y, v1.vup.z);  // up
	glFrontFace(GL_CCW);//要設定逆時針方向為正面
	light();

	glStencilFunc(GL_NOTEQUAL, 1, 1);	//只畫stencil=0的地方 //畫坐著的熊在鏡子裡的頭的那個部分
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);	//void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass); 
	DrawSitBear();

	//Draw Scene
	glStencilFunc(GL_EQUAL, 1, 1);	//只畫stencil=1的部分(不在鏡子範圍內)
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	DrawScene();

	glutSwapBuffers();
}


void DrawScene()
{
	for (int k = 0; k < (s1.obj_num) - 1; k++)          //////////////set each object
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

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void DrawBoxAndSitBear()
{
	for (int k = 0; k < 2; k++)          //////////////set each object
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

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		glPopMatrix();
	}

}

void DrawSitBear()
{
	int k = 1;         //////////////set each object

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

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j<3; ++j)
		{
			glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
			glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
}

void DrawStandBear()
{
	int k = 2;         //////////////set each object

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

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j<3; ++j)
		{
			glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
			glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
}

void DrawMirror()
{
	int k = 3;         //////////////set each object

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

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glNormal3fv(s1.object[k]->nList[s1.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s1.object[k]->vList[s1.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		glPopMatrix();
	
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
	float x_rotate, y_rotate;
	case 'w':
		v1.fovy -= 1;  //camera角度變小: Zoom in
		glutPostRedisplay();
		break;
	case 's':
		v1.fovy += 1;	//camera角度變大: Zoom out
		glutPostRedisplay();
		break;
	case 'a':
		x_rotate = v1.eye.x - v1.vat.x;
		z_rotate = v1.eye.z - v1.vat.z;
		v1.eye.x = x_rotate*cos(-0.1) + z_rotate*sin(-0.1);
		v1.eye.z = z_rotate*cos(-0.1) - x_rotate*sin(-0.1); 
		glutPostRedisplay();
		break;
	case 'd':
		x_rotate = v1.eye.x - v1.vat.x;
		z_rotate = v1.eye.z - v1.vat.z;
		v1.eye.x = x_rotate*cos(0.1) + z_rotate*sin(0.1);
		v1.eye.z = z_rotate*cos(0.1) - x_rotate*sin(0.1);
		glutPostRedisplay();
		break;
	case 'r':
		if (reflectance < 1.0f) reflectance += 0.1f;
		//cout << "reflectance:" << reflectance << endl;
		glutPostRedisplay();
		break;
	case 'f':
		
		if (reflectance <= 0) 
				reflectance = 0;
		else reflectance -= 0.1;
		//cout << "reflectance:" << reflectance << endl;
		glutPostRedisplay();
		break;
	case 't':
		if (transmittance < 1.0f) transmittance += 0.1f;
		//cout << "transmittance:" << transmittance << endl;
		glutPostRedisplay();
		break;
	case 'g':
		if (transmittance <= 0.0f) 
			transmittance = 0.0f;
		else transmittance -= 0.1f;
		//cout << "transmittance:" << transmittance << endl;
		glutPostRedisplay();
		break;
	}
}

int old_x = 0;
int old_y = 0;

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN )
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
