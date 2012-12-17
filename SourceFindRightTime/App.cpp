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

vector<Region> findImportantRegions(const QImage &image)
{
	vector<Region> regions;

	int cellsX = image.width()/20 + 1;
	int cellsY = image.height()/20 + 1;

	regions.push_back(Region());
	regions[0].setColor(RX::vec3::zero);
	for(int i = 0; i < cellsY; ++i)
	{
		for(int j = 0; j < cellsX; ++j)
		{
			int count = 0;
			for(int k = 0; k < 20; ++k)
				for(int l = 0; l < 20; ++l)
					if(image.bits()[(i*image.width() + j)*4] > 50)
						++count;

			if(count <= 10)
			{
				regions[0].addBox(BBox(vec2(i*20, j*20), vec2((i+1)*20, j*20), vec2((i+1)*20, (j+1)*20), vec2(i*20, (j+1)*20)));
			}
			else
			{ 
				Region r;
				r.addBox(BBox(vec2(i*20, j*20), vec2((i+1)*20, j*20), vec2((i+1)*20, (j+1)*20), vec2(i*20, (j+1)*20)));
				r.setColor(RX::vec3(rand()%255, rand()%255, rand()%255));
				regions.push_back(r);
			}
		}
	}


	return regions;
}

void drawRegions(QImage *image, const vector<Region> &regions)
{
	for(int i = 0; i < image->height(); ++i) 
	{
		for(int j = 0; j < image->width(); ++j) 
		{
			for(int k = 0; k < regions.size(); ++k)
			{
				if(regions[k].isInside(RX::vec2(i, j))) {
					image->bits()[(i*image->width() + j)*4]     = regions[k].colorR();
					image->bits()[(i*image->width() + j)*4 + 1] = regions[k].colorG();
					image->bits()[(i*image->width() + j)*4 + 2] = regions[k].colorB();
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

	vector<Region> regions = findImportantRegions(finalFrame);
	drawRegions(&finalFrame, regions);
	finalFrame.save(folder+"/FinalFrame3.png");

	//RX::VideoDecoder _video;
	//QImage frame;
	//_video.openFile(folder+"/FinalVideo.avi");
	//_video.seekNextFrame();
	//_video.getFrame(frame);
	//
	//int h = frame.height();
	//int w = frame.width();
	//
	//do
	//{
	//
	//}
	//while(_video.seekNextFrame());

}
