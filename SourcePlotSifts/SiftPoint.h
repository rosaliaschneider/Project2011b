#ifndef __SIFTPOINT_H
#define __SIFTPOINT_H

#include <RX/vec2.h>

struct SiftPoint
{
	SiftPoint()
	: pos(0, 0)
	{
	}

	SiftPoint(double x, double y)
	: pos(x, y)
	{
	}

	SiftPoint(double x, double y, int frame)
	: pos(x, y), firstFrame(frame)
	{
	}

	RX::vec2 pos;
	int firstFrame;
	std::string info;
};


#endif //__SIFTPOINT_H