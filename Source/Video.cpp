#include <RX/VideoDecoder.h>
#include <QDir>
#include "Globals.h"
#include "Video.h"

Video::Video()
: _started(false), _maxFrameSeen(-1)
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));
	_timer.setInterval(30);
}

Video::~Video()
{
}

void Video::start(std::string folder)
{
	// load first frame and info
	QImage frame((folder+"/Frames/frame_0.png").c_str());

	// if first frame doesnt exist, create all
	if(frame.width() == 0)
	{
		int f = -1;
		_decoder.openFile(filename.toAscii());
		while(_decoder.seekNextFrame())
		{
			char buffer[30];	
			sprintf(buffer, "/Frames/frame_%d.png", ++f);
			_decoder.getFrame(frame);
			frame.save((folder+buffer).c_str());
			++_totalFrames;
		}
	}
	else
		_totalFrames = QDir(QString((folder+"/Frames").c_str())).count()-2;

	_width = frame.width();
	_height = frame.height();
	_started = true;

	markLabels();
	goToFrame(0);
}

void Video::goToFrame(int number)
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", number);
	_frame = QImage(folder+buffer);
	_frameNumber = number;

	if(_frameNumber > _maxFrameSeen)
		_maxFrameSeen = _frameNumber;
}

void Video::next()
{
	if(homObj && homObj->lastReady() < _frameNumber)
		return;
	
	if(homMapObj && homMapObj->lastReady() < _frameNumber)
		return;

	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", ++_frameNumber);
	_frame = QImage(folder+buffer);
	
	if(_frameNumber > _maxFrameSeen)
		_maxFrameSeen = _frameNumber;
}

void Video::previous()
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", --_frameNumber);
	_frame = QImage(folder+buffer);
}

QImage Video::frame(int frameNumber)
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", frameNumber);
	return QImage(folder+buffer);
}

void Video::play()
{
	_timer.start();
}

void Video::pause()
{
	_timer.stop();
}

void Video::markLabels()
{
	_labels = QImage(width(), height(), QImage::Format_RGB32);
	
	unsigned char *bits = _labels.bits();
	memset(bits, 0, width()*height()*4);
	for(int i = height()-1; i > height()-20; --i) {
		for(int j = width()-1; j > width()-130; --j) {
			bits[(i*width() + j)*4]     = 255;
			bits[(i*width() + j)*4 + 1] = 255;
			bits[(i*width() + j)*4 + 2] = 255;
			bits[(i*width() + j)*4 + 3] = 255;
		}
	}
}