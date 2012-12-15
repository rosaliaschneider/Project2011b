#include <QMouseEvent>
#include "GLWidget.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), _currentFrame(NULL), _lastFrameShown(-1), _saveFrame(false), _selected(false), _baseFile(), _scale(1.0)
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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void GLWidget::paintGL()
{
	if(_frame)
	{
		bool first = false;
		if(_lastFrameShown != *_currentFrame) 
			first = true;
		_lastFrameShown = *_currentFrame;

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, tex );
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		int h = _frame->height();
		int w = _frame->width();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, _frame->bits());

		RX::vec2 framePos1 = RX::vec2(-w/2, h/2);
		RX::vec2 framePos2 = RX::vec2(-w/2, -h/2);
		RX::vec2 framePos3 = RX::vec2(w/2, -h/2);
		RX::vec2 framePos4 = RX::vec2(w/2, h/2);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(framePos1.x*_scale, framePos1.y*_scale, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(framePos2.x*_scale, framePos2.y*_scale, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(framePos3.x*_scale, framePos3.y*_scale, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(framePos4.x*_scale, framePos4.y*_scale, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		glFlush();

		for(int i = 0; i < _boards.size(); ++i)
			_boards[i].draw(*_currentFrame);
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

	glClear(GL_DEPTH_BUFFER_BIT);
}

void GLWidget::setNumberOfBoards(int boards)
{
	for(int i = 0; i < boards; ++i) {
		_boards.push_back(Board());
	}
}
void GLWidget::addCorner(int board, int corner, int currentFrame, RX::vec2 pos)
{
	_boards[board].frames.push_back(currentFrame);
	_boards[board].points.push_back(pos);
	_boards[board].corners.push_back(corner);

}
void GLWidget::startMovement(int board, int currentFrame)
{
	_boards[board].addMovement(currentFrame);
}
void GLWidget::endMovement(int board, int currentFrame)
{
	_boards[board].endMovement(currentFrame);
}

void GLWidget::saveInfo(std::string filename)
{
	if(_currentFrame == NULL) return;

	ofstream output((_folder + filename).c_str());

	output << _lastFrameShown+1 << endl;
	output << _frame->width() << " " << _frame->height() << endl;

	// remove useless boards
	for(int i = 0; i < _boards.size(); ++i) {
		if(_boards[i].points.size() == 0) {
			--i;
			_boards.pop_back();
		}
	}

	output << _boards.size() << endl;
	for(int i = 0; i < _boards.size(); ++i) {
		output << _boards[i].points.size()/4 << endl;
		
		for(int j = 0; j < _boards[i].points.size()/4; ++j) {
			output << _boards[i].points[j*4 + 0].x/_scale << " " << _boards[i].points[j*4 + 0].y/_scale << " ";
			output << _boards[i].points[j*4 + 1].x/_scale << " " << _boards[i].points[j*4 + 1].y/_scale << " ";
			output << _boards[i].points[j*4 + 2].x/_scale << " " << _boards[i].points[j*4 + 2].y/_scale << " ";
			output << _boards[i].points[j*4 + 3].x/_scale << " " << _boards[i].points[j*4 + 3].y/_scale << endl;
		}

		for(int j = 0; j < _boards[i].frames.size()/4; ++j)
			output << _boards[i].frames[j*4] << " ";
		output << endl;

		output << _boards[i].moves.size() << endl;
		for(int j = 0; j < _boards[i].moves.size(); ++j) {
			output << _boards[i].moves[j].start << " " << _boards[i].moves[j].end << endl;
		}

	}
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
		RX::vec2 mousePos = RX::vec2(((ev->pos()).x()-_width/2), ((ev->pos()).y()-_height/2));
		addCorner(_board, _point, *_currentFrame, mousePos);
		_selected = false;
	}
	//else {
	//	for(int i = 0; i < _boards.size(); ++i) {
	//		for(int j = 0; j < 4; ++j) {
	//			if(_boards[i].frames[j] == -1 || _boards[i].initFrames[j] > *_currentFrame)
	//				continue;
	//	
	//			float distx = _boards[i].initPoints[j].x - ((ev->pos()).x()-_width/2);
	//			float disty = _boards[i].initPoints[j].y - ((ev->pos()).y()-_height/2);
	//			float dist = sqrt(distx*distx + disty*disty);
	//			if(dist <= 3) {
	//				_board = i;
	//				_point = j;
	//				_selected = true;
	//				emit selectedBoardAndPoint(i, j);
	//				return;
	//			}
	//		}
	//	}
	//}
}

void GLWidget::mouseMoveEvent(QMouseEvent *ev)
{
	int mouseX = (ev->pos()).x();
	int mouseY = (ev->pos()).y();

	//if(_selected) {
	//	_boards[_board].initPoints[_point].x = mouseX-_width/2;
	//	_boards[_board].initPoints[_point].y = mouseY-_height/2;
	//}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *ev)
{

}
