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

	// Gets
	int startingFrame() const { return _startingFrame; }
	int nBoxes() const { return _boxes.size(); }
	RX::vec3 color() const { return _color; }
	std::vector<BBox> boxes() { return _boxes; }
	BBox box(int boxNum) { return _boxes[boxNum]; }


	// Sets
	void setStartingFrame(int startingFrame) { _startingFrame = startingFrame; }
	void setColor(RX::vec3 color) { _color = color; }

private:
	int _startingFrame;
	std::vector<BBox> _boxes;
	RX::vec3 _color;
};



#endif // __BBOX_H