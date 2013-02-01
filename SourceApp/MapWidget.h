#ifndef __MAPWIDGET_H
#define __MAPWIDGET_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Region.h"
#include "VideoPlayer.h"
using namespace std;

class MapWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void goToRegion(int region);
	//void selectedPoint(int point);
	//void selectedBoardAndPoint(int board, int point);

public slots:
	void setScale(double scale) { _scale = scale; }  

public:
    MapWidget(QWidget* parent = 0);
	~MapWidget();

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent  *ev);

private:
	int _board;
	int _region;
	double _scale;

	GLuint tex;
	QTimer _timer;
};

#endif // __MAPWIDGET_H