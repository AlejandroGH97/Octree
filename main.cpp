#include "Octree.h"
#include <string>
#include <vector>
using namespace std;
using namespace cimg_library;

vector<double> vectorize(CImg<double> &img, int m) {
	CImg<double> haar = img.haar(false, m);
	CImg<double> crop = haar.crop(0, 0, 32, 32);
	
	vector<double> result;

	cimg_forXY(crop, x, y) { 
		result.push_back((crop(x, y, 0) + crop(x, y, 1) + crop(x, y, 2)) / 3);
	}

	return result;
}



int main() {
	Octree ot;
	ot.build();
    //CImg<double> A(entry_path.c_str());
    //A.resize(48,48);
    // vector<double> vA = vectorize(A,3); 
	// for(auto e: imgs) {
	// 	e.display();
	// }
	for(int z = 0; z < 40 ; z++) {
		ot.getCut(0,0,z,255,0,z,0,255,z);
	}
	

    return 0;
}
