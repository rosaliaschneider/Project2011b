#include <fstream>
#include <vector>
#include <RX/vec3.h>
#include <vector>
#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <RX/SoftwareRenderer.h>
#include <QFileDialog>
#include <QMainWindow>
#include <QtGui/QApplication>
#include "BBox.h"
#include "Interval.h"
#include "Homography.h"

using namespace std;

QString _folder;
QImage _frame;
int _currentFrame, _numFrames, _numBoards;

RX::VideoDecoder _decoderNormal;
vector< RX::mat3 > _homographies;
vector< vector<BBox> > _boards;
vector< vector<Interval> > _intervals;

void loadBoards(std::string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames >> _numBoards;
	_boards.reserve(_numFrames);
	for(int i = 0; i < _numFrames; ++i) 
	{
		std::vector<BBox> boards;
		for(int j = 0; j < _numBoards; ++j) 
		{
			BBox b(4);
			double x, y;
			for(int k = 0; k < 4; ++k) {
				input >> x >> y;
				b.setX(k, x);
				b.setY(k, y);
			}
			boards.push_back(b);
		}
		_boards.push_back(boards);
	}
}

void loadIntervals(std::string filename)
{
	std::ifstream input(filename.c_str());
	
	int nBoards;
	input >> nBoards;
	for(int i = 0; i < nBoards; ++i)
	{
		int intervals;
		input >> intervals;

		_intervals.push_back(vector<Interval>());
		for(int j = 0; j < intervals; ++j)
		{
			int begin, end;
			input >> begin >> end;
			_intervals[i].push_back(Interval(begin, end));
		}
	}
}

