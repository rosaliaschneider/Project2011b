#include <RX/VideoDecoder.h>
#include <RX/SoftwareRenderer.h>
#include <RX/Framebuffer.h>
#include <QImage>
#include <QDir>
#include <QPainter> 
#include <QPrinter> 
#include <QGLContext>
#include "Globals.h"
#include "MapObj.h"

MapObj::MapObj()
: _lastReady(-1)
{
}

MapObj::~MapObj()
{
}

void MapObj::initialize()
{
	// get info
	video.start(folder.toStdString());

	fbContext = new FramebufferWidget();
	fbContext->resize(1000, 1000);
}

void MapObj::process()
{
	// wait until homObj ready
	while(!homObj);

	fbContext->initializeGL();
	fbContext->begin();
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	
	fbContext->end();

	QImage map;

	RX::vec3 pos1, pos2, pos3, pos4;
	RX::vec3 oldPos1, oldPos2, oldPos3, oldPos4;

	int w = video.width();
	int h = video.height();
	oldPos1 = RX::vec3(0, 0, 1);
	oldPos2 = RX::vec3(0, 0, 1);
	oldPos3 = RX::vec3(0, 0, 1);
	oldPos4 = RX::vec3(0, 0, 1);
	
	while(_lastReady < video.totalFrames())
	{
		if(_lastReady < homObj->lastReady())
		{
			QImage image = video.frame(_lastReady+1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, _texFrame);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

			pos1 = homObj->hom(_lastReady+1) * RX::vec3(-w/2, h/2, 1);
			pos2 = homObj->hom(_lastReady+1) * RX::vec3(-w/2, -h/2, 1);
			pos3 = homObj->hom(_lastReady+1) * RX::vec3(w/2, -h/2, 1);
			pos4 = homObj->hom(_lastReady+1) * RX::vec3(w/2, h/2, 1);
		
			fbContext->begin();
		
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f(pos1.x + 1 + w, pos1.y + 1 + h, 1);
			glTexCoord2f(0.0, 0.0); glVertex3f(pos2.x + 1 + w, pos2.y     + h, 1);
			glTexCoord2f(1.0, 0.0); glVertex3f(pos3.x     + w, pos3.y     + h, 1);
			glTexCoord2f(1.0, 1.0); glVertex3f(pos4.x     + w, pos4.y + 1 + h, 1);
			glEnd();
		
			fbContext->end();

			double dist = 0;
			dist += abs(pos1.x - oldPos1.x) + abs(pos1.y - oldPos1.y);
			dist += abs(pos2.x - oldPos2.x) + abs(pos2.y - oldPos2.y);
			dist += abs(pos3.x - oldPos3.x) + abs(pos3.y - oldPos3.y);
			dist += abs(pos4.x - oldPos4.x) + abs(pos4.y - oldPos4.y);

			if(dist > 10)
			{
				char buffer[100];
				sprintf(buffer, "/Maps/map_%d.png", _lastReady+1);
				map = fbContext->image();
				map.save(folder+buffer);
				clickMap.addImage(_lastReady+1);

				oldPos1 = pos1;
				oldPos2 = pos2;
				oldPos3 = pos3;
				oldPos4 = pos4;
			}

			++_lastReady;
		}
	}
}