#ifndef __BBOX_H
#define __BBOX_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>
#include "Board.h"

class Region
{
public:
	void addBoxes(std::vector<BBox> boxes);
	void addBox(BBox b) { _boxes.push_back(b); }

	bool isInside(RX::vec2 point) const;
	int startingFrame() const { return _startingFrame; }
	std::vector<BBox> boxes() { return _boxes; }

	void setStartingFrame(int startingFrame) { _startingFrame = startingFrame; }

private:
	int _startingFrame;
	std::vector<BBox> _boxes;
};



#endif // __BBOX_H