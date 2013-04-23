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

class VideoPlayer : public Phonon::VideoPlayer
{
    Q_OBJECT

public:
    VideoPlayer(QWidget* parent = 0);
	~VideoPlayer();

protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev);
	
private:
};

#endif // __VIDEOPLAYER_H