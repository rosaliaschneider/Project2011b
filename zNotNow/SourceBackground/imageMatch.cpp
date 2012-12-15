//=================================================
// Name:			ImageMatch.cpp
// Author:			Rosália Galiazzi Schneider
// Last Modified:	29/09/2009
//=================================================

#include <QPainter>
#include <fstream>
#include <iostream>
#include "imageMatch.h"
using namespace std;

int *toYIQ(uchar *bits, int w, int h)
{
    int *yiq = (int *)malloc(sizeof(int)*w*h*4);

	for(int j = 0; j < w*h*4; j+=4) {

		int r = bits[j+2], g = bits[j+1], b = bits[j];
		// yiq
		int y = 0.299*r    + 0.587*g    + 0.114*b;
		int i = 0.595716*r - 0.274453*g - 0.321263*b;
		int q = 0.211456*r - 0.522591*g + 0.311135*b; 

                yiq[j]   = y;
                yiq[j+1] = i;
                yiq[j+2] = q;
	}
    return yiq;
}

uchar *toRGB(int *yiq, int w, int h)
{
	uchar *rgb = (uchar*)malloc(w*h*4);

	for(int j = 0; j < w*h*4; j+=4) {
		int y = yiq[j], i = yiq[j+1], q = yiq[j+2];
		// rgb
		int r = y   + 0.956*i   + 0.621*q;
		int g = y	- 0.272*i	- 0.647*q;
		int b = y	- 1.105*i	+ 1.702*q; 

        rgb[j+2] = min(255, max(r, 0));
        rgb[j+1] = min(255, max(g, 0));
        rgb[j]   = min(255, max(b, 0));
	}

	return rgb;
}