#include <iostream>
#include "BBox.h"

using namespace std;

bool BBox::intersecting(RX::vec2 p1a, RX::vec2 p2a, RX::vec2 p3a, RX::vec2 p4a, RX::vec2 p1b, RX::vec2 p2b, RX::vec2 p3b, RX::vec2 p4b)
{
	double minx1, miny1, maxx1, maxy1;
	double minx2, miny2, maxx2, maxy2;
	
	minx1 = min(p1a.x, min(p2a.x, min(p3a.x, p4a.x)));
	miny1 = min(p1a.y, min(p2a.y, min(p3a.y, p4a.y)));
	maxx1 = max(p1a.x, max(p2a.x, max(p3a.x, p4a.x)));
	maxy1 = max(p1a.y, max(p2a.y, max(p3a.y, p4a.y)));

	minx2 = min(p1b.x, min(p2b.x, min(p3b.x, p4b.x)));
	miny2 = min(p1b.y, min(p2b.y, min(p3b.y, p4b.y)));
	maxx2 = max(p1b.x, max(p2b.x, max(p3b.x, p4b.x)));
	maxy2 = max(p1b.y, max(p2b.y, max(p3b.y, p4b.y)));

	if(minx2 > maxx1) return false;
	if(minx1 > maxx2) return false;
	if(miny2 > maxy1) return false;
	if(miny1 > maxy2) return false;

	return true;
}