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

int a, b, c, d;

bool intersecta(Node* node, int p1_x, int p1_y, int p1_z, int p2_x, int p2_y, int p2_z, int p3_x, int p3_y, int p3_z) { 
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

		//Usar ecuacion del plano
		return ((a*node->xi + b*node->yi + c*node->zi + d > 0) == (a*node->xf + b*node->yf + c*node->zf + d < 0)) ||
			((a*node->xf + b*node->yi + c*node->zi + d > 0) == (a*node->xi + b*node->yf + c*node->zf + d < 0)) ||
			((a*node->xf + b*node->yf + c*node->zi + d > 0) == (a*node->xi + b*node->yi + c*node->zf + d < 0)) ||
			((a*node->xi + b*node->yf + c*node->zi + d > 0) == (a*node->xf + b*node->yi + c*node->zf + d < 0)) ||
	
			(a*node->xi + b*node->yi + c*node->zi + d == 0) ||
			(a*node->xf + b*node->yi + c*node->zi + d == 0) ||
			(a*node->xi + b*node->yf + c*node->zi + d == 0) ||
			(a*node->xi + b*node->yi + c*node->zf + d == 0) ||
			(a*node->xf + b*node->yf + c*node->zi + d == 0) ||
			(a*node->xf + b*node->yi + c*node->zf + d == 0) ||
			(a*node->xi + b*node->yf + c*node->zf + d == 0) ||
			(a*node->xf + b*node->yf + c*node->zf + d == 0);
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
	// string filename;
	// for(int z = 0; z < 40 ; z++) {
	// 	filename = "gif/img"+to_string(z)+".jpeg";
	// 	ot.getCut(0,0,z,0,255,z,255,0,z, filename);
	// }
	//ot.getCut(0,0,0,0,255,0,255,0,0,"a");	
	ot.getCut(0,0,20,0,512,23,512,0,20,"a");	

	// Node* n = new Node(0,2,0,2,0,2);

	// cout<<intersecta(n,0,1,0,1,2,0,2,1,0)<<endl;
	

    return 0;
}
