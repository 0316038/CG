#include<iostream>
#include<vector>
using namespace std;
class light{
public:
	light(const char* light_file);
	void loadlight(const char* file_name);
	~light();
	vector<vector<float>> position;
	vector<vector<float>> ambient;
	vector<vector<float>> diffuse;
	vector<vector<float>> specular;
	vector<float> environment;
	int number;
};

