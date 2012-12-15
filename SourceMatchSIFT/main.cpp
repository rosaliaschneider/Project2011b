#include <iostream>
#include <fstream>
#include <RX/VideoDecoder.h>
#include "Sift.h"

#ifdef _WIN32
#include <gl/glew.h>
#endif

using namespace std;

int main(int argc, char **argv)
{
	RX::VideoDecoder decoder;
	QImage frame1, frame2;

	//string inputFile = "D:/Research/Project2011b/Data/Poisson/Original.m4v";
	//string outputBaseFile = "D:/Research/Project2011b/Data/Poisson/Sifts/state";
	//string inputFile = "D:/Research/Project2011b/Data/Poisson/Original.m4v";
	//string outputBaseFile = "D:/Research/Project2011b/Data/Poisson/Sifts/state";
	//string inputFile = "D:/Research/Project2011b/Data/Probability/Video.avi";
	//string outputBaseFile = "D:/Research/Project2011b/Data/Probability/Sifts/state";
	string inputFile = "D:/Research/Project2011b/Data/Separable/Video.avi";
	string outputBaseFile = "D:/Research/Project2011b/Data/Separable/Sifts/state";

	decoder.openFile(inputFile.c_str());

	int currentFrame = 0;

	Sift sift;
	sift.initialize();

	decoder.seekNextFrame();
	decoder.getFrame(frame1);
	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame2);
		
		std::string out = outputBaseFile;
		char buf[40];
		sprintf(buf, "_%d.txt", currentFrame);
		out += buf;
		
		sift.first(frame1.width(), frame1.height(), (char*)(frame1.bits()), GL_RGB, GL_UNSIGNED_BYTE);
		sift.second(frame2.width(), frame2.height(), (char*)(frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE);
		sift.match(out.c_str());

		frame1 = frame2;
		++currentFrame;
	}
	
	return 0;
}