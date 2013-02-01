#include <RX/VideoDecoder.h>
#include <QImage>
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

	string inputFile = "D:/Research/Project2011b/Data/Probability/Video.avi";
	decoder.openFile(inputFile.c_str());

	int currentFrame = 0;

	Sift sift;
	sift.initialize();

	decoder.seekNextFrame();
	decoder.getFrame(frame1);

	_sifts.push_back(vector<SiftPoint>());
	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame2);

		sift.first(frame1.width(), frame1.height(), (char*)(frame1.bits()), GL_RGB, GL_UNSIGNED_BYTE, &_sifts[_lastReady+1]);
		
		_sifts.push_back(vector<SiftPoint>());
		sift.second(frame2.width(), frame2.height(), (char*)(frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE, &_sifts[_lastReady+2]);

		_matches.push_back(vector< pair<int, int> >());
		sift.match(&_matches[_lastReady+1]);

		frame1 = frame2;
		++_lastReady;
	}
}

const double SiftObj::siftPoints(int file, int sift, int xy)
{
	if(xy == 0)
		return _sifts[file][sift].pos.x;
	else
		return _sifts[file][sift].pos.y;
}