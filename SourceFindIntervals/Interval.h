#ifndef __INTERVAL_H
#define __INTERVAL_H

struct Interval
{
	Interval(int b, int e)
	:begin(b), end(e)
	{
	}

	int begin, end;
};

#endif //__INTERVAL_H