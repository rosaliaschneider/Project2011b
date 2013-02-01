#include "Globals.h"
#include "Homography.h"
#include "HomographyObj.h"

HomographyObj::HomographyObj()
: _lastReady(-1)
{
}

HomographyObj::~HomographyObj()
{
}


void HomographyObj::process()
{
	Homography _homography;
	RX::mat3 _hom;
	
	_homography.loadInfo(_folder+"/Info.txt");
	//_homography.setSiftFolder(_folder+"/Sifts/");

	_hom.setIdentity();
	_homographies.push_back(_hom);
	++_lastReady;

	for(int frame = 1; frame < _homography.numFrames(); ++frame)
	{
		if(siftObj->lastReady() < frame-1) {
			--frame;
			continue;
		}

		// estimate movement from points
		_homography.loadPoints(frame-1, true);
		_homography.all();

		_hom.setIdentity();
		if(_homography.numPoints() >= 30)
		{
			_homography.estimate();
			if(_homography.numInliers() >= 20)
				_hom = _homography.hom();
		}
		_homographies.push_back(_homographies[_homographies.size()-1] * _hom);
		++_lastReady;
	}
}