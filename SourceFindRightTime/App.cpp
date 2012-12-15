#include <vector>
#include <RX/SoftwareRenderer.h>
#include <RX/VideoDecoder.h>
#include <RX/mat3.h>
#include <QFileDialog>
#include <QMainWindow>
#include <QtGui/QApplication>
#include "Board.h"
#include "Interval.h"

using namespace std;

vector< vector<Board> > _boards;
vector< RX::mat3 > _homographies;
vector< vector<Interval> > _intervals;

void saveIntervals(string folder)
{
	std::ofstream output((folder+"/Intervals.txt").c_str());

	output << _intervals.size() << endl;
	
	for(int i = 0; i < _intervals.size(); ++i) 
	{
		output << _intervals[i].size() << endl;
		for(int j = 0; j < _intervals[i].size(); ++j) 
		{
			output << _intervals[i][j].begin << " " << _intervals[i][j].end << endl;
		}
	}
}

void loadHomographies(string folder)
{
	ifstream input((folder+"/GlobalHoms.txt").c_str());

	_homographies.clear();
	int n;
	input >> n;
	_homographies.resize(n);
	for(int i = 0; i < n; ++i)
	{
		RX::mat3 m;
		double dummy;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				input >> dummy;
				m.set(j, k, dummy);
			}
		}
		_homographies[i] = m;
	}
}

void loadBoards(string folder)
{
	std::ifstream input((folder+"/Boards.txt").c_str());

	int numFrames, numBoards;
	input >> numFrames >> numBoards;

	_boards.resize(numFrames);
	for(int i = 0; i < numFrames; ++i) 
	{
		_boards[i].resize(numBoards);
		for(int j = 0; j < numBoards; ++j) 
		{
			for(int k = 0; k < 4; ++k) {
				double x, y;
				input >> x >> y;
				_boards[i][j].points[k] = RX::vec2(x, y);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QString folder = QFileDialog::getExistingDirectory(NULL, "Load Video");

	loadHomographies(folder.toStdString());
	loadBoards(folder.toStdString());

	RX::VideoDecoder _video;
	QImage frame;
	_video.openFile(folder+"/Video.avi");
	_video.seekNextFrame();
	_video.getFrame(frame);

	int h = frame.height();
	int w = frame.width();

	unsigned char *screen = new unsigned char[w*h*4];

	RX::mat3 _hom;
	_hom.setIdentity();
	int f = 0;

	// get intersections
	vector< vector<bool> > intersections;
	do
	{
		_otherVideo.getFrame(frame2);
		_profVideo.getFrame(frame);

		for(int i = 0; i < h; ++i) {
			for(int j = 0; j < w; ++j) {
				if(frame.bits()[(i*w + j)*3] != 0 || frame.bits()[(i*w + j)*3 + 1] != 0 || frame.bits()[(i*w + j)*3 + 2] != 0) 
				{
					frame.bits()[(i*w + j)*3] = 255;
					frame.bits()[(i*w + j)*3 + 1] = 0;
					frame.bits()[(i*w + j)*3 + 2] = 255;
				}
			}
		}
		_hom = _homographies[f].inverse();

		int totalPinks = RX::SoftwareRenderer::countColor(RX::vec3(255, 0, 255), RX::vec3(0, 0, 1), RX::vec3(w, 0, 1), RX::vec3(w, h, 1), RX::vec3(0, h, 1), frame.bits(), w, h, 3, 3);

		vector<bool> intersecting;
		for(int i = 0; i < _boards[f].size(); ++i) 
		{	
			intersecting.push_back(false);

			RX::vec3 p[4];
			for(int j = 0; j < 4; ++j) {
				p[j] = _hom * RX::vec3(_boards[f][i].points[j].x, _boards[f][i].points[j].y, 1);
			}

			memcpy(screen, frame.bits(), w*h*3);
			RX::SoftwareRenderer::render(RX::vec3(0, 255, 0), p[0], p[1], p[2], p[3], screen, frame.width(), frame.height(), 3);

			if(i == 0)
			{
				char buf[40];
				sprintf(buf, "image_%d.png", f);
				QImage image(screen, w, h, QImage::Format_RGB888);
				image.save(buf);

				sprintf(buf, "image_%d_b.png", f);
				frame2.save(buf);
			}

			int newPinks = RX::SoftwareRenderer::countColor(RX::vec3(255, 0, 255), RX::vec3(0, 0, 1), RX::vec3(w, 0, 1), RX::vec3(w, h, 1), RX::vec3(0, h, 1), screen, w, h, 3, 3);
			if(newPinks < totalPinks)
				intersecting[i] = true;
		}
		intersections.push_back(intersecting);
		++f;
		cout << f << endl;
		// provisory
		if(f == 10000)
			break;

		_otherVideo.seekNextFrame();
	}
	while(_profVideo.seekNextFrame());
	
	delete[] screen;

	// change intersections into intervals
	std::ofstream intersectFile("intersectFile.txt");
	intersectFile << intersections.size() << std::endl;
	for(int i = 0; i < intersections.size(); ++i)
	{
		for(int j = 0; j < intersections[i].size(); ++j)
		{
			intersectFile << j << " " << intersections[i][j] << " " << std::endl;
		}
		intersectFile << "#####################################################################" << std::endl;
	}

	vector<bool> state;
	for(int i = 0; i < _boards[0].size(); ++i) {
		state.push_back(true);
		_intervals.push_back(vector<Interval>());
	}

	for(int i = 0; i < intersections.size(); ++i)
	{
		for(int j = 0; j < state.size(); ++j)
		{
			if(state[j] != intersections[i][j])
			{
				if(intersections[i][j])
					_intervals[j][_intervals[j].size()-1].end = i-1;
				else
					_intervals[j].push_back(Interval(i, -1));

				state[j] = intersections[i][j];
			}
		}
	}
	for(int i = 0; i < _intervals.size(); ++i)
		for(int j = 0; j < _intervals[i].size(); ++j)
			if(_intervals[i][j].end == -1)
				_intervals[i][j].end = f;

	saveIntervals(folder.toStdString());

}
