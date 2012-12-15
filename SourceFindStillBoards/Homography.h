#ifndef __HOMOGRAPHY_H
#define __HOMOGRAPHY_H

#include <vector>
#include <RX/mat3.h>
#include <RX/vec2.h>
#include "Define.h"
using namespace std;

typedef vector< vector<double> > matd;

struct point
{
	RX::vec2 pos;
};

class Homography
{
public:
	Homography();
	~Homography();

	void setPoints1(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);
	void setPoints2(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);
	void estimate();

	inline RX::mat3 hom() { return _hom; }

private:
	void fit(matd &x1, matd &x2, RX::mat3 &H);

	RX::mat3 _hom;
	matd _x1, _x2;
	matd _ux1, _ux2;
	int _n, _nused;
	
	int _numFrames;
	int _w, _h;
};

#endif // __HOMOGRAPHY_H
