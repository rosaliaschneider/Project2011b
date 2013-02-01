#ifndef __REGION_H
#define __REGION_H

#include <vector>
#include <RX/vec2.h>
#include <RX/vec3.h>
#include <RX/BBox.h>
#include <QImage>

class Region
{
public:
	void addBoxes(std::vector<RX::BBox> boxes);
	void addBox(RX::BBox b) { _boxes.push_back(b); }

	bool isInside(RX::vec2 point) const;

	// Gets
	int startingFrame() const { return _startingFrame; }
	int nBoxes() const { return _boxes.size(); }
	RX::vec3 color() const { return _color; }
	std::vector<RX::BBox> boxes() { return _boxes; }
	RX::BBox box(int boxNum) { return _boxes[boxNum]; }
	QImage image() { return _image; }

	// Sets
	void setStartingFrame(int startingFrame) { _startingFrame = startingFrame; }
	void setColor(RX::vec3 color) { _color = color; }
	void setImage(QImage image) { _image = image; }

private:
	int _startingFrame;
	std::vector<RX::BBox> _boxes;
	RX::vec3 _color;
	QImage _image;
};



#endif // __REGION_H