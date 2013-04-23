#ifndef __HOMOGRAPHY_H
#define __HOMOGRAPHY_H

#include <vector>
#include <RX/mat3.h>
#include <RX/vec2.h>
#include "Ransac.h"
using namespace std;

class Homography
{
public:
	Homography();
	~Homography();

	void setSiftFolder(std::string siftFolder) { _siftFolder = siftFolder; }
	void setWidth(int width);
	void setHeight(int height);

	void loadPointsFirst(int file);
	void loadPointsSecond(int file);
	void estimate(int *noi=NULL, int *nof=NULL, double threshold=0.0001);

	// update sifts
	void updateSifts(int file);

	// select which points to use
	void all();
	int numPoints() { return _x1[0].size(); }
	int numInliers();

	inline RX::mat3 hom() { return _hom; }
	inline vector<bool> inliers() { return _inliers; }
	inline int numFrames() const { return _numFrames; }

	int width() { return _w; }
	int height() { return _h; }

	void drawUsedPoints(); 
	void drawAllPoints();

private:
	RX::mat3 estimatePriv();

	RX::mat3 _hom;
	matd _x1, _x2;
	matd _ux1, _ux2;
	int _n, _nused;
	
	vector<bool> _inliers;

	int _numFrames;
	int _w, _h;
	
	std::string _siftFolder;
};

#endif // __HOMOGRAPHY_H