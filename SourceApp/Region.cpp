#include <RX/vec2.h>
#include <RX/mat3.h>
#include "Region.h"

bool Region::isInside(RX::vec2 point) const
{
	for(int i = 0; i < _boxes.size(); ++i)
	{
		if(_boxes[i].isInside(point.x, point.y))
			return true;
	}
	return false;
}

void Region::addBoxes(std::vector<BBox> boxes)
{
	for(int i = 0; i < boxes.size(); ++i)
		_boxes.push_back(boxes[i]);
}

//bool Region::isNeighbor(Region region) const
//{
//	if((abs(_indexI - region.indexI()) <= 1 && abs(_indexJ - region.indexJ()) <= 1)&&
//		abs(_startingFrame - region.startingFrame()) <= 300)
//	{
//		return true;
//	}
//	return false;
//}
