#ifndef __FRAMEBUFFERWIDGET_H
#define __FRAMEBUFFERWIDGET_H

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

class MapObj;

class FramebufferWidget : public QGLWidget 
{
    Q_OBJECT
	
	friend class MapObj;

public:
    FramebufferWidget(QWidget* parent = 0);
	~FramebufferWidget();

	void begin();
	void end();
	int width();
	int height();
	QImage image();

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
	RX::Shader _transparency, _grayscale;
	GLuint _texFrame, _texBg, _texLabels;
	RX::Framebuffer _frameBuffer;
};

#endif // __FRAMEBUFFERWIDGET_H