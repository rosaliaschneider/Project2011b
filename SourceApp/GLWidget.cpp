#include <QMouseEvent>
#include "Globals.h"
#include "GLWidget.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _scale(1.0), _bg(NULL), _bgW(0), _bgH(0)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();

	log = ofstream("log.txt");
}

GLWidget::~GLWidget()
{
	if(_bg != NULL)
		delete[] _bg;
}

void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_texBg);
	glGenTextures(1, &_texFrame);
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;

	QImage image = video.frame();
	if(image.width() != 0 && lastFrameShown != video.frameNumber() && homObj->lastReady() >= video.frameNumber())
	{
		lastFrameShown = video.frameNumber();

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, _texFrame);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
		int h = image.height();
		int w = image.width();
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
	
		RX::vec3 framePos1 = RX::vec3(-w/2, h/2, 1);
		RX::vec3 framePos2 = RX::vec3(-w/2, -h/2, 1);
		RX::vec3 framePos3 = RX::vec3(w/2, -h/2, 1);
		RX::vec3 framePos4 = RX::vec3(w/2, h/2, 1);

		framePos1 = homObj->hom(video.frameNumber()) * framePos1;
		framePos2 = homObj->hom(video.frameNumber()) * framePos2;
		framePos3 = homObj->hom(video.frameNumber()) * framePos3;
		framePos4 = homObj->hom(video.frameNumber()) * framePos4;
		framePos1.divideByZ();
		framePos2.divideByZ();
		framePos3.divideByZ();
		framePos4.divideByZ();
	
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(framePos1.x*_scale + _translate.x, framePos1.y*_scale + _translate.y, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(framePos2.x*_scale + _translate.x, framePos2.y*_scale + _translate.y, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(framePos3.x*_scale + _translate.x, framePos3.y*_scale + _translate.y, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(framePos4.x*_scale + _translate.x, framePos4.y*_scale + _translate.y, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	
		glFlush();
	}
}

void GLWidget::moveHorizontally(int howMuch)
{
	makeCurrent();

	int oldTranslate = _translate.x;
	_translate.x = (int)(width()*((howMuch-50)/100.0));

	glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, _bg);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBg);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _bg);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-width()/2 + _translate.x - oldTranslate, -height()/2, 1);
	glTexCoord2f(0.0, 0.0); glVertex3f(-width()/2 + _translate.x - oldTranslate, height()/2, 1);
	glTexCoord2f(1.0, 0.0); glVertex3f(width()/2 + _translate.x - oldTranslate,  height()/2, 1);
	glTexCoord2f(1.0, 1.0); glVertex3f(width()/2 + _translate.x- oldTranslate,  -height()/2, 1);
	glEnd();

}

void GLWidget::moveVertically(int howMuch)
{
	makeCurrent();

	int oldTranslate = _translate.y;
	_translate.y = (int)(height()*((howMuch-50)/100.0));

	glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, _bg);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBg);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _bg);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-width()/2, -height()/2 + _translate.y - oldTranslate, 1);
	glTexCoord2f(0.0, 0.0); glVertex3f(-width()/2, height()/2 + _translate.y - oldTranslate, 1);
	glTexCoord2f(1.0, 0.0); glVertex3f(width()/2,  height()/2 + _translate.y - oldTranslate, 1);
	glTexCoord2f(1.0, 1.0); glVertex3f(width()/2,  -height()/2 + _translate.y - oldTranslate, 1);
	glEnd();
}

void GLWidget::resizeGL(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glClear(GL_DEPTH_BUFFER_BIT);

	if(_bg != NULL)
		delete[] _bg;
	_bg = new unsigned char[width()*height()*4];
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
	int mousePosX = ev->pos().x() - width()/2.0 + video.width()/2;
	int mousePosY = ev->pos().y() - height()/2.0 + video.height()/2;

	int siftFrom = std::min(video.frameNumber(), siftObj->lastReady());
	
	int minDistance = 999999;
	int index = 0;
	int px, py;
	for(int i = 0; i < siftObj->numPoints(siftFrom); ++i)
	{
		int x = siftObj->siftPoints(siftFrom, i, 0);
		int y = siftObj->siftPoints(siftFrom, i, 1);

		int distance = (mousePosX-x)*(mousePosX-x) + (mousePosY-y)*(mousePosY-y);
		if(distance < minDistance)
		{
			minDistance = distance;
			index = i;
			px = x;
			py = y;
		}
	}
	emit goToFrame(siftObj->siftFrame(siftFrom, index));
	log << siftFrom << " " << mousePosX << " " << mousePosY << std::endl;
	log << siftFrom << " " << px << " " << py << " - " << siftObj->siftFrame(siftFrom, index) << std::endl;

}

void GLWidget::jumpTo(int region)
{
	//makeCurrent();
	//
	//Region r = regions[region];
	//QImage image = r.image();
	//
	//_bgH = image.height();
	//_bgW = image.width();
	//
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, _texBg);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _bgW, _bgH, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
	//
	//glClear(GL_COLOR_BUFFER_BIT);
	//RX::vec3 bgPos1 = RX::vec3(-_bgW/2, _bgH/2, 1);
	//RX::vec3 bgPos2 = RX::vec3(-_bgW/2, -_bgH/2, 1);
	//RX::vec3 bgPos3 = RX::vec3(_bgW/2,  -_bgH/2, 1);
	//RX::vec3 bgPos4 = RX::vec3(_bgW/2,  _bgH/2, 1);
	//glBegin(GL_QUADS);
	//glTexCoord2f(0.0, 1.0); glVertex3f(bgPos1.x, bgPos1.y, 1);
	//glTexCoord2f(0.0, 0.0); glVertex3f(bgPos2.x, bgPos2.y, 1);
	//glTexCoord2f(1.0, 0.0); glVertex3f(bgPos3.x, bgPos3.y, 1);
	//glTexCoord2f(1.0, 1.0); glVertex3f(bgPos4.x, bgPos4.y, 1);
	//glEnd();
}


