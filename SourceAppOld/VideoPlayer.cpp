#include <phonon\videowidget.h>
#include "VideoPlayer.h"
using namespace RX;

VideoPlayer::VideoPlayer(QWidget* parent)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setMouseTracking(true);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::mousePressEvent(QMouseEvent *ev)
{
}

void VideoPlayer::mouseMoveEvent(QMouseEvent *ev)
{
}