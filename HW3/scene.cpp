#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include "scene.h"

scene::scene(){
	obj_num = 0;
}

scene::~scene(){

}

void scene::Loadscene(scene &scene, int test_scene){
	ifstream scener;
	string str, dir, bmp, cube[6];
	int mode = 0, ID = 0;

	if (test_scene == 1){
		scener.open("Peter.scene", ifstream::in);
		dir = "";
	}
	

	//model obj_file_name Sx Sy Sz Angle Rx Ry Rz Tx Ty Tz
	while (scener >> str){		
		if (str == "model"){
			scener >> str; //obj_file_name
			str = dir + str;
			object[obj_num] = new mesh(str.c_str()); //transfer string to cahr*
			scener >> scale_value[obj_num].x >> scale_value[obj_num].y >> scale_value[obj_num].z;
			scener >> angle[obj_num];
			scener >> rotation_axis_vctor[obj_num].x >> rotation_axis_vctor[obj_num].y >> rotation_axis_vctor[obj_num].z;
			scener >> transfer_vector[obj_num].x >> transfer_vector[obj_num].y >> transfer_vector[obj_num].z;

			texture_mode[obj_num] = mode;
			texture_ID[obj_num] = ID - 1;
			obj_num++;
		}
		else if (str == "no-texture"){
			mode = 0;
		}
		else if (str == "single-texture"){
			mode = 1;
			scener >> bmp;//texture_name.bmp 
			bmp = dir + bmp;

			LoadTexture(bmp, ID);
			ID++;
		}
		else if (str == "multi-texture"){
			mode = 2;
			for (int i = 0; i < 2; i++){
				scener >> bmp;
				bmp = dir + bmp;
				LoadTexture(bmp, ID);
				ID++;
			}
		}
		else if (str == "cube-map"){
			mode = 3;
			for (int i = 0; i < 6; i++){
				scener >> cube[i];
				cube[i] = dir + cube[i];
			}
			LoadCubeTexture(cube, ID);
			ID++;
		}
		
	}
	cout << "Load Complete" << endl;
	scener.close();
}

void scene::LoadTexture(string Filename, int index){

	const char* pFilename;
	pFilename = Filename.c_str();	//string to const char*

	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);	//Converts a bitmap to 32 bits (BGRA 8 bits per channel)
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texObject[index]);	//Bind texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, iWidth, iHeight, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);	//Build mipmaps
	FreeImage_Unload(p32BitsImage);	//Deletes a previously loaded FIBITMAP from memory
	FreeImage_Unload(pImage);
}

void scene::LoadCubeTexture(string Filename[6], int index){

	char* pFilename[6];
	for (int i = 0; i<6; i++)
		pFilename[i] = (char*)Filename[i].c_str();

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[index]);
	for (int i = 0; i<6; i++){
		FIBITMAP *pImage = FreeImage_Load(FreeImage_GetFileType(pFilename[i], 0), pFilename[i]);
		FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
		int iWidth = FreeImage_GetWidth(pImage);
		int iHeight = FreeImage_GetHeight(pImage);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);	//Build mipmaps
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 4, iWidth, iHeight, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
		
		FreeImage_Unload(p32BitsImage);
		FreeImage_Unload(pImage);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}
