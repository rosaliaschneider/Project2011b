#ifndef __FRAMEWIDGET_H
#define __FRAMEWIDGET_H

#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <RX/matn.h>
#include <RX/Shader.h>
#include <QGLWidget>
#include <QSemaphore>
#include <QImage>
#include <QTimer>
#include "BBox.h"

class FrameWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void nearPoint(int i);
	void valuesChanged(double *p);

public:
    FrameWidget(QWidget* parent = 0);
	~FrameWidget();

	void setFrame(QImage *frame) { _frame = frame; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }

	void setHomographies(vector<RX::mat3> *homographies);
	void setBoards(vector< vector<BBox> > *boards);
	void setUpdating(bool updating) { _isUpdating = updating; }
	void setOriginalDimensions(int width, int height) { _frameOrigW = width; _frameOrigH = height; }

	void setScale(double scale) { _scale = scale; }

	void apply();
		
 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

protected:
	void mouseMoveEvent(QMouseEvent *ev);

private:
	QImage *_frame;
	int *_currentFrame;
	double _scale;

	vector<RX::mat3> *_homographies;
	vector< vector<BBox> > *_boards;

	RX::mat3 _hom;
	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	RX::vec3 _boardPos1[9], _boardPos2[9], _boardPos3[9], _boardPos4[9];

	GLuint _tex;
	RX::Shader _persp;

	int _frameOrigW, _frameOrigH;
	int _width, _height;
	int _left, _right, _top, _bottom;
	QTimer _timer;

	bool _isUpdating;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H