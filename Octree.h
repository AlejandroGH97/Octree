#ifndef OCTREE_H
#define OCTREE_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define cimg_display  1
#define cimg_use_png  1
#define cimg_use_jpeg 1
#include "CImg.h"
using namespace cimg_library;
using namespace std;

CImg<char>  Binarizar(CImg<float> & img, int umbral)
{
    CImg<char> R(img.width(),img.height());
    for(int i=0;i< img.width();i++)
        for(int j=0;j< img.height();j++)
        {
            int r = img(i,j,0);
            int g = img(i,j,1);
            int b = img(i,j,2);
            if ( (r+g+b)/3  > umbral)
                R(i,j) = 255;
            else
                R(i,j) = 0;
        }
    return R;
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
	std::vector<CImg<char>> current_image;
	CImg<char> curPlane;

	bool sameColor(int xi, int xf, int yi, int yf, int zi, int zf, std::vector<CImg<char>>& img) {
		char pixel = img[zi](xi,yi);
		for(int k = zi; k <= zf; ++k){
			for (int i = xi; i <= xf; ++i) {
				for (int j = yi; j <= yf; ++j) {
					if (pixel != img[k](i,j)) {
						//std::cout<<xi<<" "<<xf<<" "<<yi<<" "<<yf<<" "<<zi<<" "<<zf<<" not same color.\n"<<std::endl;
						return false;
					}
				}
			}	
		}

		return true;
	}

    void insert(int xi, int xf, int yi, int yf, int zi, int zf, std::vector<CImg<char>>& img, Node* &n) {
        n = new Node(xi, xf, yi, yf, zi, zf);
		//std::cout<<xi<<" "<<xf<<" "<<yi<<" "<<yf<<" "<<zi<<" "<<zf<<std::endl;
		if (sameColor(xi, xf, yi, yf, zi, zf, img) || (xi == xf && yi == yf && zi == zf)) {
			n->color = img[zi](xi,yi);	
		} else {
			int zmid = (zf+zi)/2+1;
			int ymid = (yf+yi)/2;
			insert(xi, (xf+xi)/2, yi, ymid, zi, (zf+zi)/2, img, n->children[0]);
			insert((xf+xi)/2+1, xf, yi, ymid, zi, (zf+zi)/2, img, n->children[1]);
			insert(xi, (xf+xi)/2, ymid+1, yf, zi, (zf+zi)/2, img, n->children[2]);
			insert((xf+xi)/2+1, xf, ymid+1, yf, zi, (zf+zi)/2, img, n->children[3]);
			if(zmid > zf) zmid--;
			insert(xi, (xf+xi)/2, yi, ymid, zmid, zf, img, n->children[4]);
			insert((xf+xi)/2+1, xf, yi, ymid, zmid, zf, img, n->children[5]);
			insert(xi, (xf+xi)/2, ymid+1, yf, zmid, zf, img, n->children[6]);
			insert((xf+xi)/2+1, xf, ymid+1, yf, zmid, zf, img, n->children[7]);
		}
        
	}

	bool instersecta(int xi, int xf, int yi, int yf, int zi, int zf), Node* node) {

	}

	void draw(Node* & node, int xi, int xf, int yi, int yf, int zi, int zf) {
		if(instersecta( xi, xf, yi, yf, zi, zf), Node* node)
		if (node->color == 100) {
			for (int i = 0; i < 4; ++i)
				draw(node->children[i], N);
		} else {
			for (int i = node->xi; i <= node->xf; ++i) {
		   		for (int j = node->yi; j <= node->yf; ++j)
					curPlane(i,j) = node->color;
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
    Octree():root(0){};
    Node* getRoot() {return root;}

	void draw(int xi, int xf, int yi, int yf, int zi, int zf) {
		CImg<char> N(root->xf+1, root->yf+1, 1);
		draw(xi, xf, yi, zyf, zi, zf, root);
		curPlane.display();
	}

    void build() {

		string entry_path = "/Users/panflete/Documents/UTEC/Ciclo 6/EDA/octree/data/paciente 1/1/Paciente1CC-27-10-1988- CT from 18-01-2011 S0 I";
		string final_path;
		std::vector<CImg<char>> cube;
		CImg<float> R;
		for(int i = 0; i < 40; i++) {
			final_path = entry_path + to_string(i) + ".BMP";
			R = CImg<float>(final_path.c_str());
			cube.push_back(Binarizar(R,120));
		}
        
		// for(auto e: cube) {
		// 	e.display();
		// }
		
        insert(0, R.width()-1, 0, R.height()-1, 0, 39, cube, root);
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
