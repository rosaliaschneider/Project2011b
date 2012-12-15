#include "Define.h"
#include "Canny.h"

unsigned char *Canny::edgeDetection(unsigned char *img, int w, int h)
{
	unsigned char *binary = new unsigned char[w*h];
	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			binary[i*w + j] = img[(i*w + j)*4]*0.3 + img[(i*w + j)*4+1]*0.59+ img[(i*w + j)*4+2]*0.11;
		}
	}

	// remove frame borders
	/*
	for(int i = 1; i < h-1; ++i) {
		for(int j = 1; j < w-1; ++j) {

			if((img[(i-1)*w + j-1] == 0)||(img[(i-1)*w + j] == 0)||(img[(i-1)*w + j+1] == 0)||
			   (img[(i)*w + j-1] == 0)||(img[(i)*w + j+1] == 0)||
			   (img[(i+1)*w + j-1] == 0)||(img[(i+1)*w + j] == 0)||(img[(i+1)*w + j+1] == 0))
				binary[i*w + j] = 0;
		}
	}
	*/

	IplImage *ipl = buffer2ipl(binary, w, h, 1);
	cvCanny(ipl, ipl, 10, 100, 3);
	//cvSaveImage("lalala.bmp", ipl);
	delete[] binary;
	binary = ipl2buffer(ipl);
	
	// paint borders to black
	for(int i = 0; i < h; ++i)
		binary[i*w] = binary[(i+1)*w - 1] = 0;
	for(int i = 0; i < w; ++i)
		binary[i] = binary[(h-2)*w + i] = 0;
	
	free(ipl->imageData);
	cvReleaseImageHeader(&ipl);

	return binary;
}
