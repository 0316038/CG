#include "view.h"
#include <iostream>
#include <fstream>
#include <string>

view::view()
{

}

view::~view()
{

}

void view::Loadview(view &view, int test_scene)
{
	ifstream viewer;
	string receiver;
	if (test_scene == 1){
		viewer.open("CornellBox\\CornellBox.view", ifstream::in);
	}
	/*else if (test_scene == 2){
		viewer.open("TestScene2\\scene2.view", ifstream::in);
	}*/
	viewer >> receiver;
	viewer >> eye.x >> eye.y >> eye.z;

	viewer >> receiver;
	viewer >> vat.x >> vat.y >> vat.z;

	viewer >> receiver;
	viewer >> vup.x >> vup.y >> vup.z;

	viewer >> receiver >> fovy >> receiver >> dnear >> receiver >> dfar;
	viewer >> receiver >> viewport_x >> viewport_y >> viewport_w >> viewport_h;
	viewer.close();
}