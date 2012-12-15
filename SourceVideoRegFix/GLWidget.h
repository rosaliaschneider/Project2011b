#ifndef __GLWIDGET_H
#define __GLWIDGET_H

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

struct FramePosition
{
	RX::vec3 pos1, pos2, pos3, pos4;
};

class GLWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void selectedBoard(int board);
	void selectedPoint(int point);

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFrame(QImage *frame) { _frame = frame; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void setBoards(vector< vector<BBox> > *boards);
	void setOriginalDimensions(int width, int height) { _frameOrigW = width; _frameOrigH = height; }
	void setScale(double scale) { _scale = scale; }

	void setHomographies(vector<RX::mat3> *homographies);
	void setGlobalHomographies(vector<RX::mat3> *globalHomographies);

	void addCorner(int board, int corner, int currentFrame, RX::vec2 pos);
	void correctHomographies();
			
 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

protected:
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent  *ev);

private:
	QImage *_frame;
	int *_currentFrame;
	double _scale;

	vector<RX::mat3> *_homographies;
	vector<RX::mat3> *_globalHomographies;
	vector< vector<BBox> > *_boards;

	RX::mat3 _hom;
	RX::vec3 _framePos1, _framePos2, _framePos3, _framePos4;
	RX::vec3 _boardPos[9][4];

	RX::vec2 _newPoints[4], _oldPoints[4];

	GLuint _tex;
	RX::Shader _persp;

	bool _selected;
	int _board, _point;

	int _frameOrigW, _frameOrigH;
	int _width, _height;
	int _left, _right, _top, _bottom;
	QTimer _timer;

	bool _isUpdating;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H