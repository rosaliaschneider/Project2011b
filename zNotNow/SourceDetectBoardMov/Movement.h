#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#include <RX/vec2.h>
#include <vector>
#include "BBox.h"

class Movement
{
public:
	Movement(int start, int end, int board)
	{
		_start = start;
		_end = end;
		_board = board;

		addBoards();
	}

	int start() { return _start; }
	int end() { return _end; }
	int board() { return _board; }

	void addBoards()
	{
		for(int i = _start; i <= _end; ++i)
			points.push_back(BBox(RX::vec2(-100, 100), RX::vec2(-100, -100), RX::vec2(100, -100), RX::vec2(100, 100)));
	}

	bool active(int frame)
	{
		return frame >= _start  && frame <= _end;
	}

	double getX(int frame, int point)
	{
		return points[frame-_start].getX(point);
	}
	double getY(int frame, int point)
	{
		return points[frame-_start].getY(point);
	}
	void setX(int frame, int point, double value)
	{ 
		for(int i = 0; i <= _end-frame; ++i)
			points[frame-_start+i].setX(point, value);
	}
	void setY(int frame, int point, double value)
	{
		for(int i = 0; i <= _end-frame; ++i)
			points[frame-_start+i].setY(point, value);
	}

	double getCX(int frame)
	{
		return points[frame-_start].getCX();
	}
	double getCY(int frame)
	{
		return points[frame-_start].getCY();
	}
	void setCentroid(int frame, double x, double y)
	{ 
		for(int i = 0; i <= _end-frame; ++i)
			points[frame-_start+i].setCentroid(x, y);
	}
private:
	int _start, _end;
	int _board;

	// points at each frame
	std::vector<BBox> points;
};

#endif // __MOVEMENT_H