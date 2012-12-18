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
	void setUsed(bool used) { _used = used; }
	void setSeen(bool seen) { _seen = seen; }
	void setStartingFrame(int startingFrame) { _startingFrame = startingFrame; }

	bool used() const { return _used; }
	bool seen() const { return _seen; }
	int startingFrame() { return _startingFrame; }
	double getX(int point);
	double getY(int point);
	RX::vec2 getPoint(int point);

	bool isInside(RX::vec2 point) const;

private:
	std::vector<RX::vec2> _p;
	bool _used, _seen;
	int _startingFrame;

};

class Region
{
public:
	void addBoxes(std::vector<BBox> boxes);
	void addBox(BBox b) { _boxes.push_back(b); }

	bool isInside(RX::vec2 point) const;

	int colorR() const { return _color.x; }
	int colorG() const { return _color.y; }
	int colorB() const { return _color.z; }
	int startingFrame() const { return _startingFrame; }
	std::vector<BBox> boxes() { return _boxes; }

	void setColor(RX::vec3 color) { _color = color; }
	void setStartingFrame(int startingFrame) { _startingFrame = startingFrame; }

private:
	int _startingFrame;
	std::vector<BBox> _boxes;
	RX::vec3 _color;
};



#endif // __BBOX_H