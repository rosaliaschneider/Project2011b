#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include <gl/glew.h>
#include <RX/vec3.h>
#include <RX/matn.h>
#include <RX/mat3.h>
#include <RX/Shader.h>
#include <QSemaphore>
#include <QImage>
#include <QTimer>
#include <QMouseEvent>
#include <Phonon/mediaobject.h>
#include <Phonon/videoplayer.h>
#include "Board.h"
#include "Interval.h"

class VideoPlayer : public Phonon::VideoPlayer
{
    Q_OBJECT

public:
    VideoPlayer(QWidget* parent = 0);
	~VideoPlayer();

	void setBoards(vector<Board> *boards) { _boards = boards; }

protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev);

private:
	vector<Board> *_boards;

};

#endif // __VIDEOPLAYER_H