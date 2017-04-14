#include "light.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<cstdlib>
using namespace std;
light::light(const char* light_file){
	number = 0;
	loadlight (light_file);
}
void light::loadlight(const char* file_name){
	fstream fs;
	fs.open(file_name, ios::in);
	if (fs.fail() == true) {
		cout << "fail to open light file";
		return;
	}
	string str, ss, name;
	float f;
	while (fs >> str){
		if (str == "light"){
			number++;
			vector<float> pos;
			for (int j = 0; j < 3; j++){
				fs >> ss;
				f = atof(ss.c_str());
				pos.push_back(f);
			}
			position.push_back(pos);
			vector<float> amb;
			for (int k = 0; k < 3; k++){
				fs >> ss;
				f = atof(ss.c_str());
				amb.push_back(f);
			}
			ambient.push_back(amb);
			vector<float> diff;
			for (int l = 0; l < 3; l++){
				fs >> ss;
				f = atof(ss.c_str());
				diff.push_back(f);
			}
			diffuse.push_back(diff);
			vector<float> spec;
			for (int m = 0; m < 3; m++){
				fs >> ss;
				f = atof(ss.c_str());
				spec.push_back(f);
			}
			specular.push_back(spec);
		}
		else if (str == "ambient"){
			for (int n = 0; n < 3; n++){
				fs >> ss;
				f = atof(ss.c_str());
				environment.push_back(f);
			}
		}
	}
	fs.close();
}
light::~light(){
	position.clear();
	ambient.clear();
	diffuse.clear();
	specular.clear();
	environment.clear();
}