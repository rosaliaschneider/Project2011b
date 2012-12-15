#ifndef __GLWIDGET_H
#define __GLWIDGET_H

#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <RX/matn.h>
#include <QGLWidget>
#include <QMouseEvent>
#include <QImage>
#include <QTimer>
#include "BBox.h"

class GLWidget : public QGLWidget 
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFrame(QImage *frame) { _frame = frame; }
	void setReplace(QImage *replace) { _replace = replace; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }

 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

private:
	QImage *_frame, *_replace;
	int *_currentFrame;

	GLuint _tex;

	int _width, _height;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);
void fill(unsigned char *image, int w, int h, int bpp);
void fillRecursion(unsigned char *image, int w, int h, int bpp, int i, int j);


#endif // __GLWIDGET_H