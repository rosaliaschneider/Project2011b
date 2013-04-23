#include <QPrinter>
#include <QPainter>
#include "Globals.h"
#include "Map.h"

Map::Map()
: _isSet(false)
{
}

Map::~Map()
{
}

// Sets
void Map::addImage(int img)
{
	_availableMaps.push_back(img);
}

QImage Map::image(int frameNumber)
{
	if(frameNumber == -1 || _isSet)
		return _map;

	int index;
	for(int i = 0; i < _availableMaps.size(); ++i)
		if(_availableMaps[i] <= frameNumber)
			index = _availableMaps[i];
		else
			break;
	
	char buffer[100];
	sprintf(buffer, "/Maps/map_%d.png", index);
	_map = QImage(folder+buffer);
	return _map;
}

void Map::createMap(int numPages, int pageWidth, int pageHeight)
{
	int width =  pageWidth*2;
	int height = (numPages/2 + numPages%2)*pageHeight;

	_map = QImage(width, height, QImage::Format_RGB32);
	_isSet = true;
}

void Map::setPage(int index, QImage &page)
{
	unsigned char *data = _map.bits();
	unsigned char *pageData = page.bits();

	int iPos = index/2;
	int jPos = index%2;
	int iStart = iPos*_map.width()*page.height()*4;
	int jStart = jPos*page.width()*4;

	for(int i = 0; i < page.height(); ++i)
	{
		for(int j = 0; j < page.width(); ++j)
		{
			data[(i*_map.width()+j)*4 + iStart + jStart]     = pageData[(i*page.width()+j)*4];
			data[(i*_map.width()+j)*4 + iStart + jStart + 1] = pageData[(i*page.width()+j)*4 + 1];
			data[(i*_map.width()+j)*4 + iStart + jStart + 2] = pageData[(i*page.width()+j)*4 + 2];
		}
	}
}

void Map::exportNotes(std::string filename)
{
	QPrinter printer(QPrinter::ScreenResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setColorMode(QPrinter::Color);
	printer.setOutputFileName(filename.c_str());
	printer.setPageSize(QPrinter::A0);
	printer.setOrientation(QPrinter::Portrait);

	// get map dimensions
	int x, y, w, h;

	x = _map.width();
	y = _map.height();

	int bpp = _map.depth()/8;
	for(int i = 0; i < _map.height(); ++i)
	{
		for(int j = 0; j < _map.width(); ++j)
		{
			unsigned char r = _map.bits()[(i*_map.width() + j)*bpp];
			unsigned char g = _map.bits()[(i*_map.width() + j)*bpp + 1];
			unsigned char b = _map.bits()[(i*_map.width() + j)*bpp + 2];

			if(r != 255 || g != 255 || b != 255)
			{
				if(y > i) y = i;
				if(x > j) x = j;
			}
		}
	}
	w = _map.width() - x;
	h = _map.height() - y;
	
	// copy map
	QImage cleanMap = _map.copy(x, y, w, h);
	
	// print map to pdf
	QPainter painter(&printer);
	painter.drawImage(QPoint(0, 0), cleanMap);
	painter.end();

	// print map to png
	filename[filename.size()-1] = 'g';
	filename[filename.size()-2] = 'n';
	cleanMap.save(filename.c_str());
}
