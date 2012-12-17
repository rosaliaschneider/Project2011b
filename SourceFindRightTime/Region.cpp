#include <RX/vec2.h>
#include <RX/mat3.h>
#include "Region.h"

bool Region::isInside(RX::vec2 point) const
{
	for(int i = 0; i < _boxes.size(); ++i)
	{
		if(_boxes[i].isInside(point))
			return true;
	}
	return false;
}

BBox::BBox()
{
}

BBox::BBox(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{
	_p.push_back(p1);
	_p.push_back(p2);
	_p.push_back(p3);
	_p.push_back(p4);
}

bool BBox::isInside(RX::vec2 point) const
{
	double minX = 99999, maxX = -99999, minY = 99999, maxY = -99999;
	for(int i = 0; i < 4; ++i)
	{
		if(minX > _p[i].x) minX = _p[i].x;
		if(minY > _p[i].y) minY = _p[i].y;
		if(maxX < _p[i].x) maxX = _p[i].x;
		if(maxY < _p[i].y) maxY = _p[i].y;
	}
	if(point.x < minX) return false;
	if(point.x > maxX) return false;
	if(point.y < minY) return false;
	if(point.y > maxY) return false;
	return true;
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
