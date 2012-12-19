#ifndef __BOX_H
#define __BOX_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>

extern bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b);
extern bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p);

class BBox
{
public:
	BBox()
	{
		for(int i = 0; i < 4; ++i)
			p.push_back(RX::vec2(0, 0));
	}

	BBox(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
	{
		p.push_back(p1);
		p.push_back(p2);
		p.push_back(p3);
		p.push_back(p4);
	}

	bool isInside(RX::vec2 point)
	{
		if((pointInsideTriangle(p[0], p[1], p[2], point))||(pointInsideTriangle(p[0], p[2], p[3], point)))
		{
			return true;
		}
		return false;
	}

	void setX(int point, double value) 
	{ 
		p[point].x = value; 
	}
	void setY(int point, double value) 
	{ 
		p[point].y = value;
	}

	double getX(int point) { return p[point].x; }
	double getY(int point) { return p[point].y; }
	int nPoints() { return p.size(); }

private:
	std::vector<RX::vec2> p;
};

#endif // __BBOX_H