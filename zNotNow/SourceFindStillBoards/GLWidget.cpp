#include <RX/vec3f.h>
#include <RX/SoftwareRenderer.h>
#include "GLWidget.h"

using namespace RX;

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _screen(NULL)
{
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();
}

GLWidget::~GLWidget()
{
	if(_screen) delete[] _screen;
}

void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	hom.setIdentity();
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool firstj = true;

	glClear(GL_COLOR_BUFFER_BIT);
	if (_frame) 
	{
		// first
		if(lastFrameShown != *_currentFrame)
		{
			memset(_screen, 0, _width*_height*3);

			lastFrameShown = *_currentFrame;
		
			int h = _frame->height();
			int w = _frame->width();

			_framePos1 = RX::vec3(-w/2, h/2, 1);
			_framePos2 = RX::vec3(-w/2, -h/2, 1);
			_framePos3 = RX::vec3(w/2, -h/2, 1);
			_framePos4 = RX::vec3(w/2, h/2, 1);

			// render frame
			RX::SoftwareRenderer::render(*_frame, _framePos1, _framePos2, _framePos3, _framePos4, _screen, _width, _height, 3, 3);
									
			// draw boards
			hom = (*_homographies)[lastFrameShown].inverse() * hom;
			for(int i = 0; i < (*_boards)[lastFrameShown].size(); ++i)
			{
				(*_boards)[lastFrameShown][i].transform(hom);

				RX::vec3 pos[4];
				for(int j = 0; j < 4; ++j) {
					pos[j].x = (*_boards)[lastFrameShown][i].getX(j);
					pos[j].y = (*_boards)[lastFrameShown][i].getY(j);
					pos[j].z = 1;
				}
				RX::SoftwareRenderer::render(RX::vec3(0, 255, 0), pos[0], pos[1], pos[2], pos[3], _screen, _width, _height, 3);
			}

			for(int i = 0; i < _alpha->height(); ++i) {
				for(int j = 0; j < _alpha->width(); ++j) {
					if(_alpha->bits()[(i*_alpha->width() + j)*3] != 0 || _alpha->bits()[(i*_alpha->width() + j)*3 + 1] != 0 || _alpha->bits()[(i*_alpha->width() + j)*3 + 2] != 0) {
						_alpha->bits()[(i*_alpha->width() + j)*3]     = 255;
						_alpha->bits()[(i*_alpha->width() + j)*3 + 1] = 0;
						_alpha->bits()[(i*_alpha->width() + j)*3 + 2] = 255;
					}
				}
			}

			RX::SoftwareRenderer::renderNoColorA(*_alpha, RX::vec3(0, 0, 0), _framePos1, _framePos2, _framePos3, _framePos4, _screen, _width, _height, 3, 3);
			
			flipV(_screen, _width, _height, 3);
			glRasterPos2i(-_width/2, _height/2);
			glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, _screen);
			glFlush();

			// save frame
			unsigned char *cleanFrame = new unsigned char[w*h*3];
			glReadPixels(_width/2-w/2, _height/2-h/2, w, h, GL_RGB, GL_UNSIGNED_BYTE, cleanFrame);
			flipV(cleanFrame, w, h, 3);
		
			char buf[100];
			sprintf(buf, "D:/Research/_ Project2011b/Data/Recitation13/Replace/frame_%d.png", *_currentFrame);
			QImage cleanFrameImg(cleanFrame,  w, h, QImage::Format_RGB888);
			cleanFrameImg.save(buf);
			
			char buf2[100];
			sprintf(buf2, "D:/Research/_ Project2011b/Data/Recitation13/Mask/frame_%d.png", *_currentFrame);
			QImage cleanFrameImg2(_frame->bits(),  w, h, QImage::Format_RGB888);

			for(int i = 0; i < cleanFrameImg2.height(); ++i) {
				for(int j = 0; j < cleanFrameImg2.width(); ++j) {
					int r = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3]; 
					int g = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3 + 1];
					int b = cleanFrameImg.bits()[(i*cleanFrameImg.width() + j)*3 + 2];
					if((r == 255 && g == 0 && b == 255)||(r == 0 && g == 255 && b == 0))
					{
					}
					else {
						cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3] = 0;
						cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3 + 1] = 255;
						cleanFrameImg2.bits()[(i*cleanFrameImg.width() + j)*3 + 2] = 0;
					}
				}
			}

			cleanFrameImg2.save(buf2);
			
			delete[] cleanFrame;
		}
		glRasterPos2i(-_width/2, _height/2);
		glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, _screen);
		glFlush();

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

	if(_screen) delete[] _screen;
	_screen = new unsigned char[_width*_height*3];
	memset(_screen, 0, _width*_height*3);
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
