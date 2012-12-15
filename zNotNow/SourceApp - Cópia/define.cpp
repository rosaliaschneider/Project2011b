#include <RX/vec3.h>
#include "Define.h"

void createMatrix(matd &m, int rows, int cols)
{
	m.clear();
	for(int i = 0; i < rows; ++i) {
		vector<double> v;
		for(int j = 0; j < cols; ++j) {
			v.push_back(0.0);
		}
		m.push_back(v);
	}
}

bool sameSide(RX::vec2 p1, RX::vec2 p2, RX::vec2 a, RX::vec2 b)
{
	RX::vec3 cross1 = RX::vec3((a-b),1) ^ RX::vec3((p1-b),1);
	RX::vec3 cross2 = RX::vec3((a-b),1) ^ RX::vec3((p2-b),1);
	if(cross1 * cross2 >= 0) 
		return true;
	return false;
}

bool pointInsideTriangle(RX::vec2 p1, RX::vec2 p2, RX::vec2 p3, RX::vec2 p)
{
	if((sameSide(p, p1, p2, p3))&&(sameSide(p, p2, p1, p3))&&(sameSide(p, p3, p1, p2)))
		return true;
	return false;
}

//IplImage *buffer2ipl(unsigned char *data, int w, int h, int bpp)
//{
//	IplImage *imgHeader = cvCreateImageHeader( cvSize(w, h), IPL_DEPTH_8U, bpp);
//	uchar* newdata = (uchar*) malloc(sizeof(uchar) * w*h*bpp);
//	memcpy(newdata, data, w*h*bpp);
//	imgHeader->imageData = (char*) newdata;
//	imgHeader->align = 1;
//	imgHeader->widthStep = w*bpp;
//	imgHeader->imageDataOrigin = (char*) newdata;
//
//	return imgHeader;
//}
//
//unsigned char *ipl2buffer(IplImage *iplImg)
//{
//	int h = iplImg->height;
//	int w = iplImg->width;
//	int bpp = iplImg->nChannels;
//	unsigned char *buffer = new unsigned char[w*h*bpp];
//	char *data = iplImg->imageData;
//
//	for (int i = 0; i < w*h*bpp; ++i) {
//		buffer[i] = data[i];
//	}
//	return buffer;
//}
