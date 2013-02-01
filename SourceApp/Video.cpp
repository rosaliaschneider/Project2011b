#include <RX/VideoDecoder.h>
#include "Video.h"

Video::Video()
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));
	// hack - I don't know what to do about this
	_timer.setInterval(94);
}

Video::~Video()
{
}

void Video::load(std::string folder)
{
	_folder = folder;

	// check if it already exists
	QImage frame((folder+"/Frames/frame_0.png").c_str());
	if(frame.width() != 0) {
		goToFrame(0);
		return;
	}
	
	// if does not exist, create
	RX::VideoDecoder decoder;
	decoder.openFile((folder+"/Video.avi" ).c_str());

	int f = 0;
	while(decoder.seekNextFrame())
	{
		char buffer[30];
		sprintf(buffer, "/Frames/frame_%d.png", f);
		
		decoder.getFrame(frame);
		frame.save((folder+buffer).c_str());
		++f;
	}
	goToFrame(0);
}

void Video::goToFrame(int number)
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", number);
	_frame = QImage((_folder+buffer).c_str());
	_frameNumber = number;
}

void Video::next()
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", ++_frameNumber);
	_frame = QImage((_folder+buffer).c_str());
}

void Video::play()
{
	_timer.start();
}

void Video::pause()
{
	_timer.stop();
}