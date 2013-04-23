#ifndef __INTERVAL_H
#define __INTERVAL_H

#include <vector>

enum Resolution
{
	r_050,
	r_100,
	r_150,
	r_200,
	r_250,

	Total
};

Resolution setResolution(float scaling)
{
	if(abs(scaling - 50) < abs(scaling - 100))
		return Resolution::r_050;
	if(abs(scaling - 100) < abs(scaling - 150))
		return Resolution::r_100;
	if(abs(scaling - 150) < abs(scaling - 200))
		return Resolution::r_150;
	if(abs(scaling - 200) < abs(scaling - 250))
		return Resolution::r_200;
	
	return Resolution::r_250;
}

struct Interval
{
	Interval(int b, int e)
	: begin(b), end(e)
	{
		buffer.resize(Resolution::Total);
		usingFrame.resize(Resolution::Total);
	}

	int bestRes(int w, int h)
	{
		int minBlacks = w*h + 1;
		int returnRes;

		for(int res = Resolution::r_250; res >= 0; --res)
		{
			int blacks = 0;
			for(int i = 0; i < h; ++i) {
				for(int j = 0; j < w; ++j) {
					if(buffer[res][(i*w +j)*3] == 0 && buffer[res][(i*w +j)*3 + 1] == 0 && buffer[res][(i*w +j)*3 + 2] == 0)
						++blacks;
				}
			}

			// if no blacks, return best resolution
			if(blacks == 0)
				return res;

			// if all resolutions have blacks, return minimum number of blacks
			if(blacks < minBlacks) {
				minBlacks = blacks;
				returnRes = res;
			}
		}
		return returnRes;
	}

	int begin, end;
	std::vector<unsigned char *> buffer;
	std::vector<int> usingFrame;
};

#endif // __INTERVAL_H