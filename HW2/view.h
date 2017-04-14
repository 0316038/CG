#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class view{
public:
	struct vertex{
		float x, y, z;
	};
	vertex eye;  // position of camera
	vertex	vat;  // position where camera look at
	vertex	vup;  // vector of camera
	int		fovy;	// field of vision in angle
	int		dnear;	// near plane
	int		dfar;	// far plane
	int		viewport_x, viewport_y;	// position of viewport
	int		viewport_w, viewport_h;	// size of view port
	view();
	~view();
	void Loadview(view &view, int test_scene);
};