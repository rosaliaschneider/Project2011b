#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#include <RX/vec2.h>
#include <vector>
#include "BBox.h"

struct Movement
{
	int start, end;
	int board;

	// points at each frame
	std::vector<BBox> points;

	Movement()
	: board(-1)
	{
	}

	void addBoard(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
	{
		points.push_back(BBox(p1, p2, p3, p4));
	}

	BBox bbox(int frame)
	{
		return points[frame-start];
	}

	bool active(int frame)
	{
		return frame >= start  && frame <= end;
	}

	double getX(int frame, int point)
	{
		return points[frame-start].getX(point);
	}
	double getY(int frame, int point)
	{
		return points[frame-start].getY(point);
	}
	void setX(int frame, int point, double value)
	{ 
		for(int i = 0; i <= end-frame; ++i)
			points[frame-start+i].setX(point, value);
	}
	void setY(int frame, int point, double value)
	{
		for(int i = 0; i <= end-frame; ++i)
			points[frame-start+i].setY(point, value);
	}
};

#endif // __MOVEMENT_H