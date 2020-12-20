#ifndef OCTREE_H
#define OCTREE_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#define cimg_display  1
#define cimg_use_png  1
#define cimg_use_jpeg 1
#include "CImg.h"
using namespace cimg_library;
using namespace std;

void  Binarizar(CImg<float> & img, int umbral, int z_plane, char*** cube)
{
    for(int i=0;i< img.width();i++)
        for(int j=0;j< img.height();j++)
        {
            int r = img(i,j,0);
            int g = img(i,j,1);
            int b = img(i,j,2);
            if ( (r+g+b)/3  > umbral)
                cube[z_plane][i][j] = 0;
            else
                cube[z_plane][i][j] = 255;
        }
}

struct Node {
    int xi, xf, yi, yf, zi, zf;

    char color = 100;
    Node* children[8] = {};

    Node(){};

	Node(int xi, int xf, int yi, int yf, int zi, int zf) {
        this->xi = xi;
        this->xf = xf;
        this->yi = yi;
        this->yf = yf;
        this->zi = zi;
        this->zf = zf;
    }
	friend ostream& operator<<( ostream &, Node* &);
  	friend istream& operator>>( istream &, Node* &);
};

ostream& operator<<(ostream& stream, Node* & obj) {
	stream.write((char*)&obj->xi, sizeof(int));
    stream.write((char*)&obj->xf, sizeof(int));
    stream.write((char*)&obj->yi, sizeof(int));
    stream.write((char*)&obj->yf, sizeof(int));
    stream.write((char*)&obj->color, sizeof(char));

	return stream;
}

istream& operator>>(istream& stream, Node* & obj) {
	stream.read((char*)&obj->xi, sizeof(int));
	stream.read((char*)&obj->xf, sizeof(int));
	stream.read((char*)&obj->yi, sizeof(int));
	stream.read((char*)&obj->yf, sizeof(int));
	stream.read((char*)&obj->color, sizeof(char));
	
	return stream;
}


class Octree {
private:
    Node* root;
	CImg<char> curPlane;
	long a, b, c, d;
	int pixcount = 0;

	bool sameColor(int xi, int xf, int yi, int yf, int zi, int zf, char*** img) {
		char pixel = img[zi][xi][yi];
		for(int k = zi; k <= zf; ++k){
			for (int i = xi; i <= xf; ++i) {
				for (int j = yi; j <= yf; ++j) {
					if (pixel != img[k][i][j]) {
						//std::cout<<xi<<" "<<xf<<" "<<yi<<" "<<yf<<" "<<zi<<" "<<zf<<" not same color.\n"<<std::endl;
						return false;
					}
				}
			}	
		}

		return true;
	}

    void insert(int xi, int xf, int yi, int yf, int zi, int zf, char*** img, Node* &n) {
        n = new Node(xi, xf, yi, yf, zi, zf);
		//std::cout<<xi<<" "<<xf<<" "<<yi<<" "<<yf<<" "<<zi<<" "<<zf<<std::endl;
		if (sameColor(xi, xf, yi, yf, zi, zf, img) || (xi == xf && yi == yf && zi == zf)) {
			n->color = img[zi][xi][yi];	
		} else {
			int zmid = (zf+zi)/2+1;
			int ymid = (yf+yi)/2;
			if(ymid > yf) cout<<"Y OVERFLOW\n";
			if(ymid > yf) ymid--;
			insert(xi, (xf+xi)/2, yi, ymid, zi, (zf+zi)/2, img, n->children[0]);
			insert((xf+xi)/2+1, xf, yi, ymid, zi, (zf+zi)/2, img, n->children[1]);
			insert(xi, (xf+xi)/2, ymid+1, yf, zi, (zf+zi)/2, img, n->children[2]);
			insert((xf+xi)/2+1, xf, ymid+1, yf, zi, (zf+zi)/2, img, n->children[3]);
			if(zmid > zf) {
				// cout<<"Z OVERFLOW\n";
				zmid--;
				// return;
			}
			insert(xi, (xf+xi)/2, yi, ymid, zmid, zf, img, n->children[4]);
			insert((xf+xi)/2+1, xf, yi, ymid, zmid, zf, img, n->children[5]);
			insert(xi, (xf+xi)/2, ymid+1, yf, zmid, zf, img, n->children[6]);
			insert((xf+xi)/2+1, xf, ymid+1, yf, zmid, zf, img, n->children[7]);
		}
        
	}

	bool intersecta(Node* & node) { 
		//Usar ecuacion del plano
		return (((a*node->xi + b*node->yi + c*node->zi + d > 0) && (a*node->xf + b*node->yf + c*node->zf + d < 0)) ||
			((a*node->xi + b*node->yi + c*node->zi + d > 0) && (a*node->xf + b*node->yf + c*node->zf + d < 0))) ||
			//
			(((a*node->xf + b*node->yi + c*node->zi + d > 0) && (a*node->xi + b*node->yf + c*node->zf + d < 0)) ||
			((a*node->xf + b*node->yi + c*node->zi + d < 0) && (a*node->xi + b*node->yf + c*node->zf + d > 0))) ||

			//
			(((a*node->xf + b*node->yf + c*node->zi + d > 0) && (a*node->xi + b*node->yi + c*node->zf + d < 0)) ||
			((a*node->xf + b*node->yf + c*node->zi + d < 0) && (a*node->xi + b*node->yi + c*node->zf + d > 0))) ||

			//
			(((a*node->xi + b*node->yf + c*node->zi + d > 0) && (a*node->xf + b*node->yi + c*node->zf + d < 0)) ||
			((a*node->xi + b*node->yf + c*node->zi + d < 0) && (a*node->xf + b*node->yi + c*node->zf + d > 0))) ||
	
			//
			(a*node->xi + b*node->yi + c*node->zi + d == 0) ||
			(a*node->xf + b*node->yi + c*node->zi + d == 0) ||
			(a*node->xi + b*node->yf + c*node->zi + d == 0) ||
			(a*node->xi + b*node->yi + c*node->zf + d == 0) ||
			(a*node->xf + b*node->yf + c*node->zi + d == 0) ||
			(a*node->xf + b*node->yi + c*node->zf + d == 0) ||
			(a*node->xi + b*node->yf + c*node->zf + d == 0) ||
			(a*node->xf + b*node->yf + c*node->zf + d == 0);
	}

