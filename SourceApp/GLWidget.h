#ifndef __GLWidget_H
#define __GLWidget_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Resources.h"
#include "VideoPlayer.h"
using namespace std;

class GLWidget : public QGLWidget 
{
    Q_OBJECT

public slots:
	void moveHorizontally(int howMuch);
	void moveVertically(int howMuch);
	void setScale(double scale) { _scale = scale; }  

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void play();
	void pause();
	void jumpTo(int region);

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent  *ev);

private:
	double _scale;
	RX::vec2 _translate;

	GLuint _texFrame, _texBg;
	QTimer _timer;

	unsigned char *_bg;
	int _bgW, _bgH;
};

#endif // __GLWidget_H