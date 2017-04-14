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
	vertex scale_value[100];  //Sx Sy Sz
	float angle[100];
	vertex rotation_axis_vctor[100];  //Rx Ry Rz
	vertex transfer_vector[100];  //Tx Ty Tz
	int obj_num;
	int texture_mode[100];
	int texture_ID[100];
	unsigned int texObject[100];
	mesh* object[100];

	scene();
	~scene();
	void Loadscene(scene &scene, int test_scene);
	void LoadTexture(string Filename, int index);
	void LoadCubeTexture(string Filename[6], int index);
};