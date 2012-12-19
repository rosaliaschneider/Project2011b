#include <RX/vec3f.h>
#include <RX/ImageProcessor.h>
#include <RX/SoftwareRenderer.h>
#include <QMouseEvent>
#include "GLWidget.h"
#include "Homography.h"

using namespace RX;

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _scale(1.0), _selected(false), buffer(NULL), buffer2(NULL)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();

	for(int i = 0; i < 4; ++i)
		_newPoints[i] = RX::vec2(-1, -1);

	_translateToFit = RX::vec3(0, 0, 0);
}

GLWidget::~GLWidget()
{
	if(buffer) delete[] buffer;
	if(buffer2) delete[] buffer2;
}

void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glShadeModel(GL_FLAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_tex);

	_persp.initialize("perspV.cg", "", "perspF.cg");
	_persp.setFParameterTex(_tex, "decal");

	buffer = new unsigned char[1000*1000*4];
	buffer2 = new unsigned char[1000*1000*4];
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;

	if(_frame) 
	{
		int h = _frame->height();
		int w = _frame->width();

		if(*_currentFrame != lastFrameShown)
		{
			_framePos[0] = (*_globalHomographies)[*_currentFrame] * RX::vec3(-w/2, h/2, 1);
			_framePos[1] = (*_globalHomographies)[*_currentFrame] * RX::vec3(-w/2, -h/2, 1);
			_framePos[2] = (*_globalHomographies)[*_currentFrame] * RX::vec3(w/2, -h/2, 1);
			_framePos[3] = (*_globalHomographies)[*_currentFrame] * RX::vec3(w/2, h/2, 1);

			for(int j = 0; j < _boards->size(); ++j)
				for(int k = 0; k < 4; ++k)	
					_boardPos[j][k] = RX::vec3((*_boards)[j][*_currentFrame].getX(k), (*_boards)[j][*_currentFrame].getY(k), 1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, _tex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, _frame->bits());
		}

		glBindTexture(GL_TEXTURE_2D, _tex);

		_persp.enable();

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(_framePos[0].x * _scale + _translateToFit.x, _framePos[0].y * _scale + _translateToFit.y, _framePos[0].z);
		glTexCoord2f(0.0, 0.0); glVertex3f(_framePos[1].x * _scale + _translateToFit.x, _framePos[1].y * _scale + _translateToFit.y, _framePos[1].z);
		glTexCoord2f(1.0, 0.0); glVertex3f(_framePos[2].x * _scale + _translateToFit.x, _framePos[2].y * _scale + _translateToFit.y, _framePos[2].z);
		glTexCoord2f(1.0, 1.0); glVertex3f(_framePos[3].x * _scale + _translateToFit.x, _framePos[3].y * _scale + _translateToFit.y, _framePos[3].z); 
		glEnd();

		_persp.disable();

		glDisable(GL_TEXTURE_2D);

		//for(int i = 0; i < _boards->size(); ++i)
		//{
		//	glColor3f(0.0, 1.0, 0.0);
		//	glBegin(GL_LINE_LOOP);
		//	for(int j = 0; j < 4; ++j)
		//		glVertex3f(_boardPos[i][j].x * _scale, _boardPos[i][j].y * _scale - 200, 1);
		//	glEnd();
		//}

		glColor3f(1.0, 0.0, 0.0);
		glPointSize(3);
		glBegin(GL_POINTS);
		for(int j = 0; j < 4; ++j) {
			if(_newPoints[j].x != -1)
				glVertex3f(_newPoints[j].x, _newPoints[j].y, 1);
		}
		glEnd();

		if(*_currentFrame != lastFrameShown)
		{
			// keep old
			memcpy(buffer2, buffer, 1000*1000*4);

			// apply edge detector to frame
			Filter lap;
			lap.setLaplacian();
			ImageProcessor imgProc;
			imgProc.setImage(_frame, 3);
			imgProc.toGray();
			imgProc.applyFilter(lap);
			// render frame to buffer
			RX::SoftwareRenderer::render(*_frame, _framePos[0]+_translateToFit, _framePos[1]+_translateToFit, _framePos[2]+_translateToFit, _framePos[3]+_translateToFit, buffer, 1000, 1000, 3, 4);

			// count edge pixels that were in buffer, but now disappeared
			int count = 0;
			int totalCount = 0;
			for(int i = 0; i < 1000; ++i) {
				for(int j = 0; j < 1000; ++j) {
					if(buffer2[(i*1000 + j)*4] > 50) {
						++totalCount;
						if(buffer[(i*1000 + j)*4] <= 50)
							++count;
					}
				}
			}

			if(count >= 100 && count >= totalCount * 0.7) 
			{
				char filename[100];
				sprintf(filename, "test_%d.png", *_currentFrame);
				QImage edges(buffer, 1000, 1000, QImage::Format_RGB32);
				edges.save(filename);

				startMovingFrame();
				moveRight(w);
				correctHomographies();
				_translateToFit -= RX::vec3(w, 0, 0);
			}

			lastFrameShown = *_currentFrame;
		}
	} 
}

