#include <vector>
#include <RX/SoftwareRenderer.h>
#include <RX/VideoDecoder.h>
#include <RX/ImageProcessor.h>
#include <RX/mat3.h>
#include <RX/mat3.h>
#include <QFileDialog>
#include <QMainWindow>
#include <QtGui/QApplication>
#include "Region.h"

using namespace std;
using namespace RX;

int cellSize = 10;

vector<Region> findImportantRegions(const QImage &image)
{
	vector<Region> regions;

	int cellsX = image.width()/cellSize;
	int cellsY = image.height()/cellSize;

	for(int i = 0; i < cellsY; ++i)
	{
		for(int j = 0; j < cellsX; ++j)
		{
			int startingX = j*cellSize;
			int startingY = i*cellSize;

			int count = 0;
			for(int k = 0; k < cellSize; ++k) {
				for(int l = 0; l < cellSize; ++l) {
					char color = image.bits()[((startingY+k)*image.width() + (startingX+l))*4];
					if(color > 50)
						++count;
				}
			}

			if(count != 0)
			{ 
				Region r;
				r.addBox(BBox(vec2(i*cellSize, j*cellSize), vec2((i+1)*cellSize, j*cellSize), vec2((i+1)*cellSize, (j+1)*cellSize), vec2(i*cellSize, (j+1)*cellSize)));
				r.setColor(RX::vec3(rand()%255, rand()%255, rand()%255));
				regions.push_back(r);
			}
		}
	}

	return regions;
}

void drawRegions(QImage *image, const vector<Region> &regions)
{
	int lastTested = -1;
	for(int i = 0; i < image->height(); ++i) 
	{
		for(int j = 0; j < image->width(); ++j) 
		{
			// explore coherence
			if(lastTested != -1)
			{
				if(regions[lastTested].isInside(RX::vec2(i, j))) {
					image->bits()[(i*image->width() + j)*4]     = 255;//regions[k].colorR();
					//image->bits()[(i*image->width() + j)*4 + 1] = regions[k].colorG();
					//image->bits()[(i*image->width() + j)*4 + 2] = regions[k].colorB();
					continue;
				}
			}

			for(int k = 0; k < regions.size(); ++k)
			{
				if(regions[k].isInside(RX::vec2(i, j))) {
					image->bits()[(i*image->width() + j)*4]     = 255;//regions[k].colorR();
					//image->bits()[(i*image->width() + j)*4 + 1] = regions[k].colorG();
					//image->bits()[(i*image->width() + j)*4 + 2] = regions[k].colorB();
					lastTested = k;
					break;
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QString folder = QFileDialog::getExistingDirectory(NULL, "Load Video");

	QImage finalFrame(folder+"/FinalFrame.png");

	// apply edge detector to frame
	Filter lap;
	lap.setLaplacian();
	ImageProcessor imgProc;
	imgProc.setImage(&finalFrame);
	imgProc.toGray();
	imgProc.applyFilter(lap);
	finalFrame.save(folder+"/FinalFrame2.png");

	// find regions that have some edge information
	vector<Region> regions = findImportantRegions(finalFrame);
	drawRegions(&finalFrame, regions);
	finalFrame.save(folder+"/FinalFrame3.png");

	// find time for each region
	RX::VideoDecoder _video;
	QImage frame;
	_video.openFile(folder+"/FinalVideo.avi");
	_video.seekNextFrame();
	_video.getFrame(frame);
	
	int h = frame.height();
	int w = frame.width();
	
	int currentFrame = 0;
	do
	{
		imgProc.setImage(&frame, 3);
		imgProc.toGray();
		imgProc.applyFilter(lap);

		for(int i = 0; i < h; ++i) 
		{
			for(int j = 0; j < w; ++j) 
			{
				char color = frame.bits()[(i*w + j)*3];
				if(color > 50) {
					for(int k = 0; k < regions.size(); ++k)
					{
						if(regions[k].startingFrame() == -1)
							continue;
						if(regions[k].isInside(RX::vec2(j, i)))
						{
							regions[k].setStartingFrame(currentFrame);
						}
					}
				}
			}
		}
		++currentFrame;
	}
	while(_video.seekNextFrame());

	// put regions together - provisory
	bool done = false;
	while(!done)
	{
		for(int i = 0; i < regions.size(); ++i)
		{
			for(int j = i+1; j < regions.size(); ++j)
			{
				if(regions[i].isNeighbor(regions[j]))
				{
					regions[i].addBoxes(regions[j].boxes());
					regions.erase(regions.begin()+j);
					--j;
				}
			}
		}
	}

}
