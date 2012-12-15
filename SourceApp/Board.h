#ifndef __BOARD_H
#define __BOARD_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include "Interval.h"

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
	void addInterval(Interval interval);

	BBox getPosition(int frame) { return _boxes[frame]; }
	unsigned long getTime(int frame);

private:
	std::vector<BBox> _boxes;
	std::vector<Interval> _timeLinks;
};

bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b);
bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p);

#endif // __BOARD_H