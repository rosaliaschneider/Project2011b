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

	int bpp = frame1.depth()/8;
	for(int i = frame1.height()-1; i > frame1.height()-20; --i) {
		for(int j = frame1.width()-1; j > frame1.width()-130; --j) {
			frame1.bits()[(i*frame1.width() + j)*bpp] = 0;
			frame1.bits()[(i*frame1.width() + j)*bpp + 1] = 0;
			frame1.bits()[(i*frame1.width() + j)*bpp + 2] = 0;
		}
	}

	sift.first(frame1.width(), frame1.height(), (char*)(frame1.bits()), GL_RGB, GL_UNSIGNED_BYTE, 0, &_sifts[0]);

	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame2);

		int bpp = frame2.depth()/8;
		for(int i = frame2.height()-1; i > frame2.height()-20; --i) {
			for(int j = frame2.width()-1; j > frame2.width()-130; --j) {
				frame2.bits()[(i*frame2.width() + j)*bpp] = 0;
				frame2.bits()[(i*frame2.width() + j)*bpp + 1] = 0;
				frame2.bits()[(i*frame2.width() + j)*bpp + 2] = 0;
			}
		}

		sift.second(frame2.width(), frame2.height(), (char*)(frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE, _lastReady+2, &_sifts[_lastReady+2]);

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
