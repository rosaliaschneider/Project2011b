#include "Board.h"

Board::Board(int numFrames)
{
	_boxes.resize(numFrames);
}

Board::~Board()
{
}

void Board::setPos(double x, double y, int corner, int frame)
{
	_boxes[frame].points[corner] = RX::vec2(x, y);
}

void Board::setPos(BBox b, int frame)
{
	_boxes[frame] = b;
}
