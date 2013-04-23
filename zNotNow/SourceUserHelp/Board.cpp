#include <vector>
#include "Board.h"

using namespace std; 

Board::Board()
{
}

void Board::draw(int frame)
{
	glPointSize(5);

	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for(int i = 0; i < frames.size(); ++i) {
		if(frames[i] != frame) continue;
		glVertex2d(points[i].x, points[i].y);
	}
	glEnd();
	glFlush();
}

void Board::addMovement(int start)
{
	moves.push_back(Movement(start));
}

void Board::endMovement(int end)
{
	moves[moves.size()-1].end = end;
}