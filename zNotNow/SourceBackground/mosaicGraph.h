#include <fstream>
#include <vector>
#include <QImage>
#include "defines.h"

class MosaicGraph
{
public:
	// Create/Destroy
	MosaicGraph(int width, int height);
	~MosaicGraph();

	// Set graph
	void setImageUsage(QImage mask);
	void saveImageUsage(int index);
	void createGraphFromSource(int *source, int x, int y, int sW, int sH);
	void createGraphFromSourceFirst(int *source, int x, int y, int sW, int sH);

	// Solve graphcut
	int solve();

	// Get gradient
	std::vector<int> getGradientFromSource(int sp);
	int getGradientDirection(std::vector<int> gradient, int direction);

	// Update image/graph
	void update();
	// Clean image/graph
	void clean();

	// Print
	void print();

	// Gets
	inline int *image() { return _image; }
	inline int width() { return _width; }
	inline int height() { return _height; }

private:

	GraphT *g;

	int seamNodes;
	int nodes;
	int total;

	// Aux
	int getNeighbor(int index, int n, int w);
	int getNeighborIndex(int p1, int p2, int w);

	int getOldCost(int tp1, int tp2);
	int getNewCostA(int *source, int p1, int p2, int x, int y, int sW);
	int getNewCostB(int *source, int p1, int p2, int x, int y, int sW);

	std::vector<int>getValues(int index, bool *a, bool *b);

	int sourceP(int x, int y, int sW, int pixelPos);
	int targetP(int x, int y, int sW, int pixelPos);

	static const int OBJ = 4;
	static const int BKG = 5;
	

	// Node Type
	enum { 
		GREEN = 0, 
		PINK,
		A, 
		B, 
		NEW
	};

	int *_nodes;

	std::vector< int* > _seamNodes;
	
	int _width, _height, _pixels;

	// point to constant, cannot change source
	const int *_source; 
	int _sX, _sY, _sW, _sH;

	int *_image;
	uchar *_imageUsage;
	int *_oldCost;
	int *_oldGrad;

	QImage _mask, _oldMask;
};
