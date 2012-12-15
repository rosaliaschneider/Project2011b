#include "BBox.h"

bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b)
{
	RX::vec3 cross1 = RX::vec3((a-b),1) ^ RX::vec3((p1-b),1);
	RX::vec3 cross2 = RX::vec3((a-b),1) ^ RX::vec3((p2-b),1);
	if(cross1 * cross2 >= 0) 
		return true;
	return false;
}

bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p)
{
	if((sameSide(p, p1, p2, p3))&&(sameSide(p, p2, p1, p3))&&(sameSide(p, p3, p1, p2)))
		return true;
	return false;
}