#include <RX/Quad.h>
#include "Globals.h"
#include "Homography.h"
#include "HomographyObj.h"

HomographyObj::HomographyObj()
: _lastReady(-1), _nextFrame(-1)
{
	log = ofstream("GlobalSifts.txt");
}

HomographyObj::~HomographyObj()
{
}

void HomographyObj::moveToRight(int index)
{
	RX::mat3 hom;
	hom.setTranslate(RX::vec3(_homography.width(), 0, 0));
	_homographies[index] = hom * _homographies[index];
	_nextFrame = index+1;
}

void HomographyObj::process()
{
	RX::mat3 _hom;

	_homographies.resize(video.totalFrames());

	// start
	_hom.setIdentity();
	_homographies[0] = _hom;
	++_lastReady;
	
	// get info
	_homography.setWidth(video.width());
	_homography.setHeight(video.height());

	for(_frame = 1; _frame < video.totalFrames(); ++_frame)
	{
		if(_nextFrame != -1)
		{
			_lastReady = _nextFrame-1;
			_frame = _nextFrame;
			_nextFrame = -1;
		}
		
		if(siftObj->lastReady() < _frame-1) {
			--_frame;
			continue;
		}
		
		// estimate movement from points
		_homography.loadPointsFirst(_frame-1);
		_homography.loadPointsSecond(_frame-1);
		_homography.all();
		
		_hom.setIdentity();
		if(_homography.numPoints() >= 50)
		{
			_homography.estimate();
			if(_homography.numInliers() >= 30)
				_hom = _homography.hom();
		}
		_homographies[_frame] = _homographies[_frame-1] * _hom;

		// add new sifts
		for(int i = 0; i < _homography.numPoints(); ++i)
		{
			SiftPoint p = _sifts[_frame][_matches[_frame-1][i].second];

			RX::vec3 finalPos = _homographies[_frame]*RX::vec3(p.pos, 1);
			finalPos.divideByZ();
			p.pos = finalPos.xy();

			if(_hasSifts.find(std::pair<int, int>(p.pos.x/10, p.pos.y/10)) == _hasSifts.end()) {
				_globalSifts.push_back(p);
				_hasSifts[std::pair<int, int>(p.pos.x/10, p.pos.y/10)] = true;
			}
		}

		++_lastReady;
	}

	log << _globalSifts.size() << endl;
	for(int i = 0; i < _globalSifts.size(); ++i)
		log << _globalSifts[i].pos.x << " " << _globalSifts[i].pos.y << endl;
	for(int i = 0; i < _globalSifts.size(); ++i)
		log << _globalSifts[i].firstFrame << endl;
}