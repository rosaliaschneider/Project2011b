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
	// SIFT
	std::string str = _folder;
	char buf[20];
	sprintf(buf, "state_%d.txt", file1);
	str += buf;
	
	ifstream input(str.c_str());
	input >> _n;
	createMatrix(_x1, 3, _n);
	createMatrix(_x2, 3, _n);

	for(int i=0; i<_n; ++i) {
		input >> _x1[0][i] >> _x2[0][i];
		input >> _x1[1][i] >> _x2[1][i];

		_x1[0][i] -= _w/2;
		_x2[0][i] -= _w/2;
		_x1[1][i] -= _h/2;
		_x2[1][i] -= _h/2;

		_x1[2][i] = _x2[2][i] = 1;
	}		  
}

void Homography::estimate(int *noi, int *nof, double threshold)
{
	RX::mat3 H3, H4;
	
	Ransac R4(_ux2, _ux1, _nused);
	R4.normalize();
	R4.mainLoop(4, 0.01, H4);
	R4.fitHomography(H4);
	R4.denormalize(H4);

	//Ransac R3(_ux2, _ux1, _nused);
	//R3.normalize();
	//R3.mainLoop(4, 0.1, H3);
	//R3.fitHomography(H3);
	//R3.denormalize(H3);

	//if(R3.noi() >= 2*R4.noi())
	//{
	//	_hom = H3;
	//	if(noi) *noi = R3.noi();
	//	if(nof) *nof = R3.nof();
	//	_inliers = R3.inliers();
	//}
	//else
	//{
		_hom = H4;
		if(noi) *noi = R4.noi();
		if(nof) *nof = R4.nof();
		_inliers = R4.inliers();
	//}

 	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			_hom.set(i, j, _hom.at(i, j)/_hom.at(2, 2));
		}
	}
}

RX::mat3 Homography::estimatePriv()
{
	RX::mat3 H3, H4, Hn;
	
	Ransac R4(_ux2, _ux1, _nused);
	R4.normalize();
	R4.mainLoop(4, 0.01, H4);
	R4.fitHomography(H4);
	R4.denormalize(H4);

	//Ransac R3(_ux2, _ux1, _nused);
	//R3.normalize();
	//R3.mainLoop(4, 0.0001, H3);
	//R3.fitHomography(H3);
	//R3.denormalize(H3);
	//
	//if(R3.noi() >= 2*R4.noi())
		//Hn = H3;
	//else
		Hn = H4;

	return Hn;
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

bool Homography::updateMovement(int currentFrame)
{
	//bool isMoving;
	//
	//// UPDATE MOVEMENT
	//for(int i = 0; i < _currMovements.size(); ++i)
	//	_currMovements[i].board = -1;
	//isMoving = false;
	//
	//for(int i = 0; i < _movements.size(); ++i) {
	//	Movement m = _movements[i];
	//	if((m.start <= currentFrame)&&(m.end >= currentFrame)) {
	//		_currMovements[m.board].board = m.board;
	//		_currMovements[m.board].start = m.start;
	//		_currMovements[m.board].end = m.end;
	//		isMoving = true;
	//	}
	//}
	//
	//return isMoving;
	return false;
}

void Homography::loadInfo(std::string filename)
{
	std::ifstream input(filename.c_str());
	int nBoards, nBoardMovements, nMovements;
	input >> _numFrames;
	input >> _w >> _h;
}
