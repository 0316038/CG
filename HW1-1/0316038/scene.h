#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class scene{
public:
	struct vertex{
		float x, y, z;	
	};
	//model obj_file_name Sx Sy Sz Angle Rx Ry Rz Tx Ty Tz
	vertex scale_value[10];  //Sx Sy Sz
	float angle[10];
	vertex rotation_axis_vctor[10];  //Rx Ry Rz
	vertex transfer_vector[10];  //Tx Ty Tz
	int obj_num;
	mesh* object[10];

	scene();
	~scene();
	void Loadscene(scene &scene, int test_scene);
};