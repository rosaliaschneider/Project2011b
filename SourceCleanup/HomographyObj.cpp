#include "Globals.h"
#include "Homography.h"
#include "HomographyObj.h"

HomographyObj::HomographyObj()
: _lastReady(-1), _nextFrame(-1)
{
}

HomographyObj::~HomographyObj()
{
}

void HomographyObj::process()
{
	RX::mat3 _hom;

	_homographies.resize(7000);

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
		if(_homography.numPoints() >= 30)
		{
			_homography.estimate();
			if(_homography.numInliers() >= 25)
				_hom = _homography.hom();
		}
		_homographies[_frame] = _hom;

		++_lastReady;
	}
}