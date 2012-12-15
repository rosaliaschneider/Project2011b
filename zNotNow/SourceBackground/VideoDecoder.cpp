#include "VideoDecoder.h"

VideoDecoder::VideoDecoder()
{
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::load(string filename, int numFrames)
{
	_folder = filename;
	_currentFrame = 0;
	_totalFrames = numFrames;
	loadNewFrames();
}

void VideoDecoder::loadNewFrames()
{
	for(int i = 0; i < 15 && _currentFrame < _totalFrames; ++i) {
		string file = _folder;
		char buf[50];
		sprintf(buf, "\\Replace\\frame_%d.png", _currentFrame);
		file += buf;
		
		QImage img(file.c_str());
		_buffer.push_back(img);
		++_currentFrame;
	}
}

bool VideoDecoder::getNextFrame(QImage *frame)
{
	if(_buffer.size() == 0)
		if(_currentFrame == _totalFrames)
			return false;
		else
			loadNewFrames();

	*frame = _buffer.front();
	_buffer.pop_front();
	return true;
}