#ifndef __CANNY_H
#define __CANNY_H

#include <QImage>

class Canny
{
public:
	static unsigned char *edgeDetection(unsigned char *img, int w, int h);
};

#endif // __CANNY_H