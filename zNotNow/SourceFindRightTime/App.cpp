#include <vector>
#include <queue>
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

vector< vector<BBox> > initialCells;
vector<Region> regions;

void findInitialCells(const QImage &image)
{
	int cellsX = image.width()/cellSize;
	int cellsY = image.height()/cellSize;

	for(int i = 0; i < cellsY; ++i) {
		vector<BBox> row;
		for(int j = 0; j < cellsX; ++j)
			row.push_back(BBox(vec2(j*cellSize, i*cellSize), vec2((j+1)*cellSize, i*cellSize), vec2((j+1)*cellSize, (i+1)*cellSize), vec2(j*cellSize, (i+1)*cellSize), i, j));

		initialCells.push_back(row);
	}

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
				initialCells[i][j].setUsed(true);
		}
	}
}

void putRegionTogether()
{
	// put regions together - flood fill
	for(int i = 0; i < initialCells.size(); ++i)
	{
		for(int j = 0; j < initialCells[i].size(); ++j)
		{
			if(initialCells[i][j].used() && initialCells[i][j].region() == -1)
			{
				Region r;
				r.setColor(RX::vec3(rand()%255, rand()%255, rand()%255));

				queue< pair<int, int> > q;
				q.push(pair<int, int>(i, j));
				initialCells[i][j].setRegion(regions.size());
				while(!q.empty())
				{
					pair<int, int> id = q.front();
					q.pop();

					r.addBox(initialCells[id.first][id.second]);

					for(int k = -1; k <= 1; ++k) 
					{
						for(int l = -1; l <= 1; ++l) 
						{
							// boundary check
							if(k == 0 && l == 0) continue;
							if(id.first+k < 0) continue;
							if(id.first+k >= initialCells.size()) continue;
							if(id.second+l < 0) continue;
							if(id.second+l >= initialCells[0].size()) continue;
							
							// check neighborhood
							if(!initialCells[id.first+k][id.second+l].used() || initialCells[id.first+k][id.second+l].region() != -1 || abs(initialCells[id.first][id.second].startingFrame() - initialCells[id.first+k][id.second+l].startingFrame()) > 100)
								continue;

							q.push(pair<int, int>(id.first+k, id.second+l));
							initialCells[id.first+k][id.second+l].setRegion(regions.size());
						}
					}
				}
				regions.push_back(r);
			}
		}
	}
}

void mergeSmallRegions()
{
	for(int i = 0; i < regions.size(); ++i)
	{
		if(regions[i].boxes().size() > 5) continue;

		int newRegion = -1, newRegionSize = 0;
		for(int j = 0; j < regions[i].size(); ++j)
		{
			BBox b = regions[i].boxes()[j];

			// get biggest neighbor region
			if(b.i() > 0) {
				int id = initialCells[b.i()-1][b.j()].region();
				if(id != -1 && regions[id].size() > newRegionSize) {
					newRegion = id;
					newRegionSize = regions[id].size();
				}
			}
			if(b.j() > 0) {
				int id = initialCells[b.i()][b.j()-1].region();
				if(id != -1 && regions[id].size() > newRegionSize) {
					newRegion = id;
					newRegionSize = regions[id].size();
				}
			}
			if(b.i() < initialCells.size()-1) {
				int id = initialCells[b.i()+1][b.j()].region();
				if(id != -1 && regions[id].size() > newRegionSize) {
					newRegion = id;
					newRegionSize = regions[id].size();
				}
			}
			if(b.j() < initialCells[0].size()-1) {
				int id = initialCells[b.i()][b.j()+1].region();
				if(id != -1 && regions[id].size() > newRegionSize) {
					newRegion = id;
					newRegionSize = regions[id].size();
				}
			}
		}
		// merge
		if(newRegion != -1)
			regions[newRegion].addBoxes(regions[i].boxes());
	}
	for(int i = 0; i < regions.size(); ++i)
	{
		if(regions[i].boxes().size() > 5) continue;
		regions.erase(regions.begin()+i);
		--i;
	}
}

