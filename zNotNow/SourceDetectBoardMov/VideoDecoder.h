#ifndef __VIDEODECODER_H
#define __VIDEODECODER_H

#include <string>
#include <list>
#include <QImage>

using namespace std;

class VideoDecoder
{
public:
	VideoDecoder();
	~VideoDecoder();

	void load(string filename, int numFrames);
	void loadNewFrames();

	bool getNextFrame(QImage *_frame);
	bool getFrame(int frame, QImage *_frame);

private:
	list<QImage> _buffer;
	int _currentFrame, _totalFrames;
	string _folder;
};


#endif  // __VIDEODECODER_H