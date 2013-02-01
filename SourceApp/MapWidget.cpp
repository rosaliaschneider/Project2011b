#include <QMouseEvent>
#include "MapWidget.h"
#include "Globals.h" 

MapWidget::MapWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _scale(1.0), _board(-1), _region(-1)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();
}

MapWidget::~MapWidget()
{
}

void MapWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void MapWidget::paintGL()
{
	static bool first = true;

	int h = clickMap.height();
	int w = clickMap.width();

	glEnable(GL_TEXTURE_2D);

	if(first && w != 0)
	{
		first = false;

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, clickMap.bits());

	}

	glClear(GL_COLOR_BUFFER_BIT);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//if(_region != -1)
	//{
	//	glColor4f((*_regions)[_region].color().x, (*_regions)[_region].color().y, (*_regions)[_region].color().z, 0.7f); 
	//	for(int i = 0; i < (*_regions)[_region].boxes().size(); ++i)
	//	{
	//		glBegin(GL_QUADS);
	//		BBox box = (*_regions)[_region].boxes()[i];
	//		glVertex3f(box.points[0].x, box.points[0].y, 1);
	//		glVertex3f(box.points[1].x, box.points[1].y, 1);
	//		glVertex3f(box.points[2].x, box.points[2].y, 1);
	//		glVertex3f(box.points[3].x, box.points[3].y, 1);
	//		glEnd();
	//	}
	//}

	glDisable(GL_BLEND);
	
	glFlush();
}

void MapWidget::resizeGL(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glClear(GL_DEPTH_BUFFER_BIT);
}

void MapWidget::keyPressEvent(QKeyEvent  *ev)
{

}

void MapWidget::mousePressEvent(QMouseEvent *ev)
{
	int mousePosX = ev->pos().x() - width()/2.0;
	int mousePosY = ev->pos().y() - height()/2.0;;

	//unsigned long milliseconds = this->currentTime();
	unsigned int frame = 10;//milliseconds/100;

	//for(int i = 0; i < _boards->size(); ++i)
	//{
	//	BBox b = (*_boards)[i].getPosition(frame);
	//	if(b.isInside(mousePosX, mousePosY))
	//	{
	//		int time = (*_boards)[i].getTime(frame);
	//		if(time != -1)
	//			_vp->seek(time);
	//	}
	//}

	for(int i = 0; i < regions.size(); ++i)
	{
		Region r = regions[i];
		for(int j = 0; j < r.nBoxes(); ++j)
		{
			RX::BBox b = r.box(j);
			if(b.isInside(RX::vec2(mousePosX, mousePosY)))
			{
				emit goToRegion(i);
			}
		}
	}
}

void MapWidget::mouseMoveEvent(QMouseEvent *ev)
{
	int mousePosX = ev->pos().x() - width()/2.0;
	int mousePosY = ev->pos().y() - height()/2.0;;

	_board = -1;
	//for(int i = 0; i < _boards->size(); ++i)
	//{
	//	BBox b = (*_boards)[i].getPosition(0);
	//	if(b.isInside(mousePosX, mousePosY))
	//	{
	//		_board = i;
	//	}
	//}

	_region = -1;
	for(int i = 0; i < regions.size(); ++i)
	{
		Region r = regions[i];
		for(int j = 0; j < r.nBoxes(); ++j)
		{
			RX::BBox b = r.box(j);
			if(b.isInside(RX::vec2(mousePosX, mousePosY)))
			{
				_region = i;
				break;
			}
		}
		if(_region != -1)
			break;
	}
}

void MapWidget::mouseReleaseEvent(QMouseEvent *ev)
{
}
