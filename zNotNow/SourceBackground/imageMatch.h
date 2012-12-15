//=================================================
// Name:			ImageMatch.h
// Author:			Rosália Galiazzi Schneider
// Last Modified:	29/09/2009
//=================================================

#ifndef __IMAGEMATCH_H
#define __IMAGEMATCH_H

#include <QImage>
#include "defines.h"

int *toYIQ(uchar *rgb, int w, int h);
uchar *toRGB(int *yiq, int w, int h);

#endif // __IMAGEMATCH_H