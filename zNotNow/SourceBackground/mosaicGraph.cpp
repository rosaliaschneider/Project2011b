#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include "mosaicGraph.h"
#include "imageMatch.h"
using namespace std;

ofstream out;

MosaicGraph::MosaicGraph(int w, int h)
{
	_width = w;
	_height = h;
	_pixels = _width*_height;
	_nodes = NULL;
	g = NULL;

	_image = (int *)malloc(_pixels*4*sizeof(int));
	memset(_image, 0, _pixels*4*sizeof(int));

	_imageUsage = (uchar *)malloc(_pixels);
	memset(_imageUsage, 0, _pixels);

	_oldCost = (int *)malloc(_pixels*4*sizeof(int));
	memset(_oldCost, 0, _pixels*4*sizeof(int));

	_oldGrad = (int *)malloc(_pixels*2*sizeof(int));
	memset(_oldGrad, 0, _pixels*2*sizeof(int));

	out.open("output.txt");

}
MosaicGraph::~MosaicGraph()
{
	for(int i = 0; i < _seamNodes.size(); ++i)
		delete[] _seamNodes[i];
	_seamNodes.clear();

	free(_nodes);
	free(_image);
	free(_imageUsage);
	free(_oldCost);
	free(_oldGrad);

	out.close();
}

void MosaicGraph::setImageUsage(QImage mask)
{
	_oldMask = _mask;
	_mask = mask;
}

void MosaicGraph::saveImageUsage(int index)
{
	uchar *bufferOut = (uchar *)malloc(_pixels*4);

	for(int i = 0; i < _height; ++i)
	{
		for(int j = 0; j < _width; ++j)
		{
			if(_imageUsage[i*_width + j] == A)
			{
				bufferOut[(i*_width + j)*4] = 0;
				bufferOut[(i*_width + j)*4+1] = 0;
				bufferOut[(i*_width + j)*4+2] = 0;
			}
			if(_imageUsage[i*_width + j] == B)
			{
				bufferOut[(i*_width + j)*4] = 255;
				bufferOut[(i*_width + j)*4+1] = 255;
				bufferOut[(i*_width + j)*4+2] = 255;
			}
			if(_imageUsage[i*_width + j] == GREEN)
			{
				bufferOut[(i*_width + j)*4] = 0;
				bufferOut[(i*_width + j)*4+1] = 255;
				bufferOut[(i*_width + j)*4+2] = 0;
			}
			if(_imageUsage[i*_width + j] == PINK)
			{
				bufferOut[(i*_width + j)*4] = 255;
				bufferOut[(i*_width + j)*4+1] = 0;
				bufferOut[(i*_width + j)*4+2] = 255;
			}
			if(_imageUsage[i*_width + j] == NEW)
			{
				bufferOut[(i*_width + j)*4] = 255;
				bufferOut[(i*_width + j)*4+1] = 0;
				bufferOut[(i*_width + j)*4+2] = 0;
			}
		}
	}

	QImage image(bufferOut, _width, _height, QImage::Format_RGB32);
	char num[20];
	sprintf(num, "imgUsage%d.png", index);
	image.save(num);
	free(bufferOut);
}

