#ifndef __COVERAGE_H
#define __COVERAGE_H

#include <RX/vec2.h>

using namespace RX;

struct Coverage
{
	int id;
	int whites;

	Coverage() {
		this->id = 0;
		this->whites = 0;
	}

	Coverage(int id, int whites) {
		this->id = id;
		this->whites = whites;
	}
};

struct BestFrame
{
	int id;
	vec2 p1, p2, p3, p4;

	BestFrame(vec2 p1, vec2 p2, vec2 p3, vec2 p4, int id)
	{
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
		this->p4 = p4;
		this->id = id;
	}
};

#endif // __COVERAGE_H