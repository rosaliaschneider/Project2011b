#include <RX/Framebuffer.h>
#include <RX/ImageProcessor.h>
#include <QMouseEvent>
#include "Globals.h"
#include "GLWidget.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _cleaningFrame(0)
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
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glDisable(GL_DEPTH_TEST);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_texFrame);
	glGenTextures(1, &_texMask);
	
	_transparency.initialize("transpV.cg", "", "transpF.cg");
	_transparency.setFParameterTex(_texFrame, "decal");
	_transparency.setFParameterTex(_texMask, "alpha");

	glClear(GL_COLOR_BUFFER_BIT);
	_hom.setIdentity();
}

void GLWidget::paintGL()
{
	if(!homObj)
		return;

	if(homObj->lastReady() < video.frameNumber())
		return;

	QImage image = video.frame();
	if(image.width() == 0)
		return;
	QImage alpha = videoMask.frame();
	if(alpha.width() == 0)
		return;

	int h = image.height();
	int w = image.width();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texFrame);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

	glBindTexture(GL_TEXTURE_2D, _texMask);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, alpha.bits());
	
	RX::vec3 framePos1 = RX::vec3(-w/2, -h/2, 1);
	RX::vec3 framePos2 = RX::vec3(-w/2, h/2, 1);
	RX::vec3 framePos3 = RX::vec3(w/2, h/2, 1);
	RX::vec3 framePos4 = RX::vec3(w/2, -h/2, 1);

	_hom = homObj->hom(video.frameNumber()) * _hom;
	framePos1 = _hom * framePos1;
	framePos2 = _hom * framePos2;
	framePos3 = _hom * framePos3;
	framePos4 = _hom * framePos4;
	framePos1.divideByZ();
	framePos2.divideByZ();
	framePos3.divideByZ();
	framePos4.divideByZ();

	_transparency.enable();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x + 1, framePos1.y + 1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x + 1, framePos2.y,     1);
	glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x,     framePos3.y,     1);
	glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x,     framePos4.y + 1, 1);
	glEnd();
	_transparency.disable();

	glDisable(GL_TEXTURE_2D);	
	glFlush();

	unsigned char *buffer = new unsigned char[w*h*4];
	glReadPixels(width()/2 - w/2, height()/2 - h/2, w, h, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
	
	bool complete = true;
	for(int i = 0; i < w*h; ++i) 
	{
		if(buffer[i*4] == 0 && buffer[i*4 + 1] == 255 && buffer[i*4 + 2] == 0) 
		{
			complete = false;
			break;
		}
	}
	if(complete || video.frameNumber() - _cleaningFrame >= 500 || video.done())
	{
		// SAVE THIS FRAME
		char file[30];
		sprintf(file, "/frame_%d.png", _cleaningFrame);

		video.goToFrame(_cleaningFrame);
		videoMask.goToFrame(_cleaningFrame);

		image = video.frame();
		alpha = videoMask.frame();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texFrame);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

		glBindTexture(GL_TEXTURE_2D, _texMask);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, alpha.bits());
	
		RX::vec3 framePos1 = RX::vec3(-w/2, -h/2, 1);
		RX::vec3 framePos2 = RX::vec3(-w/2, h/2, 1);
		RX::vec3 framePos3 = RX::vec3(w/2, h/2, 1);
		RX::vec3 framePos4 = RX::vec3(w/2, -h/2, 1);

		_transparency.enable();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x + 1, framePos1.y + 1, 1);
		glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x + 1, framePos2.y,     1);
		glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x,     framePos3.y,     1);
		glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x,     framePos4.y + 1, 1);
		glEnd();
		_transparency.disable();
		
		glReadPixels(width()/2 - w/2, height()/2 - h/2, w, h, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
		QImage b(buffer, w, h, QImage::Format_RGB32);
		RX::ImageProcessor::flipV(&b);
		b.save((_folder+file).c_str());

		// DRAW NEXT FRAME
		++_cleaningFrame;

		glClear(GL_COLOR_BUFFER_BIT);
		video.goToFrame(_cleaningFrame);
		videoMask.goToFrame(_cleaningFrame);

		image = video.frame();
		alpha = videoMask.frame();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texFrame);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

		glBindTexture(GL_TEXTURE_2D, _texMask);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, alpha.bits());
	
		framePos1 = RX::vec3(-w/2, -h/2, 1);
		framePos2 = RX::vec3(-w/2, h/2, 1);
		framePos3 = RX::vec3(w/2, h/2, 1);
		framePos4 = RX::vec3(w/2, -h/2, 1);

		_transparency.enable();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(framePos1.x + 1, framePos1.y + 1, 1);
		glTexCoord2f(0.0, 1.0); glVertex3f(framePos2.x + 1, framePos2.y,     1);
		glTexCoord2f(1.0, 1.0); glVertex3f(framePos3.x,     framePos3.y,     1);
		glTexCoord2f(1.0, 0.0); glVertex3f(framePos4.x,     framePos4.y + 1, 1);
		glEnd();
		_transparency.disable();

		_hom.setIdentity();
	}
	delete[] buffer;
	video.next();
	videoMask.next();
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
