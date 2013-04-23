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

	// parameters
	void setFrame(QImage *frame) { _frame = frame; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }
	void setBoards(vector< vector<BBox> > *boards);
	void setHomographies(vector<RX::mat3> *homographies);
	void setGlobalHomographies(vector<RX::mat3> *globalHomographies);
	void setScale(double scale) { _scale = scale; }
	
	// interaction
	void addCorner(int board, int corner, int currentFrame, RX::vec2 pos);
	void removeFrame();
	void startMovingFrame();
	void moveLeft(int pixels, int point=-1);
	void moveRight(int pixels, int point=-1);
	void moveUp(int pixels, int point=-1);
	void moveDown(int pixels, int point=-1);
	void correctHomographies();

	// opengl
 	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

protected:
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent  *ev);

private:
	// parameters
	QImage *_frame;
	int *_currentFrame;
	double _scale;
	vector<RX::mat3> *_homographies;
	vector<RX::mat3> *_globalHomographies;
	vector< vector<BBox> > *_boards;

	// interaction
	RX::vec3 _framePos[4];
	RX::vec3 _boardPos[9][4];
	RX::vec2 _newPoints[4], _oldPoints[4];
	bool _selected;
	int _board, _point;

	// auto detect changes
	unsigned char *buffer, *buffer2;
	RX::vec3 _translateToFit;

	// opengl
	GLuint _tex;
	RX::Shader _persp;
	QTimer _timer;
};

void flipV(unsigned char *image, int w, int h, int bpp);


#endif // __GLWIDGET_H
