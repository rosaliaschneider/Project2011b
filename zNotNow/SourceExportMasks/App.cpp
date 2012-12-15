#include <fstream>
#include <vector>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <RX/SoftwareRenderer.h>
#include <QFileDialog>
#include <QMainWindow>
#include <QtGui/QApplication>
#include "BBox.h"

QString _folder;
QImage _frame, _alpha;
int _currentFrame, _numFrames;
RX::VideoDecoder _decoderNormal;
RX::VideoDecoder _decoderAlpha;
std::vector< RX::mat3 > _homographies;
std::vector< std::vector<BBox> > _boards;

void loadInfo(std::string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames;
	int dimX, dimY;
	input >> dimX >> dimY;
}

void loadBoards(std::string filename)
{
	std::ifstream input(filename.c_str());
	
	int nFrames, nBoards;
	input >> nFrames >> nBoards;
	_boards.reserve(nFrames);
	for(int i = 0; i < nFrames; ++i) 
	{
		std::vector<BBox> boards;
		for(int j = 0; j < nBoards; ++j) 
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

    _decoderNormal.openFile(folder+"/Video.wmv");		
	_decoderAlpha.openFile(folder+"/Alpha.avi");

	loadInfo((folder+"/_MoveInfo.txt").toStdString());
	loadBoards((folder+"/_LocalBoards.txt").toStdString());
	loadHomographies((folder+"/_LocalHoms.txt").toStdString());
	
	_currentFrame = -1;

	RX::mat3 hom;
	hom.setIdentity();

	unsigned char *_screen;

	while(_decoderNormal.seekNextFrame() && _decoderAlpha.seekNextFrame()) 
	{
		_decoderNormal.getFrame(_frame);
		_decoderAlpha.getFrame(_alpha);
		
		if(_currentFrame == -1)	
			_screen = new unsigned char[_frame.width()*_frame.height()*3];
		++_currentFrame;

		int h = _frame.height();
		int w = _frame.width();

		RX::vec3 framePos1 = RX::vec3(0, h, 1);
		RX::vec3 framePos2 = RX::vec3(0, 0, 1);
		RX::vec3 framePos3 = RX::vec3(w, 0, 1);
		RX::vec3 framePos4 = RX::vec3(w, h, 1);

		// render frame
		RX::SoftwareRenderer::render(_frame, framePos1, framePos2, framePos3, framePos4, _screen, _frame.width(), _frame.height(), 3, 3, false);
									
		// draw boards
		hom = _homographies[_currentFrame].inverse() * hom;
		for(int i = 0; i < _boards[_currentFrame].size(); ++i)
		{
			_boards[_currentFrame][i].transform(hom);
			
			RX::vec3 pos[4];
			for(int j = 0; j < 4; ++j) {
				pos[j].x = _boards[_currentFrame][i].getX(j);
				pos[j].y = _boards[_currentFrame][i].getY(j);
				pos[j].z = 1;
			}
			RX::SoftwareRenderer::render(RX::vec3(0, 255, 0), pos[0], pos[1], pos[2], pos[3], _screen, _frame.width(), _frame.height(), 3);
		}

		for(int i = 0; i < _alpha.height(); ++i) {
			for(int j = 0; j < _alpha.width(); ++j) {
				if(_alpha.bits()[(i*_alpha.width() + j)*3] != 0 || _alpha.bits()[(i*_alpha.width() + j)*3 + 1] != 0 || _alpha.bits()[(i*_alpha.width() + j)*3 + 2] != 0) {
					_alpha.bits()[(i*_alpha.width() + j)*3]     = 255;
					_alpha.bits()[(i*_alpha.width() + j)*3 + 1] = 0;
					_alpha.bits()[(i*_alpha.width() + j)*3 + 2] = 255;
				}
			}
		}
		RX::SoftwareRenderer::renderNoColorA(_alpha, RX::vec3(0, 0, 0), framePos1, framePos2, framePos3, framePos4, _screen, _frame.width(), _frame.height(), 3, 3, false);
			
		// save frame
		unsigned char *cleanFrame = new unsigned char[w*h*3];
		//glReadPixels(_width/2-w/2, _height/2-h/2, w, h, GL_RGB, GL_UNSIGNED_BYTE, cleanFrame);
		for(int i = 0; i < h; ++i) {
			for(int j = 0; j < w; ++j) {
				cleanFrame[(i*w + j)*3]     = _screen[(i*w + j)*3];
				cleanFrame[(i*w + j)*3 + 1] = _screen[(i*w + j)*3 + 1];
				cleanFrame[(i*w + j)*3 + 2] = _screen[(i*w + j)*3 + 2];
			}
		}

		char buf[50];
		sprintf(buf, "/Replace/frame_%d.png", _currentFrame);
		QImage cleanFrameImg(cleanFrame, w, h, QImage::Format_RGB888);
		cleanFrameImg.save((folder+buf).c_str());
			
		char buf2[50];
		sprintf(buf2, "/Mask/frame_%d.png", _currentFrame);
		QImage cleanFrameImg2(_frame.bits(),  w, h, QImage::Format_RGB888);

		for(int i = 0; i < cleanFrameImg2.height(); ++i) {
			for(int j = 0; j < cleanFrameImg2.width(); ++j) {
				int r = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3]; 
				int g = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3 + 1];
				int b = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3 + 2];
				if((r == 255 && g == 0 && b == 255)||(r == 0 && g == 255 && b == 0))
				{
				}
				else {
					cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3] = 0;
					cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3 + 1] = 255;
					cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3 + 2] = 0;
				}
			}
		}

		cleanFrameImg2.save((folder+buf2).c_str());
			
		delete[] cleanFrame;
		
	}
	delete[] _screen;
}
