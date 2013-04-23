#ifndef __BBOX_H
#define __BBOX_H

#include <vector>
#include <RX/vec2.h>

class BBox
{
public:
	BBox();
	BBox(int size);
	BBox(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);

	void setX(int point, double value);
	void setY(int point, double value);

	double getX(int point);
	double getY(int point);
	RX::vec2 getPoint(int point);
	int nPoints();

	void transform(RX::mat3 matrix);

private:
	std::vector<RX::vec2> _p;
};


#endif // __BBOX_H