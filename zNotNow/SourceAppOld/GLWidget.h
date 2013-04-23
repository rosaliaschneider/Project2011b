#ifndef __GLWidget_H
#define __GLWidget_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Board.h"
#include "Region.h"
#include "VideoPlayer.h"
using namespace std;

class GLWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void selectedBoard(int board);
	void selectedPoint(int point);
	void selectedBoardAndPoint(int board, int point);

public slots:
	void setScale(double scale) { _scale = scale; }  

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFrame(QImage *frame);
	void setVideoPlayer(VideoPlayer *vp) { _vp = vp; }
	void setBoards(vector<Board> *boards) { _boards = boards; }
	void setRegions(vector<Region> *regions) { _regions = regions; }

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent  *ev);

private:
	QImage *_frame;
	vector<Board> *_boards;
	vector<Region> *_regions;
	int _board;
	int _region;
	double _scale;

	GLuint tex;
	QTimer _timer;
	VideoPlayer *_vp;
};

#endif // __GLWidget_H