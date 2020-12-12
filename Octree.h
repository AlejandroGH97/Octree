#ifndef QUADTREE_QUADTREE_H
#define QUADTREE_QUADTREE_H

#include <string>
#include <fstream>
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
    int xi, xf, yi, yf;

    char color = 100;
    Node* children[4] = {};

    Node(){};

	Node(int xi, int xf, int yi, int yf) {
        this->xi = xi;
        this->xf = xf;
        this->yi = yi;
        this->yf = yf;
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


class QuadTree {
private:
    Node* root;

	bool sameColor(int xi, int xf, int yi, int yf, CImg<char>& img) {
		char pixel = img(xi,yi);
		for (int i = xi; i <= xf; ++i) {
			for (int j = yi; j <= yf; ++j) {
				if (pixel != img(i,j))	return false;
			}
		}
		return true;
	}

    void insert(int xi, int xf, int yi, int yf, CImg<char>& img, Node* &n) {
        n = new Node(xi, xf, yi, yf);
		if (sameColor(xi, xf, yi, yf, img)) {
			n->color = img(xi,yi);	
		} else {
			insert(xi, (xf+xi)/2, yi, (yf+yi)/2, img, n->children[0]);
			insert((xf+xi)/2+1, xf, yi, (yf+yi)/2, img, n->children[1]);
			insert(xi, (xf+xi)/2, (yf+yi)/2+1, yf, img, n->children[2]);
			insert((xf+xi)/2+1, xf, (yf+yi)/2+1, yf, img, n->children[3]);
		}
        
	}
	void draw(Node* & node, CImg<char> & N) {
		if (node->color == 100) {
			for (int i = 0; i < 4; ++i)
				draw(node->children[i], N);
		} else {
			for (int i = node->xi; i <= node->xf; ++i) {
		   		for (int j = node->yi; j <= node->yf; ++j)
					N(i,j) = node->color;
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
    QuadTree():root(0){};
    Node* getRoot() {return root;}

	void draw() {
		CImg<char> N(root->xf+1, root->yf+1, 1);
		draw(root, N);
		N.display();
	}

    void build(string name, bool bw = 1) {
        CImg<char> R;
		if (bw) R.load(name.c_str());
		else {
			CImg<float> A;
			A.load(name.c_str());
       		R = Binarizar(A, 120);
		}
        insert(0, R.width()-1, 0, R.height()-1, R, root);
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


#endif //QUADTREE_QUADTREE_H
