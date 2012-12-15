#ifndef __BBOX_H
#define __BBOX_H

#include <RX/vec2.h>

class BBox
{
public:
	static bool intersecting(RX::vec2 p1a, RX::vec2 p2a, RX::vec2 p3a, RX::vec2 p4a, RX::vec2 p1b, RX::vec2 p2b, RX::vec2 p3b, RX::vec2 p4b);
public:
	RX::vec2 p[4];
};

#endif // __BBOX_H