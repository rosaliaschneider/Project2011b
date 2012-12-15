#ifndef __BOARD_H
#define __BOARD_H

#include <RX/vec2.h>
#include <vector>

struct Movement
{
	Movement(int start)
	{
		this->start = start;
	}

	int start, end;
};

struct Board
{
	Board();

	// CHECKPOINTS
	std::vector<int> frames;
	std::vector<RX::vec2> points;
	std::vector<int> corners;

	// BOARD MOVEMENT
	std::vector<Movement> moves;
	void addMovement(int start);
	void endMovement(int end);

	void draw(int frame);
};

#endif // __BOARD_H