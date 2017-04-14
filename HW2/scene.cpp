#include "scene.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

scene::scene(){
	obj_num = 0;
}

scene::~scene(){

}

void scene::Loadscene(scene &scene, int test_scene){
	ifstream scener;
	string str,dir;
	char line[100];

	if (test_scene == 1){
		scener.open("CornellBox\\CornellBox.scene", ifstream::in);
		dir = "CornellBox\\";
	}
	/*else if (test_scene == 2){
		scener.open("TestScene2\\scene2.scene", ifstream::in);
	}*/
	//model obj_file_name Sx Sy Sz Angle Rx Ry Rz Tx Ty Tz
	scener >> str;	//no texture
	while (scener >> str){
		if (str != "model"){
			break;
		}
		scener >> str; //obj_file_name
		str = dir + str;
		object[obj_num] = new mesh(str.c_str()); //transfer string to cahr*
		scener >> scale_value[obj_num].x >> scale_value[obj_num].y >> scale_value[obj_num].z;
		scener >> angle[obj_num];
		scener >> rotation_axis_vctor[obj_num].x >> rotation_axis_vctor[obj_num].y >> rotation_axis_vctor[obj_num].z;
		scener >> transfer_vector[obj_num].x >> transfer_vector[obj_num].y >> transfer_vector[obj_num].z;

		scene.obj_num++;
	}
	cout << obj_num << endl;
	scener.close();
}
