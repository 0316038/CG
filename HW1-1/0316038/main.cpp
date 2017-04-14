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

void light();
void display();
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
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
		lighter.open("TestScene1\\light.light", ifstream::in);
	}
	else if (NUMBER == 2){
		lighter.open("TestScene2\\scene2.light", ifstream::in);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

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

	//注意light位置的設定，要在gluLookAt之後
	light();

	for (int k = 0; k < s1.obj_num; k++)          //////////////set each object
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
	case '1':
		if (s1.obj_num >= 1){
			selected_obj = 0;
			cout << "select object 1" << endl;
		}
		else
			cout << "no object 1" << endl;
		break;
	case '2':
		if (s1.obj_num >= 2){
			selected_obj = 1;
			cout << "select object 2" << endl;
		}
		else
			cout << "no object 2" << endl;
		break;
	case '3':
		if (s1.obj_num >= 3){
			selected_obj = 2;
			cout << "select object 3" << endl;
		}
		else
			cout << "no object 3" << endl;
		break;
	case '4':
		if (s1.obj_num >= 4){
			selected_obj = 3;
			cout << "select object 4" << endl;
		}
		else
			cout << "no object 4" << endl;
		break;
	case '5':
		if (s1.obj_num >= 5){
			selected_obj = 4;
			cout << "select object 5" << endl;
		}
		else
			cout << "no object 5" << endl;
		break;
	case '6':
		if (s1.obj_num >= 6){
			selected_obj = 5;
			cout << "select object 6" << endl;
		}
		else
			cout << "no object 6" << endl;
		break;
	case '7':
		if (s1.obj_num >= 7){
			selected_obj = 6;
			cout << "select object 7" << endl;
		}
		else
			cout << "no object 7" << endl;
		break;
	case '8':
		if (s1.obj_num >= 8){
			selected_obj = 7;
			cout << "select object 8" << endl;
		}
		else
			cout << "no object 8" << endl;
		break;
	case '9':
		if (s1.obj_num >= 9){
			selected_obj = 8;
			cout << "select object 9" << endl;
		}
		else
			cout << "no object 9" << endl;
		break;
	default:
		selected_obj = -1;
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
