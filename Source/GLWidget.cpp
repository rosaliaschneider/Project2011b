#include <RX/Framebuffer.h>
#include <QMouseEvent>
#include "Globals.h"
#include "GLWidget.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _scale(1.0), _mapEnabled(false), _maxFrameSeen(-1)
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
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glDisable(GL_DEPTH_TEST);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_texBg);
	glGenTextures(1, &_texFrame);
	glGenTextures(1, &_texLabels);

	_transparency.initialize("transpV.cg", "", "transpF.cg");
	_transparency.setFParameterTex(_texFrame, "decal");
	_transparency.setFParameterTex(_texLabels, "alpha");

	_grayscale.initialize("grayV.cg", "", "grayF.cg");
	_grayscale.setFParameterTex(_texBg, "decal");
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;

	QImage image = video.frame();
	if(image.width() == 0)
		return;

	_mapOffset = RX::vec2(500 - video.width(), 500 - video.height());

	if(homObj)
	{
		if(homObj->lastReady() >= video.frameNumber())
		{
			if(lastFrameShown == -1) {
				QImage labels = video.labels();
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, _texLabels);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, labels.width(), labels.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, labels.bits());
			}
			
			lastFrameShown = video.frameNumber();
			if(lastFrameShown > _maxFrameSeen)
				_maxFrameSeen = lastFrameShown;
			
			glEnable(GL_TEXTURE_2D);
			
			if(_mapEnabled)
			{
				QImage bg = clickMap.image(_maxFrameSeen);
				QImage bg2 = video.frame(_maxFrameSeen);
			
				glBindTexture(GL_TEXTURE_2D, _texBg);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg.bits());

				glBindTexture(GL_TEXTURE_2D, _texFrame);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg2.width(), bg2.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg2.bits());
			
				glClear(GL_COLOR_BUFFER_BIT);
			
				_grayscale.enable();
			
				glBindTexture(GL_TEXTURE_2D, _texBg);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
				glTexCoord2f(0.0, 1.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, bg.height()/2  + _mapOffset.y + _translate.y,     1);
				glTexCoord2f(1.0, 1.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     bg.height()/2  + _mapOffset.y + _translate.y,     1);
				glTexCoord2f(1.0, 0.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
				glEnd();

				int h = bg2.height();
				int w = bg2.width();
				RX::vec3 framePos1 = homObj->hom(_maxFrameSeen) * RX::vec3(-w/2, -h/2, 1);
				RX::vec3 framePos2 = homObj->hom(_maxFrameSeen) * RX::vec3(-w/2, h/2, 1);
				RX::vec3 framePos3 = homObj->hom(_maxFrameSeen) * RX::vec3(w/2, h/2, 1);
				RX::vec3 framePos4 = homObj->hom(_maxFrameSeen) * RX::vec3(w/2, -h/2, 1);
				framePos1.divideByZ();
				framePos2.divideByZ();
				framePos3.divideByZ();
				framePos4.divideByZ();

				glBindTexture(GL_TEXTURE_2D, _texFrame);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x*_scale + _translate.x + 1, framePos1.y*_scale + _translate.y + 1, 1);
				glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x*_scale + _translate.x + 1, framePos2.y*_scale + _translate.y,     1);
				glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x*_scale + _translate.x,     framePos3.y*_scale + _translate.y,     1);
				glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x*_scale + _translate.x,     framePos4.y*_scale + _translate.y + 1, 1);
				glEnd();
			
				_grayscale.disable();
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, _texFrame);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			
				int h = image.height();
				int w = image.width();
			
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
			
				RX::vec3 framePos1 = homObj->hom(video.frameNumber()) * RX::vec3(-w/2, -h/2, 1);
				RX::vec3 framePos2 = homObj->hom(video.frameNumber()) * RX::vec3(-w/2, h/2, 1);
				RX::vec3 framePos3 = homObj->hom(video.frameNumber()) * RX::vec3(w/2, h/2, 1);
				RX::vec3 framePos4 = homObj->hom(video.frameNumber()) * RX::vec3(w/2, -h/2, 1);
				framePos1.divideByZ();
				framePos2.divideByZ();
				framePos3.divideByZ();
				framePos4.divideByZ();

				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x*_scale + _translate.x + 1, framePos1.y*_scale + _translate.y + 1, 1);
				glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x*_scale + _translate.x + 1, framePos2.y*_scale + _translate.y,     1);
				glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x*_scale + _translate.x,     framePos3.y*_scale + _translate.y,     1);
				glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x*_scale + _translate.x,     framePos4.y*_scale + _translate.y + 1, 1);
				glEnd();

				glDisable(GL_TEXTURE_2D);
			
				glFlush();
			}
		}
	}
	if(homMapObj)
	{
		if(homMapObj->lastReady() >= video.frameNumber())
		{
			if(lastFrameShown == -1) {
				QImage labels = video.labels();
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, _texLabels);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, labels.width(), labels.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, labels.bits());
			}

			lastFrameShown = video.frameNumber();
			if(lastFrameShown > _maxFrameSeen)
				_maxFrameSeen = lastFrameShown;

			glEnable(GL_TEXTURE_2D);

			if(_mapEnabled)
			{
				QImage bg = clickMap.image(_maxFrameSeen);

				glBindTexture(GL_TEXTURE_2D, _texBg);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg.bits());
	
				glClear(GL_COLOR_BUFFER_BIT);
	
				_grayscale.enable();

				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
				glTexCoord2f(0.0, 1.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, bg.height()/2  + _mapOffset.y + _translate.y,     1);
				glTexCoord2f(1.0, 1.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     bg.height()/2  + _mapOffset.y + _translate.y,     1);
				glTexCoord2f(1.0, 0.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
				glEnd();

				_grayscale.disable();
			}

			glBindTexture(GL_TEXTURE_2D, _texFrame);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
			int h = image.height();
			int w = image.width();
	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
	
			RX::vec3 framePos1 = RX::vec3(-w/2, -h/2, 1);
			RX::vec3 framePos2 = RX::vec3(-w/2, h/2, 1);
			RX::vec3 framePos3 = RX::vec3(w/2, h/2, 1);
			RX::vec3 framePos4 = RX::vec3(w/2, -h/2, 1);

			framePos1 = homMapObj->hom(video.frameNumber()) * framePos1;
			framePos2 = homMapObj->hom(video.frameNumber()) * framePos2;
			framePos3 = homMapObj->hom(video.frameNumber()) * framePos3;
			framePos4 = homMapObj->hom(video.frameNumber()) * framePos4;
			framePos1.divideByZ();
			framePos2.divideByZ();
			framePos3.divideByZ();
			framePos4.divideByZ();

			_transparency.setFParameter1f(_mapEnabled, "mapEnabled");
			_transparency.enable();
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x*_scale + _translate.x + 1, framePos1.y*_scale + _translate.y + 1, 1);
			glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x*_scale + _translate.x + 1, framePos2.y*_scale + _translate.y,     1);
			glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x*_scale + _translate.x,     framePos3.y*_scale + _translate.y,     1);
			glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x*_scale + _translate.x,     framePos4.y*_scale + _translate.y + 1, 1);
			glEnd();
			_transparency.disable();


			glDisable(GL_TEXTURE_2D);
	
			glFlush();
		}
	}
}

