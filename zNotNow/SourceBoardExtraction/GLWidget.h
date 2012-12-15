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
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void setHomographies(vector<RX::mat3> *homographies) { _homographies = homographies; }
	void setBoards(vector< vector<Board> > *boards) { _boards = boards; }
	void setPBBox(vector<BBox> *pbboxes) { _pbboxes = pbboxes; }

	void setIntervals();
	void saveIntervals(std::string filename);

 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

private:
	QImage *_frame;
	int *_currentFrame;
	vector< vector<Board> > *_boards;
	vector<BBox> *_pbboxes;
	vector<RX::mat3> *_homographies;
	vector< vector<Interval> > _intervals;

	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	RX::vec3 _oldFramePos1, _oldFramePos2, _oldFramePos3, _oldFramePos4;
	RX::mat3 _hom;
	BBox _pbbox;
	
	unsigned char *screen;
	unsigned int *quality;
	double _oldCost;

	int _width, _height;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);
void makeBlackToGray(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H