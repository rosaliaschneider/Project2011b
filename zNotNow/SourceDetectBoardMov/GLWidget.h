#ifndef __GLWIDGET_H
#define __GLWIDGET_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Movement.h"
#include "kht.h"
using namespace std;

class GLWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void selectedPoint(int point);

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFrame(QImage *frame) { _frame = frame; }
	void setMovements(vector<Movement> *movements) { _movements = movements; }
	void setBoardsPerFrame(vector< vector<BBox> > *boards) { _boards = boards; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void loadInfo(std::string filename);

	void addFirstPoints(int movementId);
	void saveBoards(std::string filename);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);

private:
	QImage *_frame;
	int *_currentFrame;
	int _numFrames, _dimx, _dimy;
	unsigned char *screen;

	bool _isMoving;
	vector<Movement> *_movements;
	vector< vector<BBox> > *_boards;
		
	lines_list_t lines;

	RX::vec2 _points[4];
	int _movement, _point;
	int _selected;

	GLuint tex;
	int _width, _height;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);

#endif // __GLWIDGET_H