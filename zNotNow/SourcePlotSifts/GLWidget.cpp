#include <RX/vec3f.h>
#include <RX/CallMatlab.h>
#include <RX/SoftwareRenderer.h>
#include <QMouseEvent>
#include "GLWidget.h"

using namespace RX;

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame1(NULL), _frame2(NULL), _selectedP(-1)
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

	glGenTextures(1, &_tex1);
	glGenTextures(1, &_tex2);
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool first = true;

	if (_frame1) 
	{
		int h = _frame1->height();
		int w = _frame1->width();

		if(*_currentFrame != lastFrameShown) 
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, _tex1);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, _frame1->bits());

			glBindTexture( GL_TEXTURE_2D, _tex2);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, _frame2->bits());
		}
		lastFrameShown = *_currentFrame;

		int left1 = -w/2;
		int right1 = w/2;
		int	bottom1 = _height/2 - h - (_height/2 - h)/2;
		int top1 = _height/2 - (_height/2 - h)/2;

		int left2 = -w/2;
		int right2 = w/2;
		int	bottom2 = -_height/2 + (_height/2 - h)/2;
		int top2 = -_height/2 + h + (_height/2 - h)/2;
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, _tex1);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(left1, top1, 1);
		glTexCoord2f(0.0, 1.0); glVertex3f(left1, bottom1, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(right1, bottom1, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(right1, top1, 1);
		glEnd();

		glBindTexture( GL_TEXTURE_2D, _tex2);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(left2, top2, 1);
		glTexCoord2f(0.0, 1.0); glVertex3f(left2, bottom2, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(right2, bottom2, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(right2, top2, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		
		glPointSize(2);
		glBegin(GL_POINTS);
		{
			for(int i = 0; i < _sifts1->size(); ++i)
			{
				//if((*_inliers)[i] == 0)
					glColor3f(1.0, 0.0, 0.0);
				//else
				//	glColor3f(0.0, 1.0, 0.0);
		
				glVertex3f((*_sifts1)[i].x + left1, top1 - (*_sifts1)[i].y, 1.0);
				glVertex3f((*_sifts2)[i].x + left2, top2 - (*_sifts2)[i].y, 1.0);
			}
		}
		glEnd();
		
		if(_selectedP != -1) {
			glPointSize(7);
			glBegin(GL_POINTS);
		
			if((*_inliers)[_selectedP] == 0)
				glColor3f(1.0, 0.0, 0.0);
			else
				glColor3f(0.0, 1.0, 0.0);
		
			glVertex3f((*_sifts1)[_selectedP].x + left1, top1 - (*_sifts1)[_selectedP].y, 1.0);
			glVertex3f((*_sifts2)[_selectedP].x + left2, top2 - (*_sifts2)[_selectedP].y, 1.0);
		
			glEnd();
		}
		
		//if(_showMatch)
		if(0)
		{
			glBegin(GL_LINES);
			{
				for(int i = 0; i < _sifts1->size(); ++i)
				{
					if((*_inliers)[i] == 0)
						glColor3f(1.0, 0.0, 0.0);
					else
						glColor3f(0.0, 1.0, 0.0);
		
					glVertex3f((*_sifts1)[i].x + left1, top1 - (*_sifts1)[i].y, 1.0);
					glVertex3f((*_sifts2)[i].x + left2, top2 - (*_sifts2)[i].y, 1.0);
				}
			}
			glEnd();
		}
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

	_left = -w/2;
	_right = w/2;
	_bottom = -h/2;
	_top = h/2;
	if(w%2 == 0) ++_left;
	if(h%2 == 0) ++_bottom;
	
	glOrtho(_left, _right, _bottom, _top, 1000, -1000);
}

void GLWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(_frame1 == NULL)
		return;

	int mouseX = (ev->pos()).x() - _width/2;
	int mouseY = _height/2 - (ev->pos()).y();

	int h = _frame1->height();
	int w = _frame1->width();
	int left1 = -w/2;
	int top1 = _height/2 - (_height/2 - h)/2;
	
	for(int i = 0; i < _sifts1->size(); ++i)
	{
		double pX = (*_sifts1)[i].x + left1;
		double pY = top1 - (*_sifts1)[i].y;

		if(abs(pX-mouseX) < 3 && abs(pY-mouseY) < 3) {
			emit nearPoint(i);
			_selectedP = i;
		}
	}
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
