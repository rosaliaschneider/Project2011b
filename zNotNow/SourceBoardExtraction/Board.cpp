#include "Board.h"

Board::Board(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{
	_p1 = p1;
	_p2 = p2;
	_p3 = p3;
	_p4 = p4;

	at = p1.y-p2.y;
	bt = p2.x-p1.x;
	ct = p1.x*p2.y - p1.y*p2.x;

	ar = p2.y-p3.y;
	br = p3.x-p2.x;
	cr = p2.x*p3.y - p2.y*p3.x;

	ab = p3.y-p4.y;
	bb = p4.x-p3.x;
	cb = p3.x*p4.y - p3.y*p4.x;

	al = p4.y-p1.y;
	bl = p1.x-p4.x;
	cl = p4.x*p1.y - p4.y*p1.x;
}

Board::Board()
{
}

Board::~Board()
{
}

void Board::draw()
{
	glLineWidth(3);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2d(_p1.x, _p1.y);
	glVertex2d(_p2.x, _p2.y);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2d(_p2.x, _p2.y);
	glVertex2d(_p3.x, _p3.y);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2d(_p3.x, _p3.y);
	glVertex2d(_p4.x, _p4.y);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex2d(_p4.x, _p4.y);
	glVertex2d(_p1.x, _p1.y);
	
	glEnd();
	glFlush();

}