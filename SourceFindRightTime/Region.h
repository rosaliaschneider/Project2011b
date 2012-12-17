#ifndef __BBOX_H
#define __BBOX_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>

class BBox
{
public:
	BBox();
	BBox(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);

	void setX(int point, double value);
	void setY(int point, double value);

	double getX(int point);
	double getY(int point);
	RX::vec2 getPoint(int point);

	bool isInside(RX::vec2 point) const;

private:
	std::vector<RX::vec2> _p;
};

class Region
{
public:
	void setColor(RX::vec3 color) { _color = color; }
	void addBox(BBox b) { _boxes.push_back(b); }

	bool isInside(RX::vec2 point) const;

	int colorR() const { return _color.x; }
	int colorG() const { return _color.y; }
	int colorB() const { return _color.z; }

private:
	std::vector<BBox> _boxes;
	RX::vec3 _color;
};



#endif // __BBOX_H