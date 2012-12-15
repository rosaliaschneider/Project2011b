#ifndef __GLWIDGET_H
#define __GLWIDGET_H

#include <vector>
#include <gl/glew.h>
#include <RX/vec3.h>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include "Board.h"
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

	void setSaveFrame(bool saveFrame) { _saveFrame = saveFrame; }
	void setBaseFile(std::string baseFile) { _baseFile = baseFile; }

	void setFolder(std::string folder) { _folder = folder; }
	void setFrame(QImage *frame) { _frame = frame; }
	void setCurrentFrame(int *currentFrame) { _currentFrame = currentFrame; }

	void addCorner(int board, int corner, int currentFrame, RX::vec2 pos);
	void setNumberOfBoards(int boards);
	void startMovement(int board, int currentFrame);
	void endMovement(int board, int currentFrame);
	void saveInfo(std::string filename);

	inline int width() const { return _width; }
	inline int height() const { return _height; }

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
	int *_currentFrame, _lastFrameShown;
	bool _saveFrame;
	std::string _folder;
	std::string _baseFile;

	vector<Board> _boards;
	vector<Movement> _professorOut;

	bool _selected;
	int _board, _point;
	double _scale;

	GLuint tex;
	int _width, _height;
	QTimer _timer;
};

#endif // __GLWIDGET_H