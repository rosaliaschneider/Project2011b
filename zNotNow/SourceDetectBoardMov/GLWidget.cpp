#include <RX/SoftwareRenderer.h>
#include <QMouseEvent>
#include "GLWidget.h"
#include "kht.h"
#include "Canny.h"
#include "Movement.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _currentFrame(NULL), screen(NULL), _selected(0), _point(-1)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
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
	glDisable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
  
void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	if(_frame) 
	{
		bool first = lastFrameShown != *_currentFrame;
		lastFrameShown = *_currentFrame;
		
		int h = _frame->height();
		int w = _frame->width();

		RX::vec3 framePos1 = RX::vec3(-w/2, h/2, 1);
		RX::vec3 framePos2 = RX::vec3(-w/2, -h/2, 1);
		RX::vec3 framePos3 = RX::vec3(w/2, -h/2, 1);
		RX::vec3 framePos4 = RX::vec3(w/2, h/2, 1);

		if(first)
		{
			flipV(screen, _width, _height, 4);
			RX::SoftwareRenderer::render(*_frame, framePos1, framePos2, framePos3, framePos4, screen, _width, _height, 3, 4);
			flipV(screen, _width, _height, 4);

			bool movementStart = false;
			for(int i = 0; i < _movements->size(); ++i)
			{
				if(((*_movements)[i].start() < *_currentFrame)&&((*_movements)[i].end() >= *_currentFrame)) {
					movementStart = true;
				}
			}
			if(movementStart)
			{
				unsigned char *binary = Canny::edgeDetection(screen, _width, _height);
				kht(lines, binary, _width, _height);
				delete[] binary;
			}
		}

		glDrawPixels(_width, _height, GL_BGRA, GL_UNSIGNED_BYTE, screen);

		for(int i = 0; i < _movements->size(); ++i) {
			if(!((*_movements)[i].active(*_currentFrame)))
				continue;
	
			glColor3f(1.0, 1.0, 1.0); 
			glBegin(GL_LINE_LOOP);
			for(int j = 0; j < 4; ++j) {
 				glVertex3f((*_movements)[i].getX(*_currentFrame, j), (*_movements)[i].getY(*_currentFrame, j), 1);
			}
			glEnd();

			glBegin(GL_LINES);
			glVertex3f((*_movements)[i].getCX(*_currentFrame)-5, (*_movements)[i].getCY(*_currentFrame), 1);
			glVertex3f((*_movements)[i].getCX(*_currentFrame)+5, (*_movements)[i].getCY(*_currentFrame), 1);
			glVertex3f((*_movements)[i].getCX(*_currentFrame), (*_movements)[i].getCY(*_currentFrame)-5, 1);
			glVertex3f((*_movements)[i].getCX(*_currentFrame), (*_movements)[i].getCY(*_currentFrame)+5, 1);
			glEnd();

			glPointSize(3);
			glColor3f(1.0, 0.0, 0.0); 
			glBegin(GL_POINTS);
			for(int j = 0; j < 4; ++j) {
 				glVertex3f((*_movements)[i].getX(*_currentFrame, j), (*_movements)[i].getY(*_currentFrame, j), 1);
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
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_DEPTH_BUFFER_BIT);

	if(screen != NULL)
		delete[] screen;
	screen = new unsigned char[_width*_height*4];
	memset(screen, 0, _width*_height*4);
}

void GLWidget::saveBoards(std::string filename)
{
	if(_currentFrame == NULL) return;
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
	if(_selected == 0)
	{
		for(int i = 0; i < _movements->size(); ++i) {
			if(!((*_movements)[i].active(*_currentFrame)))
				continue;
	
			for(int j = 0; j < 4; ++j) {
 				float distx = (*_movements)[i].getX(*_currentFrame, j) - ((ev->pos()).x()-_width/2);
				float disty = (*_movements)[i].getY(*_currentFrame, j) - ((ev->pos()).y()-_height/2);
				float dist = sqrt(distx*distx + disty*disty);
				if(dist <= 3) {
					_movement = i;
					_point = j;
					_selected = _point+1;
					return;
				}
			}

			// select centroid
			float distx = (*_movements)[i].getCX(*_currentFrame) - ((ev->pos()).x()-_width/2);
			float disty = (*_movements)[i].getCY(*_currentFrame) - ((ev->pos()).y()-_height/2);;
			float dist = sqrt(distx*distx + disty*disty);
			if(dist <= 3) {
				_movement = i;
				_selected = 5;
				return;
			}
		}
	}
	else if(_selected == 5) {
		RX::vec2 mousePos = RX::vec2(((ev->pos()).x()-_width/2), ((ev->pos()).y()-_height/2));
		(*_movements)[_movement].setCentroid(*_currentFrame, mousePos.x, mousePos.y);
		_selected = 0;
	}
	else {
		RX::vec2 mousePos = RX::vec2(((ev->pos()).x()-_width/2), ((ev->pos()).y()-_height/2));
		(*_movements)[_movement].setX(*_currentFrame, _point, mousePos.x);
		(*_movements)[_movement].setY(*_currentFrame, _point, mousePos.y);
		_selected = 0;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *ev)
{
	int mouseX = (ev->pos()).x();
	int mouseY = (ev->pos()).y();

	if(_selected) {
		if(_selected == 5)
		{
			(*_movements)[_movement].setCentroid(*_currentFrame, mouseX-_width/2, mouseY-_height/2);
		}
		else
		{
			(*_movements)[_movement].setX(*_currentFrame, _point, mouseX-_width/2);
			(*_movements)[_movement].setY(*_currentFrame, _point, mouseY-_height/2);
		}
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *ev)
{
}

void flipV(unsigned char *image, int w, int h, int bpp)
{
	unsigned char *flip = new unsigned char[w*h*bpp];
	for(int i = 0; i < h; ++i) {
		memcpy((void*)(flip + i*w*bpp), 
			   (void*)(image + (h - i - 1)*w*4), w*bpp);
	}
	memcpy((void*)image, flip, w*h*bpp);
	delete[] flip;
}

