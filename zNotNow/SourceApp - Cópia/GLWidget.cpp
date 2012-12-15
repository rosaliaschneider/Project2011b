#include "Define.h"
#include "GLWidget.h"
using namespace RX;

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL)
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(10);
	_timer.start();
}

GLWidget::~GLWidget()
{
}

void GLWidget::setImages()
{
}

void GLWidget::setBackground(QImage *background, QImage *backgroundF)
{
	_background = background;
	_bh = _background->height();
	_bw = _background->width();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBackground);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _bw, _bh, 0, GL_BGRA, GL_UNSIGNED_BYTE, _background->bits());

	_backgroundF = backgroundF;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texBackgroundF);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _bw, _bh, 0, GL_BGRA, GL_UNSIGNED_BYTE, _backgroundF->bits());
}

void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &_tex);
	glGenTextures(1, &_texBackground);
	glGenTextures(1, &_texBackgroundF);

	_hom.setIdentity();

	_persp.initialize("perspV.cg", "", "perspF.cg");
	_persp.setFParameterTex(_tex, "decal");

	_noColor.initialize("noColorV.cg", "", "noColorF.cg");
	_noColor.setFParameter4f(0.0, 1.0, 0.0, 1.0, "colorRemove");
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool first = true;

	if (_frame) 
	{
		if(lastFrameShown == -1) {
			for(int i = 0; i < (*_intervals).size(); ++i) {
				_selectedImage.push_back(-1);
				_defaultImage.push_back(-1);
			}
		}
		if(lastFrameShown != *_currentFrame)
		{
			lastFrameShown = *_currentFrame;
			_hom = _hom * (*_homographies)[lastFrameShown];
		}

		int h = _frame->height();
		int w = _frame->width();

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, _tex);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, _frame->bits());

		
		_framePos1 = _hom * RX::vec3(-w/2, h/2, 1);
		_framePos2 = _hom * RX::vec3(-w/2, -h/2, 1);
		_framePos3 = _hom * RX::vec3(w/2, -h/2, 1);
		_framePos4 = _hom * RX::vec3(w/2, h/2, 1);

		glBindTexture( GL_TEXTURE_2D, _texBackground);
		_noColor.setFParameterTex(_texBackground, "decal");
		_noColor.enable();

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(-_bw*3/2, _bh*3/2, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(-_bw*3/2, -_bh*3/2, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(_bw*3/2, -_bh*3/2, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(_bw*3/2, _bh*3/2, 1);
		glEnd();

		_noColor.disable();
		
		glBindTexture( GL_TEXTURE_2D, _tex);

		_persp.enable();
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(_framePos1.x*3, _framePos1.y*3, _framePos1.z);
		glTexCoord2f(0.0, 0.0); glVertex3f(_framePos2.x*3, _framePos2.y*3, _framePos2.z);
		glTexCoord2f(1.0, 0.0); glVertex3f(_framePos3.x*3, _framePos3.y*3, _framePos3.z);
		glTexCoord2f(1.0, 1.0); glVertex3f(_framePos4.x*3, _framePos4.y*3, _framePos4.z);
		glEnd();
		
		_persp.disable();

		glBindTexture( GL_TEXTURE_2D, _texBackgroundF);
		_noColor.setFParameterTex(_texBackgroundF, "decal");
		_noColor.enable();

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(-_bw*3/2, _bh*3/2, 1);
		glTexCoord2f(0.0, 0.0); glVertex3f(-_bw*3/2, -_bh*3/2, 1);
		glTexCoord2f(1.0, 0.0); glVertex3f(_bw*3/2, -_bh*3/2, 1);
		glTexCoord2f(1.0, 1.0); glVertex3f(_bw*3/2, _bh*3/2, 1);
		glEnd();

		_noColor.disable();

		//glReadPixels(0, 0, _width, _height, GL_BGRA, GL_UNSIGNED_BYTE, _screen);
		//flipV(_screen, _width, _height, 4);
		//
		for(int i = 0; i < (*_intervals).size(); ++i)
		{
			_defaultImage[i] = -1;
			for(int j = 0; j < (*_intervals)[i].size(); ++j)
			{
				if(((*_intervals)[i][j].begin > *_currentFrame)||((*_intervals)[i][j].end < *_currentFrame))
					continue;
		
				_defaultImage[i] = j;
			}
		} 

		_noColor.setFParameterTex(_texBoard, "decal");
		_noColor.enable();
		// for each board
		for(int i = 0; i < (*_intervals).size(); ++i) 
		{
			if(_selectedImage[i] != -1) {

				glBindTexture( GL_TEXTURE_2D, _tex);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

				int minx = min((*_boards)[*_currentFrame][i].p1.x, min((*_boards)[*_currentFrame][i].p2.x, min((*_boards)[*_currentFrame][i].p3.x, (*_boards)[*_currentFrame][i].p4.x)));
		 		int miny = min((*_boards)[*_currentFrame][i].p1.y, min((*_boards)[*_currentFrame][i].p2.y, min((*_boards)[*_currentFrame][i].p3.y, (*_boards)[*_currentFrame][i].p4.y)));
		 		int maxx = max((*_boards)[*_currentFrame][i].p1.x, max((*_boards)[*_currentFrame][i].p2.x, max((*_boards)[*_currentFrame][i].p3.x, (*_boards)[*_currentFrame][i].p4.x)));
		 		int maxy = max((*_boards)[*_currentFrame][i].p1.y, max((*_boards)[*_currentFrame][i].p2.y, max((*_boards)[*_currentFrame][i].p3.y, (*_boards)[*_currentFrame][i].p4.y)));

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (*_intervals)[i][0].image.width(), (*_intervals)[i][0].image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, (*_intervals)[i][0].image.bits());
		 
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 1.0); glVertex3f(minx, maxy, 1);
				glTexCoord2f(0.0, 0.0); glVertex3f(minx, miny, 1);
				glTexCoord2f(1.0, 0.0); glVertex3f(maxx, miny, 1);
				glTexCoord2f(1.0, 1.0); glVertex3f(maxx, maxy, 1);
				glEnd();
			}
		}
		_noColor.disable();

		glDisable(GL_TEXTURE_2D);
	
		glFlush();
	}
}

