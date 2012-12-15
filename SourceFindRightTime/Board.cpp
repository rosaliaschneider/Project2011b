#include <vector>
#include "Board.h"

using namespace std;

Board::Board(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
	points[3] = p4;
}

Board::Board()
{
	points[0] = RX::vec2(0, 0);
	points[1] = RX::vec2(0, 0);
	points[2] = RX::vec2(0, 0);
	points[3] = RX::vec2(0, 0);
}

Board::~Board()
{
}

void Board::draw(int frame)
{
	glPointSize(5);

	glBegin(GL_POINTS);
	glColor3f(0.0f, 1.0f, 0.0f);
	for(int i = 0; i < 4; ++i) {
		glVertex2d(points[i].x, points[i].y);
	}
	glEnd();
	glFlush();

	//glBegin(GL_LINE_LOOP);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//for(int i = 0; i < 4; ++i)
	//	glVertex2d(points[i].x, points[i].y);
	//glEnd();

	glFlush();
}
