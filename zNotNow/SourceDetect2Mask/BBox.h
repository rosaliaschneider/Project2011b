#ifndef __BOX_H
#define __BOX_H

class BBox
{
public:
	void setX(int point, double value) 
	{ 
		p[point].x = value; 
		centroid.x = (p[0].x + p[1].x + p[2].x + p[3].x)/4.0;
	}
	void setY(int point, double value) 
	{ 
		p[point].y = value; 
		centroid.y = (p[0].y + p[1].y + p[2].y + p[3].y)/4.0;
	}

	double getX(int point) { return p[point].x; }
	double getY(int point) { return p[point].y; }
	double getCX() { return centroid.x; }
	double getCY() { return centroid.y; }

	RX::vec2 getP(int point) { return p[point]; }

	void setCentroid(double x, double y)
	{
		for(int i = 0; i < 4; ++i)
		{
			p[i].x = p[i].x - centroid.x + x;
			p[i].y = p[i].y - centroid.y + y;
		}
		centroid.x = x;
		centroid.y = y;
	}

private:
	RX::vec2 p[4];
	RX::vec2 centroid;
};

#endif // __BBOX_H