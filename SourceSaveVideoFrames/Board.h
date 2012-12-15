#ifndef __BOARD_H
#define __BOARD_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>
#include <RX/mat3.h>

struct BBox
{
	RX::vec2 points[4];

	bool isInside(double x, double y);
};

class Board
{
public:
	Board(int numFrames = 0);
	~Board();

	void setPos(double x, double y, int corner, int frame);
	void setPos(BBox b, int frame);
	BBox getPosition(int frame) { return _boxes[frame]; }

private:
	std::vector<BBox> _boxes;
};

#endif // __BOARD_H