void GLWidget::resizeGL(int w, int h)
{
	_width = w;
	_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-w/2, w/2, h/2, -h/2, 1000, -1000);
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
	RX::vec2 mouse = RX::vec2((ev->pos()).x()-_width/2, (ev->pos()).y()-_height/2);
	
	for(int i = 0; i < (*_boards)[*_currentFrame].size(); ++i)
	{
		if((pointInsideTriangle((*_boards)[*_currentFrame][i].p1, (*_boards)[*_currentFrame][i].p2, (*_boards)[*_currentFrame][i].p3, mouse))||
		   (pointInsideTriangle((*_boards)[*_currentFrame][i].p1, (*_boards)[*_currentFrame][i].p3, (*_boards)[*_currentFrame][i].p4, mouse)))
		{
			++_selectedImage[i];
			if(_selectedImage[i] == _defaultImage[i])
			{
				++_selectedImage[i];
			}
			if(_selectedImage[i] >= (*_intervals)[i].size())
			{
				_selectedImage[i] = -1;
			}
		}
		
	}
}

void GLWidget::setHomographies(vector<RX::mat3> *homographies)
{
	_homographies = homographies; 
	for(int i = 0; i < (*_homographies).size(); ++i)
	{
		RX::vec3 c1, c2, c3, c4;
		RX::vec2 c1_2d, c2_2d, c3_2d, c4_2d;
		_centroids.push_back((c1_2d+c2_2d+c3_2d+c4_2d)/4.0);
	}
}

void flipV(unsigned char *image, int w, int h, int bpp)
{
	unsigned char *flip = new unsigned char[w*h*bpp];
	for(int i = 0; i < h; ++i) {
		memcpy((void*)(flip + i*w*bpp), 
			   (void*)(image + (h - i - 1)*w*bpp), w*bpp);
	}
	memcpy((void*)image, flip, w*h*bpp);
	delete[] flip;
}
