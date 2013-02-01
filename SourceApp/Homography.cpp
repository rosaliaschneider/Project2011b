#include "Globals.h"
#include "Homography.h"

Homography::Homography()
: _nused(0)
{
	_hom.setIdentity();
}

Homography::~Homography()
{
}

void Homography::loadPoints(int file1, bool inverted)
{
	_n = siftObj->numMatches(file1);

	createMatrix(_x1, 3, _n);
	createMatrix(_x2, 3, _n);
	_w = 321;
	_h = 241;
	
	for(int i=0; i < _n; ++i) 
	{
		_x1[0][i] = siftObj->siftPoints(file1, siftObj->match(file1, i).first, 0);
		_x1[1][i] = siftObj->siftPoints(file1, siftObj->match(file1, i).first, 1);

		_x2[0][i] = siftObj->siftPoints(file1+1, siftObj->match(file1, i).second, 0);
		_x2[1][i] = siftObj->siftPoints(file1+1, siftObj->match(file1, i).second, 1);

		_x1[0][i] -= _w/2;
		_x2[0][i] -= _w/2;
		_x1[1][i] -= _h/2;
		_x2[1][i] -= _h/2;

		_x1[2][i] = _x2[2][i] = 1;
	}		  
}

void Homography::saveInliers(int file1, bool inverted)
{ 
}

void Homography::estimate(int *noi, int *nof, double threshold)
{
	if(_n < 20) {
		_hom.setIdentity();
		return;
	}

	RX::mat3 H3, H4;

	Ransac R4(_ux2, _ux1, _nused);
	R4.normalize();
	R4.mainLoop(4, 0.01, H4);
	R4.fitHomography(H4);
	R4.denormalize(H4);

	_hom = H4;
	if(noi) *noi = R4.noi();
	if(nof) *nof = R4.nof();
	_inliers = R4.inliers();
	
 	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			_hom.set(i, j, _hom.at(i, j)/_hom.at(2, 2));
		}
	}
}

void Homography::drawUsedPoints()
{
	glBegin(GL_POINTS);
	for(int i = 0; i < _nused; ++i) {
		glVertex2f(_ux2[0][i], _ux2[1][i]);
	}
	glEnd();
}

void Homography::drawAllPoints()
{
	glBegin(GL_POINTS);
	for(int i = 0; i < _n; ++i) {
		glVertex2f(_x2[0][i], _x2[1][i]);
	}
	glEnd();
}

void Homography::all()
{
	createMatrix(_ux1, 3, 0);
	createMatrix(_ux2, 3, 0);
	_nused = _n;

	for(int i = 0; i < _n; ++i)
	{
		_ux1[0].push_back(_x1[0][i]);
		_ux2[0].push_back(_x2[0][i]);
		_ux1[1].push_back(_x1[1][i]);
		_ux2[1].push_back(_x2[1][i]);
		_ux1[2].push_back(1);
		_ux2[2].push_back(1);
	}
}

int Homography::numInliers()
{
	int sum = 0;
	for(int i = 0; i < _n; ++i) 
		if(_inliers[i]) ++sum;
	return sum;
}

void Homography::loadInfo(std::string filename)
{
	std::ifstream input(filename.c_str());
	int nBoards, nBoardMovements, nMovements;
	input >> _numFrames;
	input >> _w >> _h;
}