void MosaicGraph::createGraphFromSourceFirst(int *source, int x, int y, int sW, int sH)
{
	_source = source;
	_sX = x;
	_sY = y;
	_sW = sW;
	_sH = sH;

	// Update image usage mask
	for(int i = _sY; i < _sY+_sH; ++i) {
		for(int j = _sX; j < _sX+_sW; ++j) {
			// green
			if((_mask.bits()[(i*_sW + j)*4] == 0)&&(_mask.bits()[(i*_sW + j)*4+1] == 255)&&(_mask.bits()[(i*_sW + j)*4+2] == 0)) {
				_imageUsage[i*_width + j] = GREEN;
				continue;
			}
			// pink
			if((_mask.bits()[(i*_sW + j)*4] == 255)&&(_mask.bits()[(i*_sW + j)*4+1] == 0)&&(_mask.bits()[(i*_sW + j)*4+2] == 255)) {
				_imageUsage[i*_width + j] = PINK;
				continue;
			}
			_imageUsage[i*_width + j] = B;
		}
	}

	if(_nodes != NULL)
		free(_nodes);

	_nodes = (int*)(malloc(_sW*_sH*6*sizeof(int)));
	nodes = _sW*_sH;
	for(int i = 0; i < _sW*_sH*6; ++i)
		_nodes[i] = -1;


	for(int i = 0; i < _sH; ++i) {
		for(int j = 0; j < _sW; ++j) {

			int sourcePix = i*_sW + j;
			int targetPix = targetP(_sX, _sY, _sW, sourcePix);

			if(_imageUsage[targetPix] == B) {
				_nodes[(i*_sW + j)*6 + OBJ] = 0;
				_nodes[(i*_sW + j)*6 + BKG] = 65535;
			}
			if(_imageUsage[targetPix] == NEW) {
				
				// cost between points
				bool a, b;
				vector<int> valid = getValues(targetPix, &a, &b);

				_nodes[(i*_sW + j)*6 + OBJ] = 0;
				_nodes[(i*_sW + j)*6 + BKG] = 0;

				if(a)
					_nodes[(i*_sW + j)*6 + OBJ] = 65535;
				if(b)
					_nodes[(i*_sW + j)*6 + BKG] = 65535;

				for(int k = 0; k < 4; ++k) {

					if(valid[k] == NEW) {

						int neighbor = getNeighbor(sourcePix, k, _sW);
						int neighborT = getNeighbor(targetPix, k, _width);

						_nodes[(i*_sW + j)*6 + k] = getOldCost(targetPix, neighborT);

						int *seamNode = new int[6];

						seamNode[OBJ] = 0;
						seamNode[BKG] = getOldCost(targetPix, neighborT);
						
						seamNode[0] = i*_sW + j;
						seamNode[1] = neighbor;

						seamNode[2] = getNewCostA(source, targetPix, neighborT, _sX, _sY, _sW);
						seamNode[3] = getNewCostB(source, targetPix, neighborT, _sX, _sY, _sW);

						_seamNodes.push_back(seamNode);
					}
				}
			}
		}
	}
}

void MosaicGraph::createGraphFromSource(int *source, int x, int y, int sW, int sH)
{
	_source = source;
	_sX = x;
	_sY = y;
	_sW = sW;
	_sH = sH;

	// Update image usage mask
	for(int i = _sY; i < _sY+_sH; ++i) {
		for(int j = _sX; j < _sX+_sW; ++j) {
			
			// old is green
			if(_imageUsage[i*_width + j] == GREEN)
			{
				// new is green - do nothing
				if((_mask.bits()[(i*_sW + j)*4] == 0)&&(_mask.bits()[(i*_sW + j)*4+1] == 255)&&(_mask.bits()[(i*_sW + j)*4+2] == 0))
					continue;
				// new is pink
				if((_mask.bits()[(i*_sW + j)*4] == 255)&&(_mask.bits()[(i*_sW + j)*4+1] == 0)||(_mask.bits()[(i*_sW + j)*4+2] == 255)) {
					_imageUsage[i*_width + j] = PINK;
					continue;
				}
				// new is valid
				_imageUsage[i*_width + j] = B;
				continue;
			}
			// old is pink
			if(_imageUsage[i*_width + j] == PINK)
			{
				// new is green - do nothing
				if((_mask.bits()[(i*_sW + j)*4] == 0)&&(_mask.bits()[(i*_sW + j)*4+1] == 255)&&(_mask.bits()[(i*_sW + j)*4+2] == 0))
					continue;
				// new is pink - do nothing
				if((_mask.bits()[(i*_sW + j)*4] == 255)&&(_mask.bits()[(i*_sW + j)*4+1] == 0)&&(_mask.bits()[(i*_sW + j)*4+2] == 255))
					continue;
				// new is valid
				_imageUsage[i*_width + j] = B;
				continue;
			}
			// old is valid
			if((_imageUsage[i*_width + j] != PINK)&&(_imageUsage[i*_width + j] != GREEN))
			{
				// new is green
				if((_mask.bits()[(i*_sW + j)*4] == 0)&&(_mask.bits()[(i*_sW + j)*4+1] == 255)&&(_mask.bits()[(i*_sW + j)*4+2] == 0)) {
					_imageUsage[i*_width + j] = A;
					continue;
				}
				// new is pink
				if((_mask.bits()[(i*_sW + j)*4] == 255)&&(_mask.bits()[(i*_sW + j)*4+1] == 0)&&(_mask.bits()[(i*_sW + j)*4+2] == 255)) {
					_imageUsage[i*_width + j] = A;
					continue;
				}
				// new is valid
				_imageUsage[i*_width + j] = NEW;
			}
		}
	}

	if(_nodes != NULL)
		free(_nodes);

	_nodes = (int*)(malloc(_sW*_sH*6*sizeof(int)));
	nodes = _sW*_sH;
	for(int i = 0; i < _sW*_sH*6; ++i)
		_nodes[i] = -1;


	for(int i = 0; i < _sH; ++i) {
		for(int j = 0; j < _sW; ++j) {

			int sourcePix = i*_sW + j;
			int targetPix = targetP(_sX, _sY, _sW, sourcePix);

			if(_imageUsage[targetPix] == B) {
				_nodes[(i*_sW + j)*6 + OBJ] = 0;
				_nodes[(i*_sW + j)*6 + BKG] = 65535;
			}
			if(_imageUsage[targetPix] == NEW || _imageUsage[targetPix] == PINK) {
				
				// cost between points
				bool a, b;
				vector<int> valid = getValues(targetPix, &a, &b);

				_nodes[(i*_sW + j)*6 + OBJ] = 0;
				_nodes[(i*_sW + j)*6 + BKG] = 0;

				if(a)
					_nodes[(i*_sW + j)*6 + OBJ] = 65535;
				if(b)
					_nodes[(i*_sW + j)*6 + BKG] = 65535;

				for(int k = 0; k < 4; ++k) {

					if(valid[k] == NEW || valid[k] == PINK) {

						int neighbor = getNeighbor(sourcePix, k, _sW);
						int neighborT = getNeighbor(targetPix, k, _width);

						_nodes[(i*_sW + j)*6 + k] = getOldCost(targetPix, neighborT);

						int *seamNode = new int[6];

						seamNode[OBJ] = 0;
						seamNode[BKG] = getOldCost(targetPix, neighborT);
						
						seamNode[0] = i*_sW + j;
						seamNode[1] = neighbor;

						seamNode[2] = getNewCostA(source, targetPix, neighborT, _sX, _sY, _sW);
						seamNode[3] = getNewCostB(source, targetPix, neighborT, _sX, _sY, _sW);

						_seamNodes.push_back(seamNode);
					}
				}
			}
		}
	}
}

