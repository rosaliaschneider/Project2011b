#include <RX/Quad.h>
#include "Globals.h"
#include "Homography.h"
#include "HomographyMapObj.h"

HomographyMapObj::HomographyMapObj()
: _lastReady(-1), _nextFrame(-1)
{
	log = ofstream("log2.txt");
}

HomographyMapObj::~HomographyMapObj()
{
}

void HomographyMapObj::moveToRight(int index)
{
	RX::mat3 hom;
	hom.setTranslate(RX::vec3(_homography.width(), 0, 0));
	_homographies[index] = hom * _homographies[index];
	_nextFrame = index+1;
}

void HomographyMapObj::process()
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

		if(siftMapObj->lastReady() < _frame-1) {
			--_frame;
			continue;
		}

		// estimate movement from points
		_homography.loadPointsFirstMap(_frame-1);
		_homography.loadPointsSecond(_frame-1, true);
		_homography.all();

		_hom.setIdentity();
		if(_homography.numPoints() >= 50)
		{
			_homography.estimate();
			if(_homography.numInliers() >= 30)
				_hom = _homography.hom();
		}
		_homographies[_frame] = _hom;

		// keep global transformed sifts for click navigation
		//int w = _homography.width();
		//int h = _homography.height();
		//RX::Quad<RX::vec2> frameQuad(RX::vec2(-w/2, -h/2), RX::vec2(-w/2, h/2), RX::vec2(-w/2, h/2), RX::vec2(w/2, -h/2));
		//frameQuad.transform(_homographies[_homographies.size()-1]);
		//for(int i = 0; i < 4; ++i)
		//	frameQuad.setPoint(i, frameQuad.getPoint(i)+RX::vec2(w/2, h/2));
		//
		//_hasSifts.clear();

		// 1. remove old sifts that are covered by the new frame
		//for(int i = 0; i < _globalSifts[_frame-1].size(); ++i)
		//{
		//	SiftPoint p = _globalSifts[_frame-1][i];
		//	if(!(frameQuad.isInside(p.pos))) {
		//		_globalSifts[_frame].push_back(p);
		//		_hasSifts[std::pair<int, int>(p.pos.x, p.pos.y)] = true;
		//	}
		//}
		//
		//// 2. add new sifts
		//for(int i = 0; i < _homography.numPoints(); ++i)
		//{
		//	SiftPoint p = siftMapObj->siftPoints(_frame, siftMapObj->match(_frame-1, i).second);
		//	if(_hasSifts.find(std::pair<int, int>(p.pos.x, p.pos.y)) == _hasSifts.end()) {
		//		_globalSifts[_frame].push_back(p);
		//		_hasSifts[std::pair<int, int>(p.pos.x, p.pos.y)] = true;
		//	}
		//}

		++_lastReady;
	}
}