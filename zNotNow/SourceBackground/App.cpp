#include <RX/mat3.h>
#include <RX/SoftwareRenderer.h>
#include <QtGui/QApplication>
#include <QFileDialog>
#include <QTimer>
#include "BBox.h"
#include "Coverage.h"
#include "VideoDecoder.h"

QString folder;
vector<RX::mat3> _homographies;
QImage _frame;
int _currentFrame, _numFrames;
VideoDecoder _decoder;

vec3 _framePos1, _framePos2, _framePos3, _framePos4;
vec3 _oldFramePos1, _oldFramePos2, _oldFramePos3, _oldFramePos4;
int _width, _height, _fw, _fh;

unsigned char *_mask;
int _mwidth, _mheight;
int _mxmin, _mymin;
int _mxmax, _mymax;

std::string _folder;
unsigned char *screen, *quality;
vector<BestFrame> _bestFrames;

#include "imageMatch.h"
#include "mosaicGraph.h"
using namespace std;

std::ofstream logFile;

void createMosaic(vector<QImage> images, vector<QImage> masks, RX::vec2 mins[10], int n, int w, int h, string outputFile)
{
	MosaicGraph graph(w, h);

	for(int i = 0; i < n; ++i)
	{
		int *buffer = toYIQ(images[i].bits(), images[i].width(), images[i].height());
		graph.setImageUsage(masks[i]);

		if(i == 0)
			graph.createGraphFromSourceFirst(buffer, mins[i].x, mins[i].y, images[i].width(), images[i].height());
		else
			graph.createGraphFromSource(buffer, mins[i].x, mins[i].y, images[i].width(), images[i].height());

		graph.saveImageUsage((i+1));
		graph.solve();
		graph.update();
		graph.saveImageUsage(-(i+1));
		free(buffer);

		uchar *bufferOut = toRGB(graph.image(), graph.width(), graph.height());
		QImage image(bufferOut, graph.width(), graph.height(), QImage::Format_RGB32);
		char num[10];
		sprintf(num, "%d.png", i);
		image.save((outputFile+num).c_str());
		free(bufferOut);
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

void createMask()
{
	int xmin = 99999, xmax = -99999, ymin = 99999, ymax = -99999;
	for(int i = 0; i < _homographies.size(); i += 100) {
		vec3 framePos1 = _homographies[i] * vec3(-_fw/2, _fh/2, 1);
		vec3 framePos2 = _homographies[i] * vec3(-_fw/2, -_fh/2, 1);
		vec3 framePos3 = _homographies[i] * vec3(_fw/2, -_fh/2, 1);
		vec3 framePos4 = _homographies[i] * vec3(_fw/2, _fh/2, 1);

		framePos1 = vec3(framePos1.x/framePos1.z, framePos1.y/framePos1.z, 1);
		framePos2 =	vec3(framePos2.x/framePos2.z, framePos2.y/framePos2.z, 1);
		framePos3 =	vec3(framePos3.x/framePos3.z, framePos3.y/framePos3.z, 1);
		framePos4 =	vec3(framePos4.x/framePos4.z, framePos4.y/framePos4.z, 1);

		if(framePos1.x < xmin) xmin = framePos1.x;
		if(framePos2.x < xmin) xmin = framePos2.x;
		if(framePos3.x < xmin) xmin = framePos3.x;
		if(framePos4.x < xmin) xmin = framePos4.x;

		if(framePos1.x > xmax) xmax = framePos1.x;
		if(framePos2.x > xmax) xmax = framePos2.x;
		if(framePos3.x > xmax) xmax = framePos3.x;
		if(framePos4.x > xmax) xmax = framePos4.x;

		if(framePos1.y < ymin) ymin = framePos1.y;
		if(framePos2.y < ymin) ymin = framePos2.y;
		if(framePos3.y < ymin) ymin = framePos3.y;
		if(framePos4.y < ymin) ymin = framePos4.y;

		if(framePos1.y > ymax) ymax = framePos1.y;
		if(framePos2.y > ymax) ymax = framePos2.y;
		if(framePos3.y > ymax) ymax = framePos3.y;
		if(framePos4.y > ymax) ymax = framePos4.y;
	}

	_mxmin = xmin;
	_mymin = ymin;
	_mxmax = xmax;
	_mymax = ymax;
	_mwidth = xmax-xmin+1;
	_mheight = ymax-ymin+1;

	if(_mask)
		delete[] _mask;
	_mask = new unsigned char[_mwidth*_mheight*4];
	memset(_mask, 128, _mwidth*_mheight*4);
	
	for(int i = 0; i < _homographies.size(); i += 100) {
		vec3 framePos1 = _homographies[i] * vec3(-_fw/2, _fh/2, 1);
		vec3 framePos2 = _homographies[i] * vec3(-_fw/2, -_fh/2, 1);
		vec3 framePos3 = _homographies[i] * vec3(_fw/2, -_fh/2, 1);
		vec3 framePos4 = _homographies[i] * vec3(_fw/2, _fh/2, 1);

		framePos1 = vec3(framePos1.x/framePos1.z, framePos1.y/framePos1.z, 1);
		framePos2 =	vec3(framePos2.x/framePos2.z, framePos2.y/framePos2.z, 1);
		framePos3 =	vec3(framePos3.x/framePos3.z, framePos3.y/framePos3.z, 1);
		framePos4 =	vec3(framePos4.x/framePos4.z, framePos4.y/framePos4.z, 1);

		framePos1.x = framePos1.x - xmin - _mwidth/2;
		framePos2.x = framePos2.x - xmin - _mwidth/2;
		framePos3.x = framePos3.x - xmin - _mwidth/2;
		framePos4.x = framePos4.x - xmin - _mwidth/2;

		framePos1.y = framePos1.y - ymin - _mheight/2;
		framePos2.y = framePos2.y - ymin - _mheight/2;
		framePos3.y = framePos3.y - ymin - _mheight/2;
		framePos4.y = framePos4.y - ymin - _mheight/2;

		SoftwareRenderer::render(vec3(255, 0, 255), framePos1, framePos2, framePos3, framePos4, _mask, _mwidth, _mheight, 4, 4);
	}
}

Coverage getBestFrame(bool ignoreBadFrames)
{
	vec3 oldFramePos1 = vec3(-_fw/2, _fh/2, 1);
	vec3 oldFramePos2 = vec3(-_fw/2, -_fh/2, 1);
	vec3 oldFramePos3 = vec3(_fw/2, -_fh/2, 1);
	vec3 oldFramePos4 = vec3(_fw/2, _fh/2, 1);
	
	vec3 framePos1 = vec3(-_fw/2, _fh/2, 1);
	vec3 framePos2 = vec3(-_fw/2, -_fh/2, 1);
	vec3 framePos3 = vec3(_fw/2, -_fh/2, 1);
	vec3 framePos4 = vec3(_fw/2, _fh/2, 1);

	unsigned char *mask = new unsigned char[_mwidth*_mheight*4];

	Coverage c;
	c.whites = SoftwareRenderer::countColor(vec3(255, 0, 255), vec3(0, 0, 1), vec3(0, _mheight, 1), vec3( _mwidth, _mheight, 1), vec3(_mwidth, 0, 1), _mask, _mwidth, _mheight, 4, 4);
	// greedy
	for(int i = 50; i < 100; ++i) 
	{
		framePos1 = _homographies[i] * vec3(-_fw/2, _fh/2, 1);
		framePos2 = _homographies[i] * vec3(-_fw/2, -_fh/2, 1);
		framePos3 = _homographies[i] * vec3(_fw/2, -_fh/2, 1);
		framePos4 = _homographies[i] * vec3(_fw/2, _fh/2, 1);

		framePos1 = vec3(framePos1.x/framePos1.z, framePos1.y/framePos1.z, 1);
		framePos2 =	vec3(framePos2.x/framePos2.z, framePos2.y/framePos2.z, 1);
		framePos3 =	vec3(framePos3.x/framePos3.z, framePos3.y/framePos3.z, 1);
		framePos4 =	vec3(framePos4.x/framePos4.z, framePos4.y/framePos4.z, 1);

		framePos1.x -= _mxmin;
		framePos2.x -= _mxmin;
		framePos3.x -= _mxmin;
		framePos4.x -= _mxmin;
				   
		framePos1.y -= _mymin;
		framePos2.y -= _mymin;
		framePos3.y -= _mymin;
		framePos4.y -= _mymin;

		char buf[50];
		sprintf(buf, "//Replace//frame_%d.png", i);
		QImage frame((_folder+buf).c_str());
		memcpy(mask, _mask, _mwidth*_mheight*4);

		//sprintf(buf, "mask%d_before.png", i);
		//QImage mask1(mask, _mwidth, _mheight, QImage::Format_RGB32);
		//mask1.save(buf);

		SoftwareRenderer::renderNoColorA(frame, vec3(255, 0, 255), framePos1, framePos2, framePos3, framePos4, mask, _mwidth, _mheight, 4, 4, false);

		//sprintf(buf, "mask%d_after.png", i);
		//QImage mask2(mask, _mwidth, _mheight, QImage::Format_RGB32);
		//mask2.save(buf);

		int whites = SoftwareRenderer::countColor(vec3(255, 0, 255), vec3(0, 0, 1), vec3(0, _mheight, 1), vec3( _mwidth, _mheight, 1), vec3(_mwidth, 0, 1), mask, _mwidth, _mheight, 4, 4);
		if(c.whites > whites) {
			c.whites = whites;
			c.id = i;
		}
		std::cout << i << std::endl;
		
	}
	delete[] mask;

	return c;
}

void getBestFrames(std::string filename)
{
 	int totalWhites = SoftwareRenderer::countColor(vec3(255, 0, 255), vec3(0, _mheight-1, 1), vec3(0, 0, 1), vec3(_mwidth-1, 0, 1), vec3(_mwidth-1, _mheight-1, 1), _mask, _mwidth, _mheight, 4, 4);
	
	int howManyWhites = totalWhites;

	// first good frames
	for(int i = 0; i < 10; ++i)
	{
 		Coverage c = getBestFrame(true);

		RX::vec3 framePos1 = _homographies[c.id] * vec3(-_fw/2, _fh/2, 1);
		RX::vec3 framePos2 = _homographies[c.id] * vec3(-_fw/2, -_fh/2, 1);
		RX::vec3 framePos3 = _homographies[c.id] * vec3(_fw/2, -_fh/2, 1);
		RX::vec3 framePos4 = _homographies[c.id] * vec3(_fw/2, _fh/2, 1);

		framePos1 = vec3(framePos1.x/framePos1.z - _mxmin - _mwidth/2, framePos1.y/framePos1.z - _mymin - _mheight/2, 1);
		framePos2 =	vec3(framePos2.x/framePos2.z - _mxmin - _mwidth/2, framePos2.y/framePos2.z - _mymin - _mheight/2, 1);
		framePos3 =	vec3(framePos3.x/framePos3.z - _mxmin - _mwidth/2, framePos3.y/framePos3.z - _mymin - _mheight/2, 1);
		framePos4 =	vec3(framePos4.x/framePos4.z - _mxmin - _mwidth/2, framePos4.y/framePos4.z - _mymin - _mheight/2, 1);

		char buf[20];
		sprintf(buf, "//Replace//frame_%d.png", c.id);
		QImage frame((_folder+buf).c_str());

		SoftwareRenderer::renderNoColorA(frame, vec3(255, 0, 255), framePos1, framePos2, framePos3, framePos4, _mask, _mwidth, _mheight, 4, 4);

		int newHowManyWhites = SoftwareRenderer::countColor(vec3(255, 0, 255), vec3(0, _mheight-1, 1), vec3(0, 0, 1), vec3(_mwidth-1, 0, 1), vec3(_mwidth-1, _mheight-1, 1), _mask, _mwidth, _mheight, 4, 4);
		if(howManyWhites == newHowManyWhites)
			break;

		_bestFrames.push_back(BestFrame(framePos1.xy(), framePos2.xy(), framePos3.xy(), framePos4.xy(), c.id));	
		howManyWhites = newHowManyWhites;
	}

	std::ofstream gcInfo(filename.c_str());
	gcInfo << _bestFrames.size() << endl;
	for(int i = 0; i < _bestFrames.size(); ++i)
	{
		gcInfo << _bestFrames[i].id << endl;
		gcInfo << _bestFrames[i].p1.x << " " << _bestFrames[i].p1.y << endl;
		gcInfo << _bestFrames[i].p2.x << " " << _bestFrames[i].p2.y << endl;
		gcInfo << _bestFrames[i].p3.x << " " << _bestFrames[i].p3.y << endl;
		gcInfo << _bestFrames[i].p4.x << " " << _bestFrames[i].p4.y << endl;
	}
}


void loadInfo(std::string filename)
{
	ifstream file(filename.c_str());
	file >> _numFrames;
}

void loadHomographies(std::string filename)
{
	ifstream file(filename.c_str());
	file >> _numFrames;
	for(int i = 0; i < _numFrames; ++i)
	{
		RX::mat3 m;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				double d;
				file >> d; 
				m.set(j, k, d);
			}
		}
		_homographies.push_back(m);
	}
}

