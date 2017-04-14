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
		viewer.open("Peter.view", ifstream::in);
	}
	else
		cout << "File not found";
	if (!viewer){//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << endl;
	}
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