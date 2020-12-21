#include "Octree.h"
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

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

char*** cube;

void getCut(int p1_x, int p1_y, int p1_z, int p2_x, int p2_y, int p2_z, int p3_x, int p3_y, int p3_z, int p4_x, int p4_y, int p4_z) {
	int a, b, c, d;
	CImg<char> curPlane;
	curPlane.resize(512,512);
	int p[3];
	p[0] = p1_x;
	p[1] = p1_y;
	p[2] = p1_z;

	int PQ[3], PR[3];
	PQ[0] = p2_x - p1_x;
	PQ[1] = p2_y - p1_y;
	PQ[2] = p2_z - p1_z;

	PR[0] = p3_x - p1_x;
	PR[1] = p3_y - p1_y;
	PR[2] = p3_z - p1_z;
	
	a = PQ[1] * PR[2] - PQ[2] * PR[1];
	b = -(PQ[0] * PR[2] - PQ[2] * PR[0]);
	c = PQ[0] * PR[1] - PQ[1] * PR[0];
	d = - a * p1_x - b * p1_y - c * p1_z;
	for(int k = 0; k <= 39; k++){
		
		for (int j = 0; j <= 511; j++) {
			
			for (int i = 0; i <= 511; i++) {
				//Reemplazar valores en ecuacion de plano, si da 0 esta en el plano y llenamos curPlane.
				if(a*i + b*j + c*k + d == 0) {
					

					if(a != 0 && b == 0 && c == 0) {//j = x, k = y
						curPlane(j,k) = cube[k][j][i];
					}
					else if(a == 0 && b != 0 && c == 0) {//i = x, k = y
						curPlane(i,k) =  cube[k][j][i];
					}
					else if(a == 0 && b == 0 && c != 0){//i = x, j = y
					
						curPlane(i,j) =  cube[k][j][i];
					}
					else if(a == 0){ //y = yz
						curPlane(i,(int)sqrt(pow(p[1]-j,2)+pow(p[2]-k,2))) =  cube[k][j][i];
					}
					else if(b == 0) { //y = y
						curPlane((int)sqrt(pow(p[0]-i,2)+pow(p[2]-k,2)),j) =  cube[k][j][i];
					}
					else  if(c == 0){ //y = z
						curPlane((int)sqrt(pow(p[0]-i,2)+pow(p[1]-j,2)),k) =  cube[k][j][i];
					}

				}
				
			}
		}
	}
}

int main() {
	Octree ot;
	ot.build();

    //CImg<double> A(entry_path.c_str());
    //A.resize(48,48);
    // vector<double> vA = vectorize(A,3); 
	// for(auto e: imgs) {
	// 	e.display();
	// // }
	// string filename;
	// for(int z = 0; z < 40 ; z++) {
	// 	filename = "gif/img"+to_string(z)+".jpeg";
	// 	ot.getCut(0,0,z,0,255,z,255,0,z,255,255,z, filename);
	// }

	//=================== CUBE TESTING ===================

	// string entry_path = "data/paciente 1/1/Paciente1CC-27-10-1988- CT from 18-01-2011 S0 I";
	// string final_path;
	// cube = new char**[40];
	// for(int i = 0; i < 40; i++) {
	// 	cube[i] = new char*[512];
	// 	for(int j = 0; j < 512; j++) {
	// 		cube[i][j] = new char[512];
	// 	}
	// }
	// CImg<float> R;
	// for(int i = 0; i < 40; i++) {

	// 	final_path = entry_path + to_string(i) + ".BMP";
	// 	R = CImg<float>(final_path.c_str());
	// 	Binarizar(R,140,i,cube);

	// }

	// pid_t pid = getpid();
	// cout<<pid<<endl;
	// sleep(10);


	// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	// ot.getCut(0,0,0,511,0,0,0,511,0,511,511,0,"a"); //First layer
	// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
	// begin = std::chrono::steady_clock::now();
	// getCut(0,0,0,511,0,0,0,511,0,511,511,0);
	// end = std::chrono::steady_clock::now();
	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

	// ot.getCut(0,0,0,
	// 		512,0,0,
	// 		0,512,40,
	// 		512,512,40,
	// 		"a"); //Lado B

	ot.getCut(0,0,0,
			512,512,0,
			0,0,40,
			512,512,40,
			"a"); //Lado A

	// ot.getCut(0,0,0,
	// 		0,512,0,
	// 		512,0,40,
	// 		512,512,40,
	// 		"a"); //Lado C
    return 0;
}
