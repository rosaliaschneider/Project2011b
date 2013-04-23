#ifndef __GLWidget_H
#define __GLWidget_H

#include <vector>
#include <fstream>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/mat3.h>
#include <RX/Shader.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
using namespace std;

class GLWidget : public QGLWidget 
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0);
	~GLWidget();

	void setFolder(std::string folder) { _folder = folder; }

protected:
	void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
	RX::Shader _transparency;
	GLuint _texFrame, _texMask;
	QTimer _timer;
	RX::mat3 _hom;

	int _cleaningFrame;
	QImage _frame, _mask;
	std::string _folder;
};

#endif // __GLWidget_H