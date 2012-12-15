#include <vector>
#include "Board.h"

using namespace std;

Board::Board(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4)
{
	currMove = 0;

	frames.push_back(vector<int>());
	for(int i = 0; i < 4; ++i)
		frames[currMove].push_back(-1);

	points.push_back(vector<RX::vec2>());
	points[currMove].push_back(p1);
	points[currMove].push_back(p2);
	points[currMove].push_back(p3);
	points[currMove].push_back(p4);
}

Board::Board()
{
	currMove = 0;

	frames.push_back(vector<int>());
	for(int i = 0; i < 4; ++i)
		frames[currMove].push_back(-1);

	points.push_back(vector<RX::vec2>());
	points[currMove].push_back(RX::vec2(0, 0));
	points[currMove].push_back(RX::vec2(0, 0));
	points[currMove].push_back(RX::vec2(0, 0));
	points[currMove].push_back(RX::vec2(0, 0));
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
		if(frames[currMove][i] <= frame)
			glVertex2d(points[currMove][i].x, points[currMove][i].y);
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
