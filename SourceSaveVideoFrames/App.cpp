#include <iostream>
#include <fstream>
#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <RX/SoftwareRenderer.h>
#include "Board.h"

using namespace std;

int _numFrames;
std::vector<RX::mat3> _homographies;
std::vector<Board> _finalBoards;

void loadHomographies(string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames;
	for(int i = 0; i < _numFrames; ++i)
	{
		RX::mat3 m;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				double d;
				input >> d;
				m.set(j, k, d);
			}
		}
		_homographies.push_back(m);
	}
}

void loadBoards(string filename)
{
	std::ifstream input(filename);
		
	int nboards, nframes;
	input >> nframes >>  nboards;

	_finalBoards.clear();
	_finalBoards.resize(nboards);

	vector<Board> boards(nboards, nframes);
	for(int j = 0; j < nframes; ++j) 
	{
		for(int i = 0; i < nboards; ++i) 
		{
			for(int k = 0; k < 4; ++k)
			{
				double x, y;
				input >> x >> y;
				boards[i].setPos(x, y, k, j);
			}
		}
	}	
	for(int i = 0; i < boards.size(); ++i)
		_finalBoards[i] = boards[i];
}

void saveBoards(string filename)
{
	std::ofstream output(filename);

	output << _numFrames << ' ' << _finalBoards.size() << endl;
	for(int i = 0; i < _numFrames; ++i) 
	{
		for(int j = 0; j < _finalBoards.size(); ++j) 
		{
			BBox b = _finalBoards[j].getPosition(i);
			for(int k = 0; k < 4; ++k)
			{
				output << b.points[k].x << ' ' << b.points[k].y << ' ';
			}
			output << endl;
		}
	}	
}

int main(int argc, char **argv)
{
	RX::VideoDecoder decoder;
	QImage frame;

	string baseFile = "D:/Research/Project2011b/Data/Probability/"; 
	string inputFile = baseFile+"Video.avi";
	string inputHomFile = baseFile+"GlobalHoms.txt";
	string inputBoardFile = baseFile+"Boards.txt";
	string outputBoardFile = baseFile+"FinalBoards.txt";
	string outputBaseFile = baseFile+"Frames/frame";

	loadHomographies(inputHomFile);
	loadBoards(inputBoardFile);

	unsigned char *buffer = new unsigned char[2000*2000*4];
	memset(buffer, 0, 2000*2000*4);

	int w, h;
	int newWidth, newHeight;
	int minX = 99999, minY = 99999, maxX = -99999, maxY = -99999;

	bool first = true;
	int currentFrame = 0;
	decoder.openFile(inputFile.c_str());
	while(decoder.seekNextFrame())
	{
		decoder.getFrame(frame);

		if(first)
		{
			w = frame.width();
			h = frame.height();
			for(int i = 0; i < _numFrames; ++i)
			{
				RX::vec3 frameMin = _homographies[i] * RX::vec3(-w/2, -h/2, 1);
				RX::vec3 frameMax = _homographies[i] * RX::vec3(w/2, h/2, 1);
				frameMin.divideByZ();
				frameMax.divideByZ();

				if(minX > frameMin.x) minX = frameMin.x;
				if(minY > frameMin.y) minY = frameMin.y;
				if(maxX < frameMax.x) maxX = frameMax.x;
				if(maxY < frameMax.y) maxY = frameMax.y;

			}
			newWidth = maxX-minX+1;
			newHeight = maxY-minY+1;
			first = false;
			//break;
		}
		
		std::string out = outputBaseFile;
		char buf[40];
		sprintf(buf, "_%d.png", currentFrame);
		out += buf;

		RX::vec3 framePos1 = _homographies[currentFrame] * RX::vec3(-w/2, h/2, 1);
		RX::vec3 framePos2 = _homographies[currentFrame] * RX::vec3(-w/2, -h/2, 1);
		RX::vec3 framePos3 = _homographies[currentFrame] * RX::vec3(w/2, -h/2, 1);
		RX::vec3 framePos4 = _homographies[currentFrame] * RX::vec3(w/2, h/2, 1);
		framePos1.divideByZ();
		framePos2.divideByZ();
		framePos3.divideByZ();
		framePos4.divideByZ();

		RX::SoftwareRenderer::render(frame, framePos1, framePos2, framePos3, framePos4, buffer, 2000, 2000, 3, 4);

		QImage newFrame(newWidth, newHeight, QImage::Format_RGB32);
		memset(newFrame.bits(), 0, newWidth*newHeight*4);

		for(int i = minY; i <= maxY; ++i)
		{
			for(int j = minX; j <= maxX; ++j)
			{
				newFrame.bits()[((i-minY)*newWidth + (j-minX))*4 + 2] = buffer[((i+1000)*2000 + j + 1000)*4];
				newFrame.bits()[((i-minY)*newWidth + (j-minX))*4 + 1] = buffer[((i+1000)*2000 + j + 1000)*4 + 1];
				newFrame.bits()[((i-minY)*newWidth + (j-minX))*4]     = buffer[((i+1000)*2000 + j + 1000)*4 + 2];
				newFrame.bits()[((i-minY)*newWidth + (j-minX))*4 + 3] = buffer[((i+1000)*2000 + j + 1000)*4 + 3];
			}
		}

		newFrame.save(out.c_str());
		++currentFrame;
	}
	for(int i = 0; i < _finalBoards.size(); ++i)
	{
		for(int j = 0; j < _numFrames; ++j)
		{
			BBox b = _finalBoards[i].getPosition(j);
			for(int k = 0; k < 4; ++k)
			{
				b.points[k].x = b.points[k].x + w/2 - newWidth/2;
				b.points[k].y = b.points[k].y + h/2 - newHeight/2;
			}
			_finalBoards[i].setPos(b, j);
		}
	}
	saveBoards(outputBoardFile);

	delete[] buffer;
	return 0;
}