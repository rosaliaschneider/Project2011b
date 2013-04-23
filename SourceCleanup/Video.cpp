#include <RX/VideoDecoder.h>
#include <RX/ImageProcessor.h>
#include <RX/Filter.h>
#include <QDir>
#include "Globals.h"
#include "Video.h"

Video::Video()
: _started(false), _totalFrames(0), _done(false)
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));
	_timer.setInterval(94);
}

Video::~Video()
{
}

void Video::start(std::string filename, std::string folder)
{
	_folder = folder;

	// load first frame and info
	QImage frame((folder+"/frame_0.png").c_str());

	// if first frame doesnt exist, create all
	if(frame.width() == 0)
	{
		int f = -1;
		_decoder.openFile(filename.c_str());
		while(_decoder.seekNextFrame())
		{
			char buffer[30];	
			sprintf(buffer, "/frame_%d.png", ++f);
			_decoder.getFrame(frame);
			frame.save((folder+buffer).c_str());
			++_totalFrames;
		}
	}
	else
		_totalFrames = QDir(QString(_folder.c_str())).count()-2;

	_width = frame.width();
	_height = frame.height();
	_started = true;
	goToFrame(0);
}

void Video::goToFrame(int number)
{
	char buffer[30];
	sprintf(buffer, "/frame_%d.png", number);
	_frame = QImage((_folder+buffer).c_str());
	_frameNumber = number;
	_done = _frameNumber == _totalFrames;
}

void Video::next()
{
	char buffer[30];
	sprintf(buffer, "/frame_%d.png", ++_frameNumber);
	_frame = QImage((_folder+buffer).c_str());
	_done = _frameNumber == _totalFrames;
}

void Video::previous()
{
	char buffer[30];
	sprintf(buffer, "/frame_%d.png", --_frameNumber);
	_frame = QImage((_folder+buffer).c_str());
	_done = _frameNumber == _totalFrames;
}

QImage Video::frame(int frameNumber)
{
	char buffer[30];
	sprintf(buffer, "/frame_%d.png", frameNumber);
	return QImage((_folder+buffer).c_str());
}

void Video::play()
{
	_timer.start();
}

void Video::pause()
{
	_timer.stop();
}

void Video::extrapolateMasks(std::string folder)
{
	// load first frame and info
	QImage mask((folder+"/frame_0.png").c_str());
	if(mask.width() != 0)
		return;
	
	for(int i = 0; i < _totalFrames; ++i)
	{
		goToFrame(i);

		Filter f;
		f.setAvg();

		RX::ImageProcessor::applyFilter(&_frame, f);
		RX::ImageProcessor::applyFilter(&_frame, f);
		RX::ImageProcessor::applyFilter(&_frame, f);
		RX::ImageProcessor::applyFilter(&_frame, f);
		RX::ImageProcessor::applyFilter(&_frame, f);
		RX::ImageProcessor::toAlpha(&_frame);

		char buffer[30];
		sprintf(buffer, "/frame_%d.png", i);
		_frame.save((folder+buffer).c_str());
	}
	_folder = folder;
}