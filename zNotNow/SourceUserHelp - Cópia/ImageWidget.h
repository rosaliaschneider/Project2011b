#ifndef __IMAGEWIDGET_H
#define __IMAGEWIDGET_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Board.h"
using namespace std;

class ImageWidget : public QGLWidget 
{
    Q_OBJECT
		
public:
    ImageWidget(QWidget* parent = 0);
	~ImageWidget();

	inline int width() const { return _width; }
	inline int height() const { return _height; }

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
	QImage *_image;

	GLuint tex;
	int _width, _height;
	QTimer _timer;
};

#endif // __IMAGEWIDGET_H