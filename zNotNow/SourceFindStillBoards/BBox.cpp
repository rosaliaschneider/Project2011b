#include <RX/vec2.h>
#include <RX/mat3.h>
#include "BBox.h"

BBox::BBox()
{
	BBox(4);
}

BBox::BBox(int size)
{
	for(int i = 0; i < size; ++i)
		_p.push_back(RX::vec2(0, 0));
}

void BBox::setX(int point, double value) 
{ 
	_p[point].x = value; 
}
void BBox::setY(int point, double value) 
{ 
	_p[point].y = value;
}

double BBox::getX(int point) { return _p[point].x; }
double BBox::getY(int point) { return _p[point].y; }
RX::vec2 BBox::getPoint(int point) { return _p[point]; }
int BBox::nPoints() { return _p.size(); }

void BBox::transform(RX::mat3 matrix)
{
	for(int i = 0; i < _p.size(); ++i)
	{
		RX::vec3 v = matrix * RX::vec3(_p[i].x, _p[i].y, 1);
		_p[i].x = v.x/v.z;
		_p[i].y = v.y/v.z;
	}
}