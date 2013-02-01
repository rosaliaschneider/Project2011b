#ifndef __SIFTPOINT_H
#define __SIFTPOINT_H

#include <RX/vec2.h>

struct SiftPoint
{
	SiftPoint(double x, double y)
	: pos(x, y)
	{
	}

	RX::vec2 pos;
	int firstFrame;
};


#endif //__SIFTPOINT_H