// Print
void MosaicGraph::print()
{
	/*
	out << "Nodes: " << endl;
	for(int i = 0; i < _nodes.size(); ++i) {
		out << i << ". ";
		for(int j = 0; j < 6; ++j) {
			out << _nodes[i][j] << " ";
		}
		out << endl;
	}
	out << endl;
	
	out << "Seam Nodes: " << endl;
	for(int i = 0; i < _seamNodes.size(); ++i) {
		out << i << ". ";
		for(int j = 0; j < 6; ++j) {
			out << _seamNodes[i][j] << " ";
		}
		out << endl;
	}
	out << endl;
	*/

	/*
	uchar *data = (uchar*)malloc(_width*_height*4);

	for(int i = 0; i < _height; ++i) {
		for(int j = 0; j < _width; ++j) {
			data[(i*_width + j)*4]     = _oldGrad[(i*_width + j)*2];
			data[(i*_width + j)*4 + 1] = _oldGrad[(i*_width + j)*2];
			data[(i*_width + j)*4 + 2] = _oldGrad[(i*_width + j)*2];
		}
	}

	QImage hGrad(data, _width, _height, QImage::Format_ARGB32);
	hGrad.save("hGrad.png");

	for(int i = 0; i < _height; ++i) {
		for(int j = 0; j < _width; ++j) {
			data[(i*_width + j)*4]     = _oldGrad[(i*_width + j)*2 + 1];
			data[(i*_width + j)*4 + 1] = _oldGrad[(i*_width + j)*2 + 1];
			data[(i*_width + j)*4 + 2] = _oldGrad[(i*_width + j)*2 + 1];
		}
	}

	QImage vGrad(data, _width, _height, QImage::Format_ARGB32);
	vGrad.save("vGrad.png");

	free(data);
	data = (uchar*)malloc(_sW*_sH*4);

	for(int i = 0; i < _sH; ++i) {
		for(int j = 0; j < _sW; ++j) {
			data[(i*_sW + j)*4]     = getGradientFromSource(i*_sW + j)[0];
			data[(i*_sW + j)*4 + 1] = getGradientFromSource(i*_sW + j)[0];
			data[(i*_sW + j)*4 + 2] = getGradientFromSource(i*_sW + j)[0];
		}
	}

	QImage shGrad(data, _sW, _sH, QImage::Format_ARGB32);
	shGrad.save("shGrad.png");

		for(int i = 0; i < _sH; ++i) {
		for(int j = 0; j < _sW; ++j) {
			data[(i*_sW + j)*4]     = getGradientFromSource(i*_sW + j)[1];
			data[(i*_sW + j)*4 + 1] = getGradientFromSource(i*_sW + j)[1];
			data[(i*_sW + j)*4 + 2] = getGradientFromSource(i*_sW + j)[1];
		}
	}

	QImage vhGrad(data, _sW, _sH, QImage::Format_ARGB32);
	vhGrad.save("vhGrad.png");

	free(data);
	*/
}

