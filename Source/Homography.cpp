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


void Homography::loadPointsFirstMap(int file)
{
	_n = siftMapObj->numMatches(file);

	createMatrix(_x1, 3, _n);
	
	for(int i=0; i < _n; ++i) 
	{
		_x1[0][i] = siftMapObj->siftPoints(0, siftMapObj->match(file, i).first, 0);
		_x1[1][i] = siftMapObj->siftPoints(0, siftMapObj->match(file, i).first, 1);

		_x1[2][i] = 1;
	}
}

void Homography::loadPointsFirst(int file)
{
	_n = _matches[file].size();

	createMatrix(_x1, 3, _n);
	
	for(int i=0; i < _n; ++i) 
	{
		_x1[0][i] = _sifts[file][_matches[file][i].first].pos.x;
		_x1[1][i] = _sifts[file][_matches[file][i].first].pos.y;

		_x1[0][i] -= _w/2;
		_x1[1][i] -= _h/2;

		_x1[2][i] = 1;
	}
}

void Homography::loadPointsSecond(int file, bool useMap)
{
	_n = _matches[file].size();

	createMatrix(_x2, 3, _n);
	for(int i=0; i < _n; ++i) 
	{
		_x2[0][i] = _sifts[file+1][_matches[file][i].second].pos.x;
		_x2[1][i] = _sifts[file+1][_matches[file][i].second].pos.y;

		_x2[0][i] -= _w/2;
		_x2[1][i] -= _h/2;

		_x2[2][i] = 1;
	}		  
}

void Homography::updateSifts(int file)
{
	for(int i=0; i < _n; ++i) 
	{
		siftObj->setFirstFrameFromMatch(file, i);
	}
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

void Homography::setWidth(int width)
{
	_w = width;
}

void Homography::setHeight(int height)
{
	_h = height;
}
