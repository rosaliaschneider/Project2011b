#include <RX/VideoDecoder.h>
#include <QImage>
#include "Globals.h"
#include "Sift.h"
#include "SiftMapObj.h"

SiftMapObj::SiftMapObj()
: _lastReady(-1)
{
}

SiftMapObj::~SiftMapObj()
{
}


void SiftMapObj::process()
{
	RX::VideoDecoder decoder;
	QImage frame2;

	decoder.openFile(filename);

	Sift sift;
	sift.initialize();

	_sifts.push_back(vector<SiftPoint>());

	QImage cMap = clickMap.image();
	sift.first(cMap.width(), cMap.height(), (char*)(cMap.bits()), GL_RGB, GL_UNSIGNED_BYTE, 0, &_sifts[0]);

	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame2);

		_sifts.push_back(vector<SiftPoint>());
		sift.second(frame2.width(), frame2.height(), (char*)(frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE, _lastReady+1, &_sifts[_lastReady+2]);

		_matches.push_back(vector< pair<int, int> >());
		sift.match(&_matches[_lastReady+1], &_sifts[_lastReady+2], _sifts[0]);

		++_lastReady;
	}
}

// Set first frame from match
void SiftMapObj::setFirstFrameFromMatch(int file, int index)
{
	_sifts[file][_matches[file-1][index].second].firstFrame = _sifts[file-1][_matches[file-1][index].first].firstFrame;
}

const SiftPoint SiftMapObj::siftPoints(int file, int sift)
{
	return _sifts[file][sift];
}

const double SiftMapObj::siftPoints(int file, int sift, int xy)
{
	if(xy == 0)
		return _sifts[file][sift].pos.x;
	else
		return _sifts[file][sift].pos.y;
}

const int SiftMapObj::siftFrame(int file, int sift)
{
	return _sifts[file][sift].firstFrame;
}