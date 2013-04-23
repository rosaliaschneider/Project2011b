#ifndef __GLWIDGET_H
#define __GLWIDGET_H

#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <RX/matn.h>
#include <QGLWidget>
#include <QSemaphore>
#include <QImage>
#include <QTimer>
#include "SiftPoint.h"

class GLWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void nearPoint(int i);

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void setFrame1(QImage *frame1) { _frame1 = frame1; }
	void setFrame2(QImage *frame2) { _frame2 = frame2; }
	void setSifts1(vector<SiftPoint> *sifts1) { _sifts1 = sifts1; }
	void setSifts2(vector<SiftPoint> *sifts2) { _sifts2 = sifts2; }
	void setInliers(vector<bool> *inliers) { _inliers = inliers; }

	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	
protected:
	void mouseMoveEvent(QMouseEvent *ev);

private:
	QImage *_frame1, *_frame2;
	vector<SiftPoint> *_sifts1, *_sifts2;
	vector<bool> *_inliers;
	
	int *_currentFrame;
	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	GLuint _tex1, _tex2;
	int _selectedP;

	int _frameOrigW, _frameOrigH;
	int _width, _height;
	int _left, _right, _top, _bottom;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H