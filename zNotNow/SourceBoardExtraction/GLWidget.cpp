#include <RX/CallMatlab.h>
#include <RX/SoftwareRenderer.h>
#include "Define.h"
#include "GLWidget.h"

GLWidget::GLWidget(QWidget* parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _frame(NULL), screen(NULL), quality(NULL)
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.setInterval(15);
	_timer.start();

}

GLWidget::~GLWidget()
{
	if(screen != NULL)
		delete[] screen;
}

void GLWidget::setIntervals()
{
	int h = 288;
	int w = 512;

	vector<bool> isIntersecting;
	for(int i = 0; i < (*_boards)[0].size(); ++i) {
		_intervals.push_back(vector<Interval>());
		isIntersecting.push_back(true);
	}
	
	for(int i = 0; i < (*_boards)[0].size(); ++i)
	{
		for(int j = 0; j < (*_pbboxes).size(); ++j) 
		{
			RX::vec2 pp1 = (*_pbboxes)[j].p[0];
			RX::vec2 pp2 = (*_pbboxes)[j].p[1];
			RX::vec2 pp3 = (*_pbboxes)[j].p[2];
			RX::vec2 pp4 = (*_pbboxes)[j].p[3];

			if(!BBox::intersecting(pp1, pp2, pp3, pp4, (*_boards)[j][i]._p1, (*_boards)[j][i]._p2, (*_boards)[j][i]._p3, (*_boards)[j][i]._p4))
			{
				if(!isIntersecting[i])
					_intervals[i][_intervals[i].size()-1].end += 1;
				else {
					isIntersecting[i] = false;
					Interval interval;
					interval.begin = interval.end = j;
					_intervals[i].push_back(interval);
				}
			}
			else 
				isIntersecting[i] = true;
		}
	}
}


void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	_hom.setIdentity();
}

