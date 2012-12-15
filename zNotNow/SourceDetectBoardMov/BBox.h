#ifndef __BOX_H
#define __BOX_H

#include <vector>
#include <RX/vec2.h>

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

	void addPoint(int id, float x, float y)
	{
		p.insert(p.begin()+id, RX::vec2(x, y));
	}

	void deletePoint(int id)
	{
		p.erase(p.begin()+id);
	}

	void setX(int point, double value) 
	{ 
		p[point].x = value; 
		centroid.x = 0;
		for(int i = 0; i < p.size(); ++i)
			centroid.x += p[i].x;
		centroid.x /= p.size();
	}
	void setY(int point, double value) 
	{ 
		p[point].y = value;
		centroid.y = 0;
		for(int i = 0; i < p.size(); ++i)
			centroid.y += p[i].y;
		centroid.y /= p.size();
	}

	double getX(int point) { return p[point].x; }
	double getY(int point) { return p[point].y; }
	double getCX() { return centroid.x; }
	double getCY() { return centroid.y; }
	int nPoints() { return p.size(); }

	void setCentroid(double x, double y)
	{
		for(int i = 0; i < p.size(); ++i)
		{
			p[i].x = p[i].x - centroid.x + x;
			p[i].y = p[i].y - centroid.y + y;
		}
		centroid.x = x;
		centroid.y = y;
	}

private:
	std::vector<RX::vec2> p;
	RX::vec2 centroid;
};

#endif // __BBOX_H