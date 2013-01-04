#include "Info.h"

using namespace std;

Info::Info()
{
}

Info::~Info()
{
}

void Info::load(std::string folder)
{
	loadBoards(folder+"/FinalBoards.txt");
	loadRegions(folder+"/Regions.txt");
	loadIntervals(folder+"/Intervals.txt");
}

void Info::loadBoards(std::string filename)
{
	std::ifstream input(filename);

	int nboards, nframes;
	input >> nframes >>  nboards;

	finalBoards.clear();
	finalBoards.resize(nboards);

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
		finalBoards[i] = boards[i];
}

void Info::loadRegions(std::string filename)
{
	std::ifstream input(filename);

	int nregions;
	input >> nregions;

	_regions.clear();
	_regions.resize(nregions);

	for(int i = 0; i < nregions; ++i) 
	{
		int startingFrame, nboxes;
		input >> startingFrame >> nboxes;
		_regions[i].setStartingFrame(startingFrame);
		_regions[i].setColor(RX::vec3((rand()%255)/255.0, (rand()%255)/255.0, (rand()%255)/255.0));

		for(int j = 0; j < nboxes; ++j) 
		{
			BBox b;
			for(int k = 0; k < 4; ++k)
			{
				double x, y;
				input >> x >> y;
				b.points[k] = RX::vec2(x, y);
			}
			_regions[i].addBox(b);
		}
	}	

}

void Info::loadIntervals(std::string filename)
{
	std::ifstream input(filename);

	int nboards, nintervals;
	input >>  nboards;

	for(int i = 0; i < nboards; ++i) 
	{
		input >> nintervals;
		for(int j = 0; j < nintervals; ++j) 
		{
			Interval interval;
			input >> interval.begin >> interval.end >> interval.timeMsec;
			finalBoards[i].addInterval(interval);
		}
	}
}