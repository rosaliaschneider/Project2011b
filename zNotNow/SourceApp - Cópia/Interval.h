#ifndef __INTERVAL_H
#define __INTERVAL_H

#include <RX/Definitions.h>
#include <string>
#include <QImage>

struct Interval
{
	Interval();

	int begin, end;
	GLuint id;
	QImage image;
};

#endif // __INTERVAL_H