//  Display next frame
bool next()
{
	static bool first = true;
	if(_decoder.getNextFrame(&_frame)) {
		++_currentFrame;

		if(first) {
			_fh = _frame.height();
			_fw = _frame.width();
			createMask();
			getBestFrames((folder+"/BgCandidates/_GraphcutInfo.txt").toStdString());
			first = false;

			_width = 2000;
			_height = 1500;
			screen = new unsigned char[_width*_height*4];
		}
	 
		bool important = false;
		for(int i = 0; i < _bestFrames.size(); ++i) {
			if(_bestFrames[i].id == _currentFrame)
				important = true;
		}

		if(!important)
			return true;
		
		_framePos1 = _homographies[_currentFrame] * vec3(-_fw/2, _fh/2, 1);
 		_framePos2 = _homographies[_currentFrame] * vec3(-_fw/2, -_fh/2, 1);
		_framePos3 = _homographies[_currentFrame] * vec3(_fw/2, -_fh/2, 1);
		_framePos4 = _homographies[_currentFrame] * vec3(_fw/2, _fh/2, 1);

		
		memset(screen, 0, _width*_height*3);
		flipV(screen, _width, _height, 3);
		SoftwareRenderer::render(RX::vec3(255, 0, 255), _framePos1, _framePos2, _framePos3, _framePos4, screen, _width, _height, 3);
		SoftwareRenderer::renderNoColorAB(_frame, RX::vec3(255, 0, 255), RX::vec3(0, 255, 0), _framePos1, _framePos2, _framePos3, _framePos4, screen, _width, _height, 4, 3);

		for(int i = 0; i < _width*_height; ++i) {
			char swap = screen[i*3];
			screen[i*3] = screen[i*3+2];
			screen[i*3+2] = swap;
		}
		
		char buf[50];
		sprintf(buf, "/BgCandidates/mask%d.png", _currentFrame);
		QImage maskImg(screen, _width, _height, QImage::Format_RGB888);
		maskImg.save(folder+buf);

		return true;
	}
	else
		return false;
}