void defineRegionStartingFrame()
{
	for(int i = 0; i < regions.size(); ++i)
	{
		regions[i].setStartingFrame(regions[i].boxes()[0].startingFrame());
		for(int j = 1; j < regions[i].boxes().size(); ++j)
		{
			if(regions[i].boxes()[j].startingFrame() < regions[i].startingFrame())
				regions[i].setStartingFrame(regions[i].boxes()[j].startingFrame());
		}
	}
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
				if(regions[lastTested].isInside(RX::vec2(j, i))) {
					image->bits()[(i*image->width() + j)*4]     = regions[lastTested].colorR();
					image->bits()[(i*image->width() + j)*4 + 1] = regions[lastTested].colorG();
					image->bits()[(i*image->width() + j)*4 + 2] = regions[lastTested].colorB();
					continue;
				}
			}

			for(int k = 0; k < regions.size(); ++k)
			{
				if(regions[k].isInside(RX::vec2(j, i))) {
					image->bits()[(i*image->width() + j)*4]     = regions[k].colorR();
					image->bits()[(i*image->width() + j)*4 + 1] = regions[k].colorG();
					image->bits()[(i*image->width() + j)*4 + 2] = regions[k].colorB();
					lastTested = k;
					break;
				}
			}
		}
	}
}

void drawBoxes(QImage *image, const vector< vector<BBox> > &boxes)
{
	int lastTested = -1;
	for(int i = 0; i < image->height(); ++i) 
	{
		for(int j = 0; j < image->width(); ++j) 
		{
			for(int k = 0; k < boxes.size(); ++k)
			{
				for(int l = 0; l < boxes[k].size(); ++l)
				{
					if(!(boxes[k][l].used())) continue;
					if(boxes[k][l].isInside(RX::vec2(j, i))) 
					{
						image->bits()[(i*image->width() + j)*4]     = 255;
						image->bits()[(i*image->width() + j)*4 + 1] = 255;
						image->bits()[(i*image->width() + j)*4 + 2] = 255;
						lastTested = k;
						break;
					}
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

	// find cells that have some edge information
	findInitialCells(finalFrame);

	// find time for each cell
	RX::VideoDecoder _video;
	QImage frame;
	_video.openFile(folder+"/FinalVideo.avi");
	
	int currentFrame = 0;
	while(_video.seekNextFrame())
	{
		_video.getFrame(frame);

		imgProc.setImage(&frame, 3);
		imgProc.toGray();
		imgProc.applyFilter(lap);

		for(int i = 0; i < initialCells.size(); ++i) 
		{
			for(int j = 0; j < initialCells[i].size(); ++j) 
			{
				if(!(initialCells[i][j].used()))
					continue;

				int startingX = j*cellSize;
				int startingY = i*cellSize;

				bool blank = true;
				for(int k = 0; k < cellSize && blank; ++k) {
					for(int l = 0; l < cellSize && blank; ++l) {
						char color = frame.bits()[((startingY+k)*frame.width() + (startingX+l))*3];
						if(color > 50) 
						{
							blank = false;
							break;
						}
					}
				}

				if(blank)
					initialCells[i][j].setStartingFrame(currentFrame+1);
			}
		}
		++currentFrame;
		cout << currentFrame << endl;
	}

	// define regions from cells
	cout << "Put together" << endl; putRegionTogether();
	cout << "Merge" << endl; mergeSmallRegions();
	cout << "Starting frame" << endl; defineRegionStartingFrame();

	//drawBoxes(&finalFrame, initialCells);
	drawRegions(&finalFrame, regions);

	ofstream regionsFile((folder+"/Regions.txt").toAscii());
	regionsFile << regions.size() << endl;
	for(int i = 0; i < regions.size(); ++i) {
		regionsFile << regions[i].startingFrame() << std::endl;
		regionsFile << regions[i].boxes().size() << std::endl;
		for(int j = 0; j < regions[i].boxes().size(); ++j) {
			for(int k = 0; k < 4; ++k)
				regionsFile << regions[i].boxes()[j].getX(k)-finalFrame.width()/2 << " " << regions[i].boxes()[j].getY(k)-finalFrame.height()/2 << " ";
			regionsFile << std::endl;
		}
	}

	ofstream log((folder+"/log.txt").toAscii());
	log << "Boxes" << endl;
	for(int i = 0; i < initialCells.size(); ++i) {
		for(int j = 0; j < initialCells[i].size(); ++j) {
			log << initialCells[i][j].startingFrame() << "  ";
		}
		log << std::endl;
	}

	finalFrame.save(folder+"/FinalFrame4.png");

}
