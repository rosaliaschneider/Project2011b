#include <RX/VideoDecoder.h>
#include <QImage>
#include "Globals.h"
#include "Sift.h"
#include "SiftObj.h"

SiftObj::SiftObj()
: _lastReady(-1)
{
}

SiftObj::~SiftObj()
{
}


void SiftObj::process()
{
	RX::VideoDecoder decoder;
	QImage frame1, frame2;

	decoder.openFile(filename);

	int currentFrame = 0;

	Sift sift;
	sift.initialize();

	decoder.seekNextFrame();
	decoder.getFrame(frame1);

	_sifts.push_back(vector<SiftPoint>());
	sift.first(frame1.width(), frame1.height(), (char*)(frame1.bits()), GL_RGB, GL_UNSIGNED_BYTE, 0, &_sifts[0]);

	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame2);

		_sifts.push_back(vector<SiftPoint>());
		sift.second(frame2.width(), frame2.height(), (char*)(frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE, _lastReady+2, &_sifts[_lastReady+2]);

		_matches.push_back(vector< pair<int, int> >());
		sift.match(&_matches[_lastReady+1], &_sifts[_lastReady+2], _sifts[_lastReady+1]);

		frame1 = frame2;
		sift.copySecondToFirst();
		++_lastReady;
	}
}

// Set first frame from match
void SiftObj::setFirstFrameFromMatch(int file, int index)
{
	_sifts[file][_matches[file-1][index].second].firstFrame = _sifts[file-1][_matches[file-1][index].first].firstFrame;
}

const SiftPoint SiftObj::siftPoints(int file, int sift)
{
	return _sifts[file][sift];
}

const double SiftObj::siftPoints(int file, int sift, int xy)
{
	if(xy == 0)
		return _sifts[file][sift].pos.x;
	else
		return _sifts[file][sift].pos.y;
}

const int SiftObj::siftFrame(int file, int sift)
{
	return _sifts[file][sift].firstFrame;
}