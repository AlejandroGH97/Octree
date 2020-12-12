#define cimg_display  1
#define cimg_use_png  1
#define cimg_use_jpeg 1
#include "CImg.h"
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

    string entry_path = "/Users/panflete/Documents/UTEC/Ciclo 6/EDA/octree/datos cerebros/paciente 1/1/Paciente1CC-27-10-1988- CT from 18-01-2011 S0 I";
	string final_path;
	vector<CImg<double>> imgs;
	for(int i = 0; i < 40; i++) {
		final_path = entry_path + to_string(i) + ".BMP";
		imgs.push_back(CImg<double>(final_path.c_str()));
	}
    //CImg<double> A(entry_path.c_str());
    //A.resize(48,48);
    // vector<double> vA = vectorize(A,3); 
	for(auto e: imgs) {
		e.display();
	}

    return 0;
}