void GLWidget::setHomographies(vector<RX::mat3> *homographies)
{
	_homographies = homographies;
}

void GLWidget::setGlobalHomographies(vector<RX::mat3> *globalHomographies)
{
	_globalHomographies = globalHomographies;
}

void GLWidget::setBoards(vector< vector<BBox> > *boards)
{
	_boards = boards;
}

void GLWidget::resizeGL(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLWidget::keyPressEvent(QKeyEvent  *ev)
{
	int b = -1, corner = -1;

	switch(ev->key())
	{
	case Qt::Key_1:
		b = 0;
		break;
	case Qt::Key_2:
		b = 1;
		break;
	case Qt::Key_3:
		b = 2;
		break;
	case Qt::Key_4:
		b = 3;
		break;
	case Qt::Key_5:
		b = 4;
		break;
	case Qt::Key_6:
		b = 5;
		break;
	case Qt::Key_7:
		b = 6;
		break;
	case Qt::Key_8:
		b = 7;
		break;
	case Qt::Key_9:
		b = 8;
		break;

	case Qt::Key_Q:
		corner = 0;
		break;
	case Qt::Key_W:
		corner = 1;
		break;
	case Qt::Key_S:
		corner = 2;
		break;
	case Qt::Key_A:
		corner = 3;
		break;

	case Qt::Key_Space:
		correctHomographies();
		break;
	};

	if(b != -1) {
		emit selectedBoard(b+1);
		_board = b;
		_selected = false;
	}
	if(corner != -1) {
		emit selectedPoint(corner+1);
		_point = corner;
		_selected = true;
	}
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
	if(_selected) {
		RX::vec2 mousePos = RX::vec2(((ev->pos()).x()-width()/2), ((ev->pos()).y()-height()/2));
		addCorner(_board, _point, *_currentFrame, mousePos);
		_selected = false;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(_frame == NULL)
		return;
}

void GLWidget::addCorner(int board, int corner, int currentFrame, RX::vec2 pos)
{
	_newPoints[corner] = pos;
}

void GLWidget::startMovingFrame()
{
	_newPoints[0] = _oldPoints[0] = _framePos[0].xy();
	_newPoints[1] = _oldPoints[1] = _framePos[1].xy();
	_newPoints[2] = _oldPoints[2] = _framePos[2].xy();
	_newPoints[3] = _oldPoints[3] = _framePos[3].xy();
}

void GLWidget::moveLeft(int pixels, int point)
{
	if(point == -1)
	{
		for(int i = 0; i < 4; ++i)
			_newPoints[i].x -= pixels;
	}
	else
		_newPoints[point].x -= pixels;
}

void GLWidget::moveRight(int pixels, int point)
{
	if(point == -1)
	{
		for(int i = 0; i < 4; ++i)
			_newPoints[i].x += pixels;
	}
	else
		_newPoints[point].x += pixels;
}

void GLWidget::moveUp(int pixels, int point)
{
	if(point == -1)
	{
		for(int i = 0; i < 4; ++i)
			_newPoints[i].y -= pixels;
	}
	else
		_newPoints[point].y -= pixels;
} 

void GLWidget::moveDown(int pixels, int point)
{
	if(point == -1)
	{
		for(int i = 0; i < 4; ++i)
			_newPoints[i].y += pixels;
	}
	else
		_newPoints[point].y += pixels;
}

void GLWidget::correctHomographies()
{
	if(*_currentFrame == 0)
		return;

	Homography correction;
	correction.setPoints1(_oldPoints[0], _oldPoints[1], _oldPoints[2], _oldPoints[3]);
	correction.setPoints2(_newPoints[0], _newPoints[1], _newPoints[2], _newPoints[3]);
	correction.estimate();

	(*_globalHomographies)[*_currentFrame] = correction.hom() * (*_globalHomographies)[*_currentFrame-1] * (*_homographies)[*_currentFrame];
	for(int i = *_currentFrame+1; i < _globalHomographies->size(); ++i)
		(*_globalHomographies)[i] = (*_globalHomographies)[i-1] * (*_homographies)[i];
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