void GLWidget::moveHorizontally(int howMuch)
{
	makeCurrent();

	QImage bg = clickMap.image(video.frameNumber());

	_translate.x = (int)(bg.width()*((howMuch-50)/100.0));
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBg);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg.bits());
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 1.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 0.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glEnd();

}

void GLWidget::moveVertically(int howMuch)
{
	makeCurrent();

	QImage bg = clickMap.image(video.frameNumber());

	_translate.y = (int)(bg.height()*((howMuch-50)/100.0));
		
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBg);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg.bits());
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 1.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 0.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glEnd();
}

void GLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
	int mousePosX = ev->pos().x() - width()/2.0 + video.width()/2;
	int mousePosY = ev->pos().y() - height()/2.0 + video.height()/2;
	
	int frameNumber;
	if(_mapEnabled)
		frameNumber = _maxFrameSeen;
	else
		frameNumber = video.frameNumber();

	mousePosX -= _translate.x;
	mousePosY -= _translate.y;

	int minDistance = 999999;
	int index = 0;
	int px, py;
	for(int i = 0; i < homObj->numGlobalSifts(); ++i)
	{
		int x = homObj->globalSiftX(i);
		int y = homObj->globalSiftY(i);

		int distance = (mousePosX-x)*(mousePosX-x) + (mousePosY-y)*(mousePosY-y);
		if(distance < minDistance)
		{
			minDistance = distance;
			index = i;
			px = x;
			py = y;
		}
	}

	QImage bg = clickMap.image(homObj->globalSiftFrame(index));
		
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBg);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bg.bits());
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(-bg.width()/2 + _mapOffset.x + _translate.x + 1, bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 1.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     bg.height()/2  + _mapOffset.y + _translate.y,     1);
	glTexCoord2f(1.0, 0.0); glVertex3f(bg.width()/2  + _mapOffset.x + _translate.x,     -bg.height()/2 + _mapOffset.y + _translate.y + 1, 1);
	glEnd();

	_mapEnabled = false;

	emit goToFrame(homObj->globalSiftFrame(index));
}

void GLWidget::keyPressEvent(QKeyEvent *ev)
{
	if(ev->isAutoRepeat())
		return;

	switch(ev->key())
	{
	case Qt::Key_V:
		qDebug("keyPress");
		_mapEnabled = true;
		break;
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent *ev)
{
	if(ev->isAutoRepeat())
		return;

	switch(ev->key())
	{
	case Qt::Key_V:
		qDebug("keyRelease");
		_mapEnabled = false;
		break;
	}
}



