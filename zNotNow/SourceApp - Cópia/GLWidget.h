#ifndef __GLWIDGET_H
#define __GLWIDGET_H

#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/matn.h>
#include <RX/mat3.h>
#include <RX/Shader.h>
#include <QGLWidget>
#include <QSemaphore>
#include <QImage>
#include <QTimer>
#include <QMouseEvent>
#include "Board.h"
#include "BBox.h"
#include "Interval.h"

class GLWidget : public QGLWidget 
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFrame(QImage *frame) { _frame = frame; }
	void setFrame2(QImage *frame2) { _frame2 = frame2; }
	void setBackground(QImage *background, QImage *backgroundF);
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void setBoards(vector< vector<Board> > *boards) { _boards = boards; }
	void setIntervals(vector< vector<Interval> > *intervals) { _intervals = intervals; }
	void setPBBox(vector<BBox> *pbbox) { _pbbox = pbbox; }
	void setImages();

	void setHomographies(vector<RX::mat3> *homographies);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

protected:
	virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
	virtual void mousePressEvent(QMouseEvent *ev);

private:
	QImage *_frame, *_frame2, *_background, *_backgroundF;
	int _bw, _bh;

	vector < vector<Interval> > *_intervals;
	vector<int> _selectedImage, _defaultImage;

	int *_currentFrame;
	vector<RX::mat3> *_homographies;
	vector<RX::vec2> _centroids;

	vector< vector<Board> > *_boards;
	vector<BBox> *_pbbox;

	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	RX::mat3 _hom;
	
	GLuint _tex, _texBoard;
	GLuint _texBackground, _texBackgroundF;

	RX::Shader _persp;
	RX::Shader _noColor;
	
	int _width, _height;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H