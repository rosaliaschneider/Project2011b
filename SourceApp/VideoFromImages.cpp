#include <RX/VideoDecoder.h>
#include "VideoFromImages.h"

VideoFromImages::VideoFromImages()
{
}

VideoFromImages::~VideoFromImages()
{
}

void VideoFromImages::createFromVideo(std::string folder, std::string filename)
{
	_folder = folder;

	RX::VideoDecoder decoder;
	decoder.openFile((folder+filename).c_str());

	QImage frame;
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

void VideoFromImages::goToFrame(int number)
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", number);
	_frame = QImage((_folder+buffer).c_str());
	_frameNumber = number;
}

void VideoFromImages::next()
{
	char buffer[30];
	sprintf(buffer, "/Frames/frame_%d.png", ++_frameNumber);
	_frame = QImage((_folder+buffer).c_str());
}