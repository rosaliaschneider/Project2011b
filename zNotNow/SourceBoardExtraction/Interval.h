#ifndef __INTERVAL_H
#define __INTERVAL_H

#include <string>

struct Interval
{
	Interval();

	int begin, end, id;
	std::string image;
	double quality;
};

#endif // __INTERVAL_H