	void getCut(Node* & node) {
		if(node->color != 100) {
			//Llenar pixeles
			for(int k = node->zi; k <= node->zf; ++k){
				for (int i = node->xi; i <= node->xf; ++i) {
					for (int j = node->yi; j <= node->yf; ++j) {
						//Reemplazar valores en ecuacion de plano, si da 0 esta en el plano y llenamos curPlane.
						if(a*i + b*j + c*k + d == 0) {

							pixcount++;

							if(a != 0 && b == 0 && c == 0) {//j = x, k = y
								curPlane(j,k) = node->color;
							}
							else if(a == 0 && b != 0 && c == 0) {//i = x, k = y
								curPlane(i,k) = node->color;
							}
							else if(a == 0 && b == 0 && c != 0){//i = x, j = y
								curPlane(i,j) = node->color;
							}
							else if(c == 0){ //diagonal en plano xy
								//X es sqrt(x^2 + y^2) Y es z
								curPlane((int)sqrt((i*i)+(j*j)),k) = node->color;
							}
							else if(b == 0){ //diagonal en plano xz
								//X es sqrt(x^2 + z^2) Y es y
								curPlane((int)sqrt((i*i)+(k*k)),j) = node->color;
							}
							else if(a == 0){ //diagonal en plano yz
								//X es sqrt(z^2 + y^2) Y es x
								curPlane((int)sqrt((k*k)+(j*j)),i) = node->color;
							}

						}
						
					}
				}
			}
			return;
		}
		for(int i = 0; i < 8; i++) {
			if(intersecta(node->children[i])) {
				getCut(node->children[i]);
			}
		}
		
		
	}

	void compress(std::ofstream &file, Node* &n) {
		file << n;
		if (n->color == 100) {
        	for (int i = 0; i < 4; ++i)
				compress(file, n->children[i]);	
        }
	}

	void load(std::ifstream &file, Node* &n) {
		n = new Node;
		file >> n;	
	   	if (n->color == 100) {
			for (int i = 0; i < 4; ++i)
				load(file, n->children[i]);
		}
	}

public:
    Octree():root(0){
		curPlane.resize(512,512);
	};
    Node* getRoot() {return root;}

	void getCut(int p1_x, int p1_y, int p1_z, int p2_x, int p2_y, int p2_z, int p3_x, int p3_y, int p3_z, string filename) {
		curPlane.clear();
		//Sacar ecuacion de plano y guardar
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



		if(a != 0 && b == 0 && c == 0) {//j = x, k = y
			curPlane.resize(512,40);
		}
		else if(a == 0 && b != 0 && c == 0) {//i = x, k = y
			curPlane.resize(512,40);
		}
		else if(a == 0 && b == 0 && c != 0){//i = x, j = y
			curPlane.resize(512,512);
		}
		else if(c == 0){ //diagonal en plano xy
			//X es sqrt(x^2 + y^2) Y es z
			
			curPlane.resize(sqrt(512*512*2),40);
		}
		else if(b == 0){ //diagonal en plano xz
			//X es sqrt(x^2 + z^2) Y es y
			//cout<<"Diagonal en plano xz\n";
			curPlane.resize(sqrt(512*512+40*40),512);
		}
		else if(a == 0){ //diagonal en plano yz
			//X es sqrt(z^2 + y^2) Y es x
			curPlane.resize(sqrt(512*512+40*40),512);
		}
		

		cout<<"Ecuacion de curva\n";
		cout<<"a: "<<a;
		cout<<"\nb: "<<b;
		cout<<"\nc: "<<c;
		cout<<"\nd: "<<d<<endl;

		getCut(root);
		cout<<"\n\nPixel count: "<<pixcount<<endl;
		//curPlane.save_jpeg(filename.c_str(), 60);	
		curPlane.display();
	}

    void build() {
		string entry_path = "/Users/panflete/Documents/UTEC/Ciclo 6/EDA/octree/data/paciente 1/1/Paciente1CC-27-10-1988- CT from 18-01-2011 S0 I";
		string final_path;
		char*** cube = new char**[40];
		for(int i = 0; i < 40; i++) {
			cube[i] = new char*[512];
			for(int j = 0; j < 512; j++) {
				cube[i][j] = new char[512];
			}
		}
		CImg<float> R;
		for(int i = 0; i < 40; i++) {

			final_path = entry_path + to_string(i) + ".BMP";
			R = CImg<float>(final_path.c_str());
			Binarizar(R,145,i,cube);

		}
		
        insert(0, R.width()-1, 0, R.height()-1, 0, 39, cube, root);
		
		for(int i = 0; i < 40; i++) {
			for(int j = 0; j < 512; j++) {
				delete(cube[i][j]);
			}
			delete(cube[i]);
		}
		delete[](cube);
    }

	void compress(string name) {
		std::ofstream file(name, std::ios::binary);
		compress(file, root);		
	}


	void load(string name) {
		std::ifstream file(name, std::ios::binary);
		load(file, root);
	}


};


#endif
