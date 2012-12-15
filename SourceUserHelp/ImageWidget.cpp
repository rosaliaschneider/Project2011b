#include <QMouseEvent>
#include "ImageWidget.h"

ImageWidget::ImageWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _image(NULL)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setMouseTracking(true);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::initializeGL()
{
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void ImageWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if(_image)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, tex );
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		int h = _image->height();
		int w = _image->width();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, _image->bits());

		RX::vec2 framePos1 = RX::vec2(-w/2, h/2);
		RX::vec2 framePos2 = RX::vec2(-w/2, -h/2);
		RX::vec2 framePos3 = RX::vec2(w/2, -h/2);
		RX::vec2 framePos4 = RX::vec2(w/2, h/2);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(framePos1.x, framePos1.y, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(framePos2.x, framePos2.y, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(framePos3.x, framePos3.y, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(framePos4.x, framePos4.y, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		glFlush();
	}
}

void ImageWidget::resizeGL(int w, int h)
{
	_width = w;
	_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, -1000, 1000);
}
