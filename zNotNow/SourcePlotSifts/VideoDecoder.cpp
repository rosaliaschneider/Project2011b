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
		string file = _folder+"\\state_";
		char buf[10];
		sprintf(buf, "%d", _currentFrame);
		file += buf;
		file += ".png";
		++_currentFrame;

		QImage img(file.c_str());
		_buffer.push_back(img);
	}
}

bool VideoDecoder::getNextFrame(QImage *_frame)
{
	if(_buffer.size() == 0)
		if(_currentFrame == _totalFrames)
			return false;
		else
			loadNewFrames();

	*_frame = _buffer.front();
	_buffer.pop_front();

	// big hack to avoid QImage memory leaking
	for(int i = 0; i < _frame->height(); ++i)
	{
		for(int j = 0; j < _frame->width(); ++j)
		{
			_frame->bits()[(i*_frame->width()+j)*3] = _frame->bits()[(i*_frame->width()+j)*4];
			_frame->bits()[(i*_frame->width()+j)*3 + 1] = _frame->bits()[(i*_frame->width()+j)*4 + 1];
			_frame->bits()[(i*_frame->width()+j)*3 + 2] = _frame->bits()[(i*_frame->width()+j)*4 + 2];
		}
	}

	return true;
}