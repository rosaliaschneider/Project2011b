#ifndef __DEFINE_H
#define __DEFINE_H

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <gl/glew.h>
#endif

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <QImage>

#include <vector>
#include <RX/vec2.h>

using namespace std;

typedef vector< vector<double> > matd;
typedef unsigned char uchar;

void createMatrix(matd &m, int rows, int cols);

bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b);
bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p);

IplImage *buffer2ipl(unsigned char *data, int w, int h, int bpp);
unsigned char *ipl2buffer(IplImage *ipl);

#endif // __DEFINE_H