int MosaicGraph::solve()
{
	seamNodes = _seamNodes.size();
	total = nodes + seamNodes;

	if(g != NULL)
		delete g;

	g = new GraphT(total, total*5);

	for(int i = 0; i < total; ++i)
		g->add_node();

	for(int i = 0; i < nodes; ++i) {
		g->add_tweights(i, _nodes[i*6 + OBJ], _nodes[i*6 + BKG]);

		if(_nodes[i*6 + 0] != -1) g->add_edge(i, i+1,     _nodes[i*6 + 0], _nodes[i*6 + 0]);													   
		if(_nodes[i*6 + 1] != -1) g->add_edge(i, i+_sW-1, _nodes[i*6 + 1], _nodes[i*6 + 1]);
		if(_nodes[i*6 + 2] != -1) g->add_edge(i, i+_sW,   _nodes[i*6 + 2], _nodes[i*6 + 2]);
		if(_nodes[i*6 + 3] != -1) g->add_edge(i, i+_sW+1, _nodes[i*6 + 3], _nodes[i*6 + 3]);
	}

	for(int i = 0; i < seamNodes; ++i) 
	{
  		g->add_tweights(i+nodes, _seamNodes[i][OBJ], _seamNodes[i][BKG]);

		g->add_edge(i+nodes, _seamNodes[i][0], _seamNodes[i][3], _seamNodes[i][3]);
		g->add_edge(i+nodes, _seamNodes[i][1], _seamNodes[i][2], _seamNodes[i][2]);
	}

	int flow = g->maxflow();
	return flow;
}

void MosaicGraph::update()
{
	// change image color
	// from pixels on the graph
	for(int i = 0; i < nodes; ++i) {
		if(g->what_segment(i) == GraphT::SINK) {

			int tpix = targetP(_sX, _sY, _sW, i);
			
			_image[tpix*4]     = _source[i*4];
			_image[tpix*4 + 1] = _source[i*4 + 1];
			_image[tpix*4 + 2] = _source[i*4 + 2];
		}
	}

	// change old cost
	for(int i = 0; i < seamNodes; ++i) {

		int p1 = targetP(_sX, _sY, _sW, _seamNodes[i][0]);
		int p2 = targetP(_sX, _sY, _sW, _seamNodes[i][1]);

		if(g->what_segment(_seamNodes[i][0]) == g->what_segment(_seamNodes[i][1])) {

			if(g->what_segment(_seamNodes[i][0]) == GraphT::SINK)
				_oldCost[p1*4 + getNeighborIndex(p1, p2, _width)] = 0;

			if(g->what_segment(_seamNodes[i][0]) == GraphT::SOURCE)
				_oldCost[p1*4 + getNeighborIndex(p1, p2, _width)] = _seamNodes[i][BKG];
		}
		else
		{
			if(g->what_segment(_seamNodes[i][0]) == GraphT::SINK)
				_oldCost[p1*4 + getNeighborIndex(p1, p2, _width)] = _seamNodes[i][3];

			if(g->what_segment(_seamNodes[i][0]) == GraphT::SOURCE)
				_oldCost[p1*4 + getNeighborIndex(p1, p2, _width)] = _seamNodes[i][4];
		}
	}

	
	// change old gradient
	for(int i = 0; i < nodes; ++i) {
		if(g->what_segment(i) == GraphT::SINK) {

			int tpix = targetP(_sX, _sY, _sW, i);

			vector<int> gradient = getGradientFromSource(i);

			_oldGrad[tpix*2] = gradient[0];
			_oldGrad[tpix*2 + 1] = gradient[1];
		}
	}

	// show graphcut
	//for(int i = 0; i < _seamNodes.size(); ++i) {
	//	if(g->what_segment(_seamNodes[i][0]) != g->what_segment(_seamNodes[i][1])) {
	//
	//		int tpix = targetP(_sX, _sY, _sW, _seamNodes[i][0]);
	//		
	//		_image[tpix*4]     = 0;
	//		_image[tpix*4 + 1] = 0;
	//		_image[tpix*4 + 2] = 255;
	//
	//		tpix = targetP(_sX, _sY, _sW, _seamNodes[i][1]);
	//		
	//		_image[tpix*4]     = 0;
	//		_image[tpix*4 + 1] = 0;
	//		_image[tpix*4 + 2] = 255;
	//	}
	//}

	if(g != NULL)
		delete g;
	g = NULL;

	if(_nodes != NULL) {
		free(_nodes);
		_nodes = NULL;
	}
	for(int i = 0; i < _seamNodes.size(); ++i)
		delete[] _seamNodes[i];
	_seamNodes.clear();

	for(int i = 0; i < _height; ++i) {
		for(int j = 0; j < _width; ++j) {

			// update image usage
			if((_imageUsage[i*_width + j] != GREEN)&&(_imageUsage[i*_width + j] != PINK))
				_imageUsage[i*_width + j] = A;

			if(_imageUsage[i*_width + j] == GREEN)
			{
				int p = i*_width + j;
				if((_mask.bits()[p*4] == 255)&&(_mask.bits()[p*4 + 1] == 0)&&(_mask.bits()[p*4 + 2] == 255))
					_imageUsage[i*_width + j] = PINK;
			}

			if(_imageUsage[i*_width + j] == PINK)
			{
				int p = i*_width + j;
				// not green and not pink
				if(((_mask.bits()[p*4] != 255)||(_mask.bits()[p*4 + 1] != 0)||(_mask.bits()[p*4 + 2] != 255))&&
				   ((_mask.bits()[p*4] != 0)||(_mask.bits()[p*4 + 1] != 255)||(_mask.bits()[p*4 + 2] != 0)))
					_imageUsage[i*_width + j] = A;
			}
		}
	}

	// change colors
	// from _pixels not on the graph
	for(int i = _sY; i < _sY+_sH; ++i) {
		for(int j = _sX; j < _sX+_sW; ++j) {
			if(_imageUsage[i*_width + j] == PINK) {
	
				int pix = i*_width + j;
				int spix = sourceP(_sX, _sY, _sW, pix);
	
				_image[pix*4]	    = 0;
				_image[pix*4 + 1]	= 0;
				_image[pix*4 + 2]	= 0;
			}
		}
	}

}

void MosaicGraph::clean()
{
	if(g != NULL)
		 delete g;
	g = NULL;

	if(_nodes != NULL) {
		free(_nodes);
		_nodes = NULL;
	}
	for(int i = 0; i < _seamNodes.size(); ++i)
		delete[] _seamNodes[i];
	_seamNodes.clear();

	// Clean image usage mask
	//for(int i = _sY; i < _sY+_sH; ++i) {
	//	for(int j = _sX; j < _sX+_sW; ++j) {
	//
	//		if(_imageUsage[i*_width + j] == B) 
	//			_imageUsage[i*_width + j] = NONE;
	//		else
	//			_imageUsage[i*_width + j] = A;
	//	}
	//}
}

vector<int> MosaicGraph::getGradientFromSource(int sp)
{
	vector<int> gradient(2, 0);

	int a = sp, b = sp;

	if((sp + 1 < _sW*_sH)&&(sp % _sW < _sW - 1))
		a = getNeighbor(sp, 0, _sW);

	if((sp - 1 >= 0)&&(sp % _sW > 0))
		b = getNeighbor(sp, 4, _sW);
	
	int r1 = _source[a*4]     - _source[b*4];
	int b1 = _source[a*4 + 1] - _source[b*4 + 1];
	int g1 = _source[a*4 + 2] - _source[b*4 + 2];

	gradient[0] = (abs(r1) + abs(b1) + abs(g1))/3.0f;

	a = sp, b = sp;

	if(sp + _sW < _sW-1)
		a = getNeighbor(sp, 2, _sW);

	if(sp - _sW >= 0)
		b = getNeighbor(sp, 6, _sW);
	
	int r2 = _source[a*4] - _source[b*4];
	int b2 = _source[a*4 + 1] - _source[b*4 + 1];
	int g2 = _source[a*4 + 2] - _source[b*4 + 2];

	gradient[1] = (abs(r2) + abs(b2) + abs(g2))/3.0f;

	return gradient;
}

int MosaicGraph::getGradientDirection(std::vector<int> gradient, int direction)
{
	vector<int> dirVector(2, 0);

	switch(direction)
	{
		case 0:
			dirVector[0] = 1;
			dirVector[1] = 0;
		break;
		case 1:
			dirVector[0] = -1;
			dirVector[1] = 1;
		break;
		case 2:
			dirVector[0] = 0;
			dirVector[1] = 1;
		break;
		case 3:
			dirVector[0] = 1;
			dirVector[1] = 1;
		break;
		case 4:
			dirVector[0] = -1;
			dirVector[1] = 0;
		break;
		case 5:
			dirVector[0] = -1;
			dirVector[1] = -1;
		break;
		case 6:
			dirVector[0] = 0;
			dirVector[1] = -1;
		break;
		case 7:
			dirVector[0] = 1;
			dirVector[1] = -1;
		break;
	}

	// normalize
	float norm = sqrt(float(dirVector[0]*dirVector[0] + dirVector[1]*dirVector[1]));

	int a = dirVector[0];
	int b = dirVector[1];

	int c = gradient[0];
	int d = gradient[1];

	dirVector[0] /= norm;
	dirVector[1] /= norm;

	// get gradient on direction
	int dot = gradient[0]*dirVector[0] + gradient[1]*dirVector[1];

	return dot;
}

vector<int> MosaicGraph::getValues(int index, bool *a, bool *b)
{
	vector<int> valid(8, 0);

	if((index + 1 < _pixels)&&(index%_width < _width-1))		  valid[0] = _imageUsage[index+1];
	if((index + _width - 1 < _pixels)&&(index%_width > 0))        valid[1] = _imageUsage[index + _width - 1];
	if(index + _width < _pixels)	                              valid[2] = _imageUsage[index + _width];
	if((index + _width + 1 < _pixels)&&(index%_width < _width-1)) valid[3] = _imageUsage[index + _width + 1];

	if((index - 1 >= 0)&&(index%_width < _width-1))          valid[4] = _imageUsage[index-1];
	if((index - _width - 1 >= 0)&&(index%_width > 0))        valid[5] = _imageUsage[index - _width - 1];
	if(index - _width >= 0)	                                 valid[6] = _imageUsage[index - _width];
	if((index - _width + 1 >= 0)&&(index%_width < _width-1)) valid[7] = _imageUsage[index - _width + 1];

	*a = *b = false;

	if(_imageUsage[index] == A) *a = true;
	if(_imageUsage[index] == B) *b = true;

	for(int i = 0; i < 8; ++i) {
		if(valid[i] == A) *a = true;
		if(valid[i] == B) *b = true;
	}

	return valid;
}

int MosaicGraph::getNeighbor(int index, int n, int w)
{
	switch(n)
	{
	case 0:
		return index+1;
	case 1:
		return index+w-1;
	case 2:
		return index+w;
	case 3:
		return index+w+1;
	case 4:
		return index-1;
	case 5:
		return index-w-1;
	case 6:
		return index-w;
	case 7:
		return index-w+1;
	default:
		assert(false);
		return -1;
	}
}

int MosaicGraph::getNeighborIndex(int p1, int p2, int w)
{
	if(p2 == p1+1)   return 0;
	if(p2 == p1+w-1) return 1;
	if(p2 == p1+w)   return 2;
	if(p2 == p1+w+1) return 3;

	if(p2 == p1-1)   return 4;
	if(p2 == p1-w-1) return 5;
	if(p2 == p1-w)   return 6;
	if(p2 == p1-w+1) return 7;
	
	assert(false);
	return -1;
}

