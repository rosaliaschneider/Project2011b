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
	// to be called from program to find and match points between two images
	//if(argc > 1)
	//{
	//	std::string str = argv[1];
	//	std::string str2 = argv[2];
	//		
	//	QImage img1(str.c_str());
	//	QImage img2(str2.c_str());
	//
	//	Sift sift;
	//	sift.initialize();
	//	sift.first(img1.width(), img1.height(), (char*)(img1.bits()), GL_RGBA, GL_UNSIGNED_BYTE);
	//	sift.second(img2.width(), img2.height(), (char*)(img2.bits()), GL_RGBA, GL_UNSIGNED_BYTE);
	//	sift.match("match.txt");
	//	return 0;
	//}

	RX::VideoDecoder decoder;
	QImage frame1, frame2;

	//string inputFile = "D:/Research/_Project2011b/Data/Finite.wmv";
	//string outputBaseFile = "D:/Research/_Project2011b/Data/Finite/state";
	string inputFile = "D:/Research/_ Project2011b/Data/Recitation13/Recitation13.wmv";
	string outputBaseFile = "D:/Research/_Project2011b/Data/Recitation13/Sifts/state";
	int size = 480;
	int sizeOriginal = 480;

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
		sift.match(out.c_str(), size, sizeOriginal);

		frame1 = frame2;
		++currentFrame;
	}
	
	return 0;
}