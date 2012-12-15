#ifndef __BOARD_H
#define __BOARD_H

#include <RX/vec2.h>

struct Movement
{
	int start, end;
	int board;

	Movement()
	: board(-1)
	{
	}
};

class Board
{
public:
	Board(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);
	Board();
	~Board();

	void draw(int frame);

	int currMove;
	std::vector< std::vector<RX::vec2> > points;
	std::vector< std::vector<int> > frames;
	bool isOpen;	
};


#endif // __BOARD_H