#include "Board.h"

Board::Board(int numFrames)
{
	_boxes.resize(numFrames);
}

Board::~Board()
{
}

void Board::setPos(double x, double y, int corner, int frame)
{
	_boxes[frame].points[corner] = RX::vec2(x, y);
}

void Board::addInterval(Interval interval)
{
	_timeLinks.push_back(interval);
}

unsigned long Board::getTime(int frame)
{
	for(int i = 0; i < _timeLinks.size(); ++i)
	{
		if(_timeLinks[i].begin <= frame && _timeLinks[i].end >= frame)
			return _timeLinks[i].timeMsec;
	}
	return -1;
}


bool BBox::isInside(double x, double y) const
{
	if((pointInsideTriangle(points[0], points[1], points[2], RX::vec2(x, y)))||(pointInsideTriangle(points[0], points[2], points[3], RX::vec2(x, y))))
	{
		return true;
	}
	return false;
}

bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b)
{
	RX::vec3 cross1 = RX::vec3((a-b),1) ^ RX::vec3((p1-b),1);
	RX::vec3 cross2 = RX::vec3((a-b),1) ^ RX::vec3((p2-b),1);
	if(cross1 * cross2 >= 0) 
		return true;
	return false;
}

bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p)
{
	if((sameSide(p, p1, p2, p3))&&(sameSide(p, p2, p1, p3))&&(sameSide(p, p3, p1, p2)))
		return true;
	return false;
}