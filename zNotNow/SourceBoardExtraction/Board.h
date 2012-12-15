#ifndef __BOARD_H
#define __BOARD_H

#include <RX/vec2.h>

class Movement
{
public:
	Movement() {}
	Movement(int start, int end, int board, RX::vec2 sp1, RX::vec2 sp2, RX::vec2 sp3, RX::vec2 sp4, RX::vec2 fp1, RX::vec2 fp2, RX::vec2 fp3, RX::vec2 fp4)
	{
		this->start = start;
		this->end = end;
		this->board = board;
		this->fp1 = fp1;
		this->fp2 = fp2;
		this->fp3 = fp3;
		this->fp4 = fp4;
		this->sp1 = sp1;
		this->sp2 = sp2;
		this->sp3 = sp3;
		this->sp4 = sp4;

		this->t1x = (fp1.x-sp1.x)/double(end-start);
		this->t2x = (fp2.x-sp2.x)/double(end-start);
		this->t3x = (fp3.x-sp3.x)/double(end-start);
		this->t4x = (fp4.x-sp4.x)/double(end-start);

		this->t1y = (fp1.y-sp1.y)/double(end-start);
		this->t2y = (fp2.y-sp2.y)/double(end-start);
		this->t3y = (fp3.y-sp3.y)/double(end-start);
		this->t4y = (fp4.y-sp4.y)/double(end-start);

	}
	
	int start, end;
	int board;
	RX::vec2 sp1, sp2, sp3, sp4;
	RX::vec2 fp1, fp2, fp3, fp4;
	double t1x, t2x, t3x, t4x;
	double t1y, t2y, t3y, t4y;
};

class Board
{
public:
	Board(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p4);
	Board();
	~Board();

	void draw();

	// provisory
	double at, bt, ct, ab, bb, cb;
	double al, bl, cl, ar, br, cr;
	
	RX::vec2 _p1, _p2, _p3, _p4;
};

#endif // __BOARD_H