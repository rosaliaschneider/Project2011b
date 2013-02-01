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

void Region::addBoxes(std::vector<RX::BBox> boxes)
{
	for(int i = 0; i < boxes.size(); ++i)
		_boxes.push_back(boxes[i]);
}

