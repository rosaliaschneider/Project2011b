#ifndef __MAPOBJ_H
#define __MAPOBJ_H

#include <vector>
#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <RX/Framebuffer.h>
#include <RX/Shader.h>
#include "FramebufferWidget.h"

class MapObj : public QObject
{
    Q_OBJECT

public:
    MapObj();
    ~MapObj();

	void initialize();

	// Gets
	const int lastReady() const { return _lastReady; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	int _lastReady;
	RX::VideoDecoder _decoder;

	GLuint _texFrame, _texBg, _texLabels;
	RX::Shader _transparency, _grayscale;
	FramebufferWidget *fbContext;
};

#endif // __MAPOBJ_H