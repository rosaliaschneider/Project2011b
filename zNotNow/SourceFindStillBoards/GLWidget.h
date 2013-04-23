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
	void setAlpha(QImage *alpha) { _alpha = alpha; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }

	void setBoards(vector< vector<BBox> > *boards) { _boards = boards; }
	void setHomographies(vector<RX::mat3> *homographies) { _homographies = homographies; }

 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

private:
	QImage *_frame, *_alpha;
	int *_currentFrame;
		
	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	
	unsigned char *_screen;
	int _width, _height;
	QTimer _timer;

	RX::mat3 hom;
	vector< RX::mat3 > *_homographies;
	vector< vector<BBox> > *_boards;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H