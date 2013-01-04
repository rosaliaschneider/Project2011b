#ifndef __INFO_H
#define __INFO_H

#include <vector>
#include <RX/mat3.h>
#include "Board.h"
#include "Region.h"

class Info
{
public:
	Info();
	~Info();

	void load(std::string folder);

	// Gets
	int nRegions() const { return _regions.size(); }
	Region region(int regionNum) { return _regions[regionNum]; }

private:
	void loadBoards(std::string filename);
	void loadRegions(std::string filename);
	void loadIntervals(std::string filename);

	std::vector<RX::mat3> homographies;
	std::vector<Board> finalBoards;
	std::vector<Region> _regions;
};

#endif // __INFO_H