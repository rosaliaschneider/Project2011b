#include <RX/Framebuffer.h>
#include <QMouseEvent>
#include "Globals.h"
#include "FramebufferWidget.h"

FramebufferWidget::FramebufferWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

FramebufferWidget::~FramebufferWidget()
{
}

void FramebufferWidget::initializeGL()
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

	if(_frameBuffer.isSupported())
		_frameBuffer.setFramebuffer();
}

void FramebufferWidget::paintGL()
{
}

void FramebufferWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT);
}

void FramebufferWidget::begin()
{
	makeCurrent();
	_frameBuffer.begin();
}

void FramebufferWidget::end()
{
	makeCurrent();
	_frameBuffer.end();
}

int FramebufferWidget::width()
{
	makeCurrent();
	return _frameBuffer.width();
}

int FramebufferWidget::height()
{
	makeCurrent();
	return _frameBuffer.height();
}

QImage FramebufferWidget::image()
{
	makeCurrent();
	return _frameBuffer.getImage();
}