int MosaicGraph::getOldCost(int tp1, int tp2)
{
	int pixel, index;
	if(tp1 < tp2) {
		pixel = tp1;
		index = getNeighborIndex(tp1, tp2, _width);
	}
	else {
		pixel = tp2;
		index = getNeighborIndex(tp2, tp1, _width);
	}
	
	// MUST BE TARGET VALUE
	return _oldCost[pixel*4 + index];
}

int MosaicGraph::getNewCostB(int *source, int p1, int p2, int x, int y, int sW)
{
	int sp1 = sourceP(x, y, sW, p1);
	int sp2 = sourceP(x, y, sW, p2);

	int r1 = _image[p1*4]	  - source[sp1*4];
	int g1 = _image[p1*4 + 1] - source[sp1*4 + 1];
	int b1 = _image[p1*4 + 2] - source[sp1*4 + 2];

	int r2 = _image[p2*4]     - source[sp2*4];
	int g2 = _image[p2*4 + 1] - source[sp2*4 + 1];
	int b2 = _image[p2*4 + 2] - source[sp2*4 + 2];

	// get gradients
	vector<int> g1a = getGradientFromSource(sp1);
	vector<int> g1b = getGradientFromSource(sp2);

	vector<int> g2a;
	g2a.push_back(_oldGrad[p1*2]);
	g2a.push_back(_oldGrad[p1*2 + 1]);

	vector<int> g2b;
	g2b.push_back(_oldGrad[p2*2]);
	g2b.push_back(_oldGrad[p2*2 + 1]);

	// transform using direction
	double n1a = getGradientDirection(g1a, getNeighborIndex(p1, p2, _width));
	double n1b = getGradientDirection(g2a, getNeighborIndex(p1, p2, _width));
	double n2a = getGradientDirection(g1b, getNeighborIndex(p1, p2, _width));
	double n2b = getGradientDirection(g2b, getNeighborIndex(p1, p2, _width));

	double denominator = n1a + n1b + n2a + n2b;
	if(denominator == 0) denominator = 1;

	int value = (sqrt(double(r1*r1 + g1*g1 + b1*b1)) + sqrt(double(r2*r2 + g2*g2 + b2*b2)))/
		        denominator;

	return value;
}

int MosaicGraph::getNewCostA(int *source, int p1, int p2, int x, int y, int sW)
{
	int sp1 = sourceP(x, y, sW, p1);
	int sp2 = sourceP(x, y, sW, p2);

	int r1 = _image[p1*4]     - source[sp1*4];
	int g1 = _image[p1*4 + 1] - source[sp1*4 + 1];
	int b1 = _image[p1*4 + 2] - source[sp1*4 + 2];

	int r2 = _image[p2*4] 	  - source[sp2*4];
	int g2 = _image[p2*4 + 1] - source[sp2*4 + 1];
	int b2 = _image[p2*4 + 2] - source[sp2*4 + 2];

	// get gradients
	vector<int> g1a = getGradientFromSource(sp1);
	vector<int> g1b = getGradientFromSource(sp2);

	vector<int> g2a;
	g2a.push_back(_oldGrad[p1*2]);
	g2a.push_back(_oldGrad[p1*2 + 1]);

	vector<int> g2b;
	g2b.push_back(_oldGrad[p2*2]);
	g2b.push_back(_oldGrad[p2*2 + 1]);

	// transform using direction
	double n1a = getGradientDirection(g1a, getNeighborIndex(p1, p2, _width));
	double n1b = getGradientDirection(g2a, getNeighborIndex(p1, p2, _width));
	double n2a = getGradientDirection(g1b, getNeighborIndex(p1, p2, _width));
	double n2b = getGradientDirection(g2b, getNeighborIndex(p1, p2, _width));

	double denominator = n1a + n1b + n2a + n2b;
	if(denominator == 0) denominator = 1;

	int value = (sqrt(double(r1*r1 + g1*g1 + b1*b1)) + sqrt(double(r2*r2 + g2*g2 + b2*b2)))/
		        denominator;

	return value;
}

int MosaicGraph::sourceP(int x, int y, int sW, int pixelPos)
{
 	int i = pixelPos/_width;
	int j = pixelPos%_width;

	int newPPos = (i-y)*sW + (j-x);
	return newPPos;
}

int MosaicGraph::targetP(int x, int y, int sW, int pixelPos)
{
 	int i = pixelPos/sW + y;
	int j = pixelPos%sW + x;

	int newPPos = i*_width + j;
	return newPPos;
}
