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

	void loadInfo(std::string filename);
	void loadPoints(int file1, bool inverted=false);
	void saveInliers(int file1, bool inverted=false);
	void estimate(int *noi=NULL, int *nof=NULL, double threshold=0.0001);

	// select which points to use
	void all();
	int numPoints() { return _x1[0].size(); }
	int numInliers();

	inline RX::mat3 hom() { return _hom; }
	inline vector<bool> inliers() { return _inliers; }
	inline int numFrames() const { return _numFrames; }

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