void load()
{
	// Prompt a video to load
	folder = QFileDialog::getExistingDirectory(NULL, "Load Video");
	if(!folder.isNull())
	{
		loadInfo((folder+"\\_LocalBoards.txt").toStdString());
		loadHomographies((folder+"\\_LocalHoms.txt").toStdString());

		_decoder.load(folder.toStdString(), _numFrames);

		_currentFrame = -1;
		_folder = folder.toStdString();

		for(int i = 1; i < _homographies.size(); ++i)
			_homographies[i] = _homographies[i] * _homographies[i-1];

		//ui->widget->setHomographies(&_homographies);

		next();
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    //load();
	//while(next());

	_bestFrames.push_back(BestFrame(RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), 53));
	_bestFrames.push_back(BestFrame(RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), 86));
	_bestFrames.push_back(BestFrame(RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), 50));
	_bestFrames.push_back(BestFrame(RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), RX::vec2(0,0), 52));

	folder = QFileDialog::getExistingDirectory(NULL, "Load Video");
	std::ifstream gcInfo((folder+"/BgCandidates/_GraphcutInfo.txt").toStdString());

    // load images to be mosaiced
	int n;
	std::string inputs[100], ms[100];
	RX::vec2 points[100][4], mins[100];

	gcInfo >> n;
	for(int i = 0; i < n; ++i) {
		int id;
		gcInfo >> id;

		char buf[50];
		sprintf(buf, "/BgCandidates/mask%d.png", _bestFrames[i].id);
		inputs[i] = buf;
		ms[i] = buf;
		
		gcInfo >> points[i][0].x >> points[i][0].y;
		gcInfo >> points[i][1].x >> points[i][1].y;
		gcInfo >> points[i][2].x >> points[i][2].y;
		gcInfo >> points[i][3].x >> points[i][3].y;

		points[i][0].x = points[i][0].y = 0;
		points[i][1].x = points[i][1].y = 0;
		points[i][2].x = points[i][2].y = 0;
		points[i][3].x = points[i][3].y = 0;
	}
	vector<QImage> images, masks;
	int minX = 9999, minY = 9999, maxX = -9999, maxY = -9999;

    for(int i = 0; i < n; ++i) {
		QImage img, mask;
		img.load((folder.toStdString()+inputs[i]).c_str());
		mask.load((folder.toStdString()+ms[i]).c_str());
		mins[i].x = min(points[i][0].x, min(points[i][1].x, min(points[i][2].x, points[i][3].x)));
		mins[i].y = min(points[i][0].y, min(points[i][1].y, min(points[i][2].y, points[i][3].y)));

		if(mins[i].x < minX) minX = mins[i].x;
		if(mins[i].y < minY) minY = mins[i].y;
		if(mins[i].x+img.width() > maxX) maxX = mins[i].x+img.width();
		if(mins[i].y+img.height() > maxY) maxY = mins[i].y+img.height();

		images.push_back(img);
		masks.push_back(mask);
	}
	int width = maxX-minX;
	int height = maxY-minY;

	createMosaic(images, masks, mins, n, width, height, folder.toStdString()+"/BgCandidates/bg");
}
