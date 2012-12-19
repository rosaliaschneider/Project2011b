#ifndef __HOMOGRAPHY_H
#define __HOMOGRAPHY_H

#include <vector>
#include <RX/mat3.h>
#include <RX/vec2.h>
#include "Board.h"
#include "Ransac.h"
using namespace std;

struct SiftPoint
{
	RX::vec2 pos;
	double size, orientation;
	float descriptor[128];
};

class Homography
{
public:
	Homography();
	~Homography();

	void setSiftFolder(std::string folder) { _folder = folder; }

	bool isReliable() { return _isReliable; }
	bool updateMovement(int currentFrame);

	void loadInfo(std::string filename);
	void loadPoints(int file1, bool inverted=false);
	void estimate(int *noi=NULL, int *nof=NULL, double threshold=0.00001);

	// select which points to use
	void all();

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
	
	RX::mat3 _outsideFirst;
	
	vector<Board> _boards;
	vector<Movement> _currMovements, _movements;
	vector<bool> _inliers;

	int _numFrames;
	int _w, _h;
	bool _isReliable;

	std::string _folder;
};

#endif // __HOMOGRAPHY_H
