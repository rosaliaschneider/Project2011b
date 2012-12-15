#ifndef __BBOX_H
#define __BBOX_H

#include <vector>
#include <RX/vec2.h>

class BBox
{
public:
	BBox();
	BBox(int size);

	void setX(int point, double value);
	void setY(int point, double value);

	double getX(int point);
	double getY(int point);
	int nPoints();

	void transform(RX::mat3 matrix);

private:
	std::vector<RX::vec2> _p;
};


#endif // __BBOX_H