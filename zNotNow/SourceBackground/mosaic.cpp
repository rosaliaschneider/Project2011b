#include <functional>
#include <fstream>
#include <iostream>
#include <queue>
#include <cassert>
#include <cmath>
#include <ctime>

#include <RX/vec2.h>

#include <QtConcurrentMap>
#include <QStringList>
#include <QVector>
#include <QCoreApplication>
#include <QFile>
#include <QDirIterator>
#include <QImage>
#include <QPainter>

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

int main(int argc, char **argv) 
{
	std::ifstream gcInfo("_GraphcutInfo.txt");

    // load images to be mosaiced
	int n;
	std::string inputs[100], ms[100];
	RX::vec2 points[100][4], mins[100];

	gcInfo >> n;
	for(int i = 0; i < n; ++i) {
		int id;
		gcInfo >> id;
		
		std::string str("mask");
		char buf[10];
		sprintf(buf, "%d", i);
		str += buf;
		str += ".png";
		inputs[i] = str;

		std::string str2("mask");
		sprintf(buf, "%d", i);
		str2 += buf;
		str2 += ".png";
		ms[i] = str2;
		
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
		img.load(inputs[i].c_str());
		mask.load(ms[i].c_str());
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

	createMosaic(images, masks, mins, n, width, height, "bg");
}
