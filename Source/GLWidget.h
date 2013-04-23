#ifndef __GLWidget_H
#define __GLWidget_H

#include <vector>
#include <fstream>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/Shader.h>
#include <RX/Framebuffer.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
using namespace std;

class GLWidget : public QGLWidget 
{
    Q_OBJECT

signals:
	void goToFrame(int number);

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

	void keyPressEvent(QKeyEvent *ev);
	void keyReleaseEvent(QKeyEvent *ev);

private:
	double _scale;
	RX::vec2 _translate;
	RX::vec2 _mapOffset;

	bool _mapEnabled;
	int _maxFrameSeen;
	RX::Shader _transparency, _grayscale;

	GLuint _texFrame, _texBg, _texLabels;
	QTimer _timer;
};

#endif // __GLWidget_H