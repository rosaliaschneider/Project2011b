#include <RX/vec3f.h>
#include <RX/CallMatlab.h>
#include <RX/SoftwareRenderer.h>
#include <QMouseEvent>
#include "BoardWidget.h"

using namespace RX;

BoardWidget::BoardWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _scale(1.0)
{
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();
}

BoardWidget::~BoardWidget()
{
}

void BoardWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glShadeModel(GL_FLAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_tex);

	_persp.initialize("perspV.cg", "", "perspF.cg");
	_persp.setFParameterTex(_tex, "decal");

	_hom.setIdentity();
}

void BoardWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool first = true;

	if(_frame) 
	{
		int h = _frame->height();
		int w = _frame->width();

		if(*_currentFrame != lastFrameShown)
		{
			//_hom = _hom * (*_homographies)[*_currentFrame];
			_hom = (*_homographies)[*_currentFrame];
			_framePos1 = _hom * RX::vec3(-w/2, h/2, 1);
			_framePos2 = _hom * RX::vec3(-w/2, -h/2, 1);
			_framePos3 = _hom * RX::vec3(w/2, -h/2, 1);
			_framePos4 = _hom * RX::vec3(w/2, h/2, 1);

			for(int j = 0; j < _boards->size(); ++j)
			{
				_boardPos1[j] = RX::vec3((*_boards)[j][*_currentFrame].getX(0), (*_boards)[j][*_currentFrame].getY(0), 1);
				_boardPos2[j] = RX::vec3((*_boards)[j][*_currentFrame].getX(1), (*_boards)[j][*_currentFrame].getY(1), 1);
				_boardPos3[j] = RX::vec3((*_boards)[j][*_currentFrame].getX(2), (*_boards)[j][*_currentFrame].getY(2), 1);
				_boardPos4[j] = RX::vec3((*_boards)[j][*_currentFrame].getX(3), (*_boards)[j][*_currentFrame].getY(3), 1);
			}

			lastFrameShown = *_currentFrame;

			glEnable(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, _tex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, _frame->bits());

		}

		glBindTexture(GL_TEXTURE_2D, _tex);

		_persp.enable();
		
		vec3 f1 = _framePos1 * _scale;
		vec3 f2 = _framePos2 * _scale;
		vec3 f3 = _framePos3 * _scale;
		vec3 f4 = _framePos4 * _scale;

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(f1.x, f1.y, _framePos1.z);
		glTexCoord2f(0.0, 0.0); glVertex3f(f2.x, f2.y, _framePos2.z);
		glTexCoord2f(1.0, 0.0); glVertex3f(f3.x, f3.y, _framePos3.z);
		glTexCoord2f(1.0, 1.0); glVertex3f(f4.x, f4.y, _framePos4.z);
		glEnd();

		_persp.disable();

		glDisable(GL_TEXTURE_2D);

		for(int i = 0; i < _boards->size(); ++i)
		{
			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_LINE_LOOP);
			glVertex3f(_boardPos1[i].x * _scale, _boardPos1[i].y * _scale, 1);
			glVertex3f(_boardPos2[i].x * _scale, _boardPos2[i].y * _scale, 1);
			glVertex3f(_boardPos3[i].x * _scale, _boardPos3[i].y * _scale, 1);
			glVertex3f(_boardPos4[i].x * _scale, _boardPos4[i].y * _scale, 1);
			glEnd();
		}

		if(!_isUpdating) {
			//glColor3f(0.0, 1.0, 0.0);
			//glPointSize(3.0);
			//glBegin(GL_POINTS);
			//glVertex3f(_framePos1.x, _framePos1.y, _framePos1.z);
			//glVertex3f(_framePos2.x, _framePos2.y, _framePos2.z);
			//glVertex3f(_framePos3.x, _framePos3.y, _framePos3.z);
			//glVertex3f(_framePos4.x, _framePos4.y, _framePos4.z);
			//glEnd();
			//glColor3f(1.0, 0.0, 0.0);
			//glBegin(GL_POINTS);
			//if(_selectedP == 1) glVertex3f(_framePos1.x, _framePos1.y, _framePos1.z);
			//if(_selectedP == 2) glVertex3f(_framePos2.x, _framePos2.y, _framePos2.z);
			//if(_selectedP == 3) glVertex3f(_framePos3.x, _framePos3.y, _framePos3.z);
			//if(_selectedP == 4) glVertex3f(_framePos4.x, _framePos4.y, _framePos4.z);
			//glEnd();
		}
	}
}

void BoardWidget::setHomographies(vector<RX::mat3> *homographies)
{
	_homographies = homographies;
}

void BoardWidget::setBoards(vector< vector<BBox> > *boards)
{
	_boards = boards;
}

void BoardWidget::resizeGL(int w, int h)
{
	_width = w;
	_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void BoardWidget::apply()
{
	if(_isUpdating)
		return;
}

void BoardWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(_frame == NULL)
		return;
}