void loadHomographies(std::string filename)
{
	std::ifstream input(filename.c_str());
	
	int nFrames;
	input >> nFrames;
	_homographies.reserve(nFrames);
	for(int i = 0; i < nFrames; ++i) 
	{
		RX::mat3 m;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				double value;
				input >> value;
				m.set(j, k, value);
			}
		}
		_homographies.push_back(m);
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QString folder = QFileDialog::getExistingDirectory(NULL, "Load Video");
	if(folder == NULL)
		return 0;

    _decoderNormal.openFile(folder+"/Video.avi");		

	loadBoards((folder+"/Boards.txt").toStdString());
	loadHomographies((folder+"/GlobalHoms.txt").toStdString());
	loadIntervals((folder+"/Intervals.txt").toStdString());

	_currentFrame = -1;

	RX::mat3 hom;
	hom.setIdentity();

	Homography boardHom;
	boardHom.setPoints1(RX::vec2(0, 0), RX::vec2(300, 0), RX::vec2(300, 100), RX::vec2(0, 100));

	unsigned char *_screen = new unsigned char[1000*500*3];
	int w, h;

	for(int i = 0; i < _intervals.size(); ++i) {
		for(int j = 0; j < _intervals[i].size(); ++j) {
			for(int k = 0; k < Resolution::Total; ++k) {
				_intervals[i][j].buffer[k] = new unsigned char[300*100*3];
				memset(_intervals[i][j].buffer[k], 0, 300*100*3);
			}
		}
	}

	while(_decoderNormal.seekNextFrame()) 
	{
		_decoderNormal.getFrame(_frame);

		h = _frame.height();
		w = _frame.width();

		++_currentFrame;
		hom = _homographies[_currentFrame];

		Resolution res;
		for(int board = 0; board < _boards[_currentFrame].size(); ++board)
		{
			int interval = -1;
			for(int j = 0; j < _intervals[board].size(); ++j)
				if(_intervals[board][j].begin <= _currentFrame && _intervals[board][j].end >= _currentFrame)
					interval = j;
			if(interval == -1)
				continue;

			boardHom.setPoints2(_boards[_currentFrame][board].getPoint(0), _boards[_currentFrame][board].getPoint(1), _boards[_currentFrame][board].getPoint(2), _boards[_currentFrame][board].getPoint(3));
			boardHom.estimate();

			// frame position such that board is on 200, 300
			RX::vec3 framePos1 = boardHom.hom().inverse() * hom * RX::vec3(-w/2, h/2, 1);
			RX::vec3 framePos2 = boardHom.hom().inverse() * hom * RX::vec3(-w/2, -h/2, 1);
			RX::vec3 framePos3 = boardHom.hom().inverse() * hom * RX::vec3(w/2, -h/2, 1); 
			RX::vec3 framePos4 = boardHom.hom().inverse() * hom * RX::vec3(w/2, h/2, 1);

			RX::vec3 scaling = boardHom.hom().inverse() * hom * RX::vec3(100, 100, 1);
			res = setResolution(scaling.x);

			// render frame
			memset(_screen, 0, 1000*500*3);
			RX::SoftwareRenderer::render(_frame, framePos1, framePos2, framePos3, framePos4, _screen, 1000, 500, 3, 3);

			for(int i = 0; i < 100; ++i)
			{
				for(int j = 0; j < 300; ++j)
				{
					int r = _screen[((i+250)*1000 + j+500)*3];
					int g =	_screen[((i+250)*1000 + j+500)*3 + 1];
					int b = _screen[((i+250)*1000 + j+500)*3 + 2];

					if(r != 0 || g != 0 || b != 0)
					{
						_intervals[board][interval].buffer[res][(i*300 + j)*3]     = r;
						_intervals[board][interval].buffer[res][(i*300 + j)*3 + 1] = g;
						_intervals[board][interval].buffer[res][(i*300 + j)*3 + 2] = b;
					}
				}
			}
			_intervals[board][interval].usingFrame[res] = _currentFrame;

			if(board == 0 && interval == 0)
			{
				QImage cleanFrameImg(_screen, 1000, 500, QImage::Format_RGB888);
				// save frame
				char buf[50];
				sprintf(buf, "/StillBoards/screen_%d_%d.png", board, _currentFrame);
				cleanFrameImg.save((folder+buf).toAscii());
			}
		}

		if(_currentFrame == 10000)
			break;

		//	
		//	// render frame
		//	memcpy(_screen, _frame.bits(), _frame.width()*_frame.height()*3);
		//	RX::vec3 pos[4];
		//	for(int j = 0; j < 4; ++j) {
		//		pos[j].x = _boards[_currentFrame][board].getX(j);
		//		pos[j].y = _boards[_currentFrame][board].getY(j);
		//		pos[j].z = 1;
		//	}
		//	// render board
		//	RX::SoftwareRenderer::render(RX::vec3(0, 255, 0), pos[0], pos[1], pos[2], pos[3], _screen, _frame.width(), _frame.height(), 3);
		//
		//	// replace greens
		//	for(int i = 0; i < h; ++i) { 
		//		for(int j = 0; j < w; ++j) {
		//			int r = _screen[(i*w + j)*3]; 
		//			int g = _screen[(i*w + j)*3 + 1];
		//			int b = _screen[(i*w + j)*3 + 2];
		//			
		//			if(r == 0 && g == 255 && b == 0)
		//			{
		//				_intervals[board][interval].buffer[(i*w + j)*3]     = _frame.bits()[(i*w + j)*3];
		//				_intervals[board][interval].buffer[(i*w + j)*3 + 1] = _frame.bits()[(i*w + j)*3 + 1];
		//				_intervals[board][interval].buffer[(i*w + j)*3 + 2] = _frame.bits()[(i*w + j)*3 + 2];
		//			}
		//			else
		//			{
		//				_intervals[board][interval].buffer[(i*w + j)*3] = 0;
		//				_intervals[board][interval].buffer[(i*w + j)*3 + 1] = 255;
		//				_intervals[board][interval].buffer[(i*w + j)*3 + 2] = 0;
		//			}
		//		}
		//	}
		//}
	}

	for(int board = 0; board < _intervals.size(); ++board)
	{
		for(int interval = 0; interval < _intervals[board].size(); ++interval)
		{
			for(int k = 0; k < Resolution::Total; ++k)
			{
	//		int minX = w, maxX = 0, minY = h, maxY = 0;
	//		// get min/max for cropping
	//		for(int i = 0; i < h; ++i) { 
	//			for(int j = 0; j < w; ++j) {
	//				int r = _intervals[board][interval].buffer[(i*w + j)*3]; 
	//				int g = _intervals[board][interval].buffer[(i*w + j)*3 + 1];
	//				int b = _intervals[board][interval].buffer[(i*w + j)*3 + 2];
	//
	//				// replace greens
	//				if(r != 0 || g != 255 || b != 0)
	//				{				
	//					if(i < minY) minY = i;
	//					if(i > maxY) maxY = i;
	//					if(j < minX) minX = j;
	//					if(j > maxX) maxX = j;
	//				}
	//			}
	//		}
	//
	//		// crop
	//		// minX = 0;
	//		// minY = 0;
	//		// maxX = w;
	//		// maxY = h;
	//		int cropH = maxY-minY+1;
	//		int cropW = maxX-minX+1;
	//
				//QImage cleanFrameImg(_intervals[board][interval].buffer[k], 300, 100, QImage::Format_RGB888);
				//char buf[50];
				//sprintf(buf, "/StillBoards/board_%d_%d_%d_%d.png", board, interval, k, _intervals[board][interval].usingFrame[k]);
				//cleanFrameImg.save((folder+buf).toAscii());
			}

			int bestRes = _intervals[board][interval].bestRes(300, 100);
			int usingFrame = _intervals[board][interval].usingFrame[bestRes];
			QImage cleanFrameImg2(_intervals[board][interval].buffer[bestRes], 300, 100, QImage::Format_RGB888);
			char buf2[50];
			sprintf(buf2, "/StillBoards/board_%d_%d_%d.png", board, interval, usingFrame);
			cleanFrameImg2.save((folder+buf2).toAscii());

		}
	}

	delete[] _screen;
	for(int i = 0; i < _intervals.size(); ++i)
		for(int j = 0; j < _intervals[i].size(); ++j)
			for(int k = 0; k < Resolution::Total; ++k)
				delete[] _intervals[i][j].buffer[k];

}
