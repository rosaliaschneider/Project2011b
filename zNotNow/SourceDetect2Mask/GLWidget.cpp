#include <queue>
#include <RX/vec3f.h>
#include <RX/CallMatlab.h>
#include <RX/SoftwareRenderer.h>
#include "GLWidget.h"
#include "Define.h"
using namespace RX;

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _replace(NULL)
{
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool firstj = true;

	glClear(GL_COLOR_BUFFER_BIT);
	if (_frame) 
	{
		int h = _frame->height();
		int w = _frame->width();

		// first
		if(lastFrameShown != *_currentFrame)
		{
			// copy frame
			lastFrameShown = *_currentFrame;
		
			glEnable(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, _tex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

			// fill pink in replace to find intersection
			fill(_replace->bits(), w, h, 3);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, _frame->bits());

			// save frame
			unsigned char *cleanFrame = new unsigned char[w*h*3];
			for(int i = 0; i < h; ++i) {
				for(int j = 0; j < w; ++j) {
					if((_replace->bits()[(i*w +j)*3] != 0)||(_replace->bits()[(i*w +j)*3 + 1] != 0)||(_replace->bits()[(i*w +j)*3 + 2] != 0)) {
						cleanFrame[(i*w +j)*3] = _frame->bits()[(i*w +j)*3 + 2];
						cleanFrame[(i*w +j)*3 + 1] = _frame->bits()[(i*w +j)*3 + 1];
						cleanFrame[(i*w +j)*3 + 2] = _frame->bits()[(i*w +j)*3];
					}
					else {
						cleanFrame[(i*w +j)*3] = 0;
						cleanFrame[(i*w +j)*3 + 1] = 255;
						cleanFrame[(i*w +j)*3 + 2] = 0;
					}
				}
			}
		
			char buf[20];
			sprintf(buf, "mask_%d.png", *_currentFrame);
			QImage cleanFrameImg(cleanFrame, w, h, QImage::Format_RGB888);
			cleanFrameImg.save(buf);
			delete[] cleanFrame;
			
			glLineWidth(3.0);
			glPointSize(5.0);
		}

		glEnable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(-w/2, h/2, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(-w/2, -h/2, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(w/2, -h/2, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(w/2, h/2, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		glFlush();
	}
}   

void GLWidget::resizeGL(int w, int h)
{
	_width = w;
	_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);
}

void flipV(unsigned char *image, int w, int h, int bpp)
{
	unsigned char *flip = new unsigned char[w*h*bpp];
	for(int i = 0; i < h; ++i) {
		memcpy((void*)(flip + i*w*bpp), 
			   (void*)(image + (h - i - 1)*w*bpp), w*bpp);
	}
	memcpy((void*)image, flip, w*h*bpp);
	delete[] flip;
}

void fill(unsigned char *image, int w, int h, int bpp)
{
	queue< pair<int, int> > next;

	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			if((image[(i*w + j)*bpp] == 255)&&(image[(i*w + j)*bpp + 1] == 0)&&(image[(i*w + j)*bpp + 2] == 255))
				next.push(pair<int, int>(i, j));
		}
	}

	while(!next.empty())
	{
		pair<int, int> point = next.front();
		next.pop();

		for(int n = -1; n <= 1; ++n) 
		{
			for(int m = -1; m <= 1; ++m) 
			{
				int i = point.first;
				int j = point.second;

				if((n == 0 && m == 0)||(i+n < 0)||(i+n >= h)||(j+m < 0)||(j+m >= w))
					continue;

				if((image[((i+n)*w + (j+m))*bpp] == 0)&&(image[((i+n)*w + (j+m))*bpp + 1] == 255)&&(image[((i+n)*w + (j+m))*bpp + 2] == 0)) 
				{
					image[((i+n)*w + (j+m))*bpp] = 255;
					image[((i+n)*w + (j+m))*bpp + 1] = 0;
					image[((i+n)*w + (j+m))*bpp + 2] = 255;
				
					next.push(pair<int, int>(i+n, j+m));
				}
			}
		}
	}
}