void GLWidget::paintGL()
{
	static int lastFrameShown = -1;
	static bool first = true;

	if (_frame && (*_currentFrame != lastFrameShown)) 
	{
		//// IF LESS SPACE FOR MOVEMENT THAN BOARDS
		//while(_currMovements.size() < (*_boards).size())
		//	_currMovements.push_back(Movement());
		//
		//// UPDATE MOVEMENT
		//for(int i = 0; i < (*_currMoving).size(); ++i)
		//	(*_currMoving)[i] = false;
		//_isMoving = false;
		//
		//for(int i = 0; i < (*_movements).size(); ++i) {
		//	Movement m = (*_movements)[i];
		//	if((m.start <= *_currentFrame)&&(m.end >= *_currentFrame)) {
		//		(*_currMoving)[m.board] = true;
		//		_currMovements[m.board] = m;
		//		_isMoving = true;
		//	}
		//}

		lastFrameShown = *_currentFrame;

		int h = _frame->height();
		int w = _frame->width();

		double tx1, tx2, tx3, tx4;
		double ty1, ty2, ty3, ty4;
		double bx2, by2, bw2, bh2;
		
		_hom = (*_homographies)[lastFrameShown];
		_pbbox = (*_pbboxes)[lastFrameShown];

		glDisable(GL_TEXTURE_2D);

		if(lastFrameShown != 0) {
			_oldFramePos1 = (*_homographies)[lastFrameShown-1] * RX::vec3(-w/2, h/2, 1);
			_oldFramePos2 = (*_homographies)[lastFrameShown-1] * RX::vec3(-w/2, -h/2, 1);
			_oldFramePos3 = (*_homographies)[lastFrameShown-1] * RX::vec3(w/2, -h/2, 1);
			_oldFramePos4 = (*_homographies)[lastFrameShown-1] * RX::vec3(w/2, h/2, 1);
		}
		else {
			_oldFramePos1 = RX::vec3(-w/2, h/2, 1);
			_oldFramePos2 = RX::vec3(-w/2, -h/2, 1);
			_oldFramePos3 = RX::vec3(w/2, -h/2, 1);
			_oldFramePos4 = RX::vec3(w/2, h/2, 1);
		}

		_framePos1 = _hom * RX::vec3(-w/2, h/2, 1);
		_framePos2 = _hom * RX::vec3(-w/2, -h/2, 1);
		_framePos3 = _hom * RX::vec3(w/2, -h/2, 1);
		_framePos4 = _hom * RX::vec3(w/2, h/2, 1);

		RX::vec2 framePos12d = RX::vec2(_framePos1.x/_framePos1.z, _framePos1.y/_framePos1.z);
		RX::vec2 framePos22d = RX::vec2(_framePos2.x/_framePos2.z, _framePos2.y/_framePos2.z);
		RX::vec2 framePos32d = RX::vec2(_framePos3.x/_framePos3.z, _framePos3.y/_framePos3.z);
		RX::vec2 framePos42d = RX::vec2(_framePos4.x/_framePos4.z, _framePos4.y/_framePos4.z);

		RX::vec2 pp1 = _pbbox.p[0];
		RX::vec2 pp2 = _pbbox.p[1];
		RX::vec2 pp3 = _pbbox.p[2];
		RX::vec2 pp4 = _pbbox.p[3];

		vector<int> saveMe;
		for(int i = 0; i < _intervals.size(); ++i)
		{
			saveMe.push_back(-1);
			for(int j = 0; j < _intervals[i].size(); ++j)
			{
				if((_intervals[i][j].begin <= lastFrameShown)&&(_intervals[i][j].end >= lastFrameShown))
				{
					double newQuality = 0;
					
					//SoftwareRenderer::getCost((*_boards)[lastFrameShown][i].p1, (*_boards)[lastFrameShown][i].p2, (*_boards)[lastFrameShown][i].p3, (*_boards)[lastFrameShown][i].p4,
					//												_framePos1, _framePos2, _framePos3, _framePos4,
					//											   _oldFramePos1, _oldFramePos2, _oldFramePos3, _oldFramePos4,
					//											   _width, _height);

					if(newQuality < _intervals[i][j].quality) {
						_intervals[i][j].quality = newQuality;
						char buf[20];
						sprintf(buf, "board_%d_%d.png", i, j);
						_intervals[i][j].image = string(buf);
						_intervals[i][j].id = lastFrameShown;
						saveMe[i] = j;
					}
				}
			}
		}
				
		flipV(screen, _width, _height, 4);
		RX::SoftwareRenderer::render(*_frame, _framePos1, _framePos2, _framePos3, _framePos4, screen, _width, _height, 4, 4);
		flipV(screen, _width, _height, 4);
		glDrawPixels(_width, _height, GL_BGRA, GL_UNSIGNED_BYTE, screen);

		for(int i = 0; i < _intervals.size(); ++i) {
			if(saveMe[i] != -1) {

				int minx = min((*_boards)[lastFrameShown][i]._p1.x, min((*_boards)[lastFrameShown][i]._p2.x, min((*_boards)[lastFrameShown][i]._p3.x, (*_boards)[lastFrameShown][i]._p4.x)));
				int miny = min((*_boards)[lastFrameShown][i]._p1.y, min((*_boards)[lastFrameShown][i]._p2.y, min((*_boards)[lastFrameShown][i]._p3.y, (*_boards)[lastFrameShown][i]._p4.y)));
				int maxx = max((*_boards)[lastFrameShown][i]._p1.x, max((*_boards)[lastFrameShown][i]._p2.x, max((*_boards)[lastFrameShown][i]._p3.x, (*_boards)[lastFrameShown][i]._p4.x)));
				int maxy = max((*_boards)[lastFrameShown][i]._p1.y, max((*_boards)[lastFrameShown][i]._p2.y, max((*_boards)[lastFrameShown][i]._p3.y, (*_boards)[lastFrameShown][i]._p4.y)));

				int boardw = maxx-minx+1;
				int boardh = maxy-miny+1;

				unsigned char *buffer = new unsigned char[boardw*boardh*4];
				glReadPixels(minx+_width/2, _height/2-maxy, boardw, boardh, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
				flipV(buffer, boardw, boardh, 4);
				makeBlackToGray(buffer, boardw, boardh, 4);

				for(int k = 0; k < boardh; ++k) {
					for(int j = 0; j < boardw; ++j) {
						if((!pointInsideTriangle((*_boards)[lastFrameShown][i]._p1, (*_boards)[lastFrameShown][i]._p2, (*_boards)[lastFrameShown][i]._p3, RX::vec2(j+minx, k+miny)))&&
						   (!pointInsideTriangle((*_boards)[lastFrameShown][i]._p1, (*_boards)[lastFrameShown][i]._p3, (*_boards)[lastFrameShown][i]._p4, RX::vec2(j+minx, k+miny))))
						{
							buffer[(k*boardw + j)*4] = 0;
							buffer[(k*boardw + j)*4 + 1] = 255;
							buffer[(k*boardw + j)*4 + 2] = 0;
						}
					}
				}

				QImage img(buffer, boardw, boardh, QImage::Format_RGB32);
				img.save(_intervals[i][saveMe[i]].image.c_str());
				delete[] buffer;
			}
		}

		for(int i = 0; i < (*_boards)[0].size(); ++i) 
		{
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_LINE_LOOP);
			glVertex3f((*_boards)[lastFrameShown][i]._p1.x, (*_boards)[lastFrameShown][i]._p1.y, 1);
			glVertex3f((*_boards)[lastFrameShown][i]._p2.x, (*_boards)[lastFrameShown][i]._p2.y, 1);
			glVertex3f((*_boards)[lastFrameShown][i]._p3.x, (*_boards)[lastFrameShown][i]._p3.y, 1);
			glVertex3f((*_boards)[lastFrameShown][i]._p4.x, (*_boards)[lastFrameShown][i]._p4.y, 1);
			glEnd();
		}

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_LOOP);
		glVertex3f(pp1.x, pp1.y, 1);
		glVertex3f(pp2.x, pp2.y, 1);
		glVertex3f(pp3.x, pp3.y, 1);
		glVertex3f(pp4.x, pp4.y, 1);
		glEnd();

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

	if(screen != NULL)
		delete[] screen;
	screen = new unsigned char[_width*_height*4];
	memset(screen, 0, _width*_height*4);

	if(quality != NULL)
		delete[] quality;
	quality = new unsigned int[_width*_height];
	for(int i = 0; i < _width*_height; ++i)
		quality[i] = 9999;
}

void GLWidget::saveIntervals(string filename)
{
	ofstream file(filename.c_str());

	file << _intervals.size() << endl;
	for(int i = 0; i < _intervals.size(); ++i) {
		file << _intervals[i].size() << endl;
		for(int j = 0; j < _intervals[i].size(); ++j) {
			file << _intervals[i][j].begin << " " << _intervals[i][j].end <<endl;
		}
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

void makeBlackToGray(unsigned char *image, int w, int h, int bpp)
{
	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			if((image[(i*w +j)*4] == 0)&&(image[(i*w +j)*4+1] == 0)&&(image[(i*w +j)*4+2] == 0)) 
			{
				image[(i*w +j)*4] = 10;
				image[(i*w +j)*4+1] = 10;
				image[(i*w +j)*4+2] = 10;
			}
		}
	}
}
