#ifndef __SIFT_H
#define __SIFT_H

#include <string>
#include <vector>
#include <RX/vec2.h>
#include "SiftPoint.h"
#include "SiftGPU.h"

using namespace std;

class Sift
{
public:
	Sift();
	~Sift();

	void initialize();
	void makeContext();
	void first(int width, int height, char *bits, int format, int type, vector<SiftPoint> *siftPoints);
	void second(int width, int height, char *bits, int format, int type, vector<SiftPoint> *siftPoints);
	void match(std::vector< std::pair<int, int> > *matches);
	void printFirst(std::string filename);
	void printSecond(std::string filename);
	
private:
	SiftGPU *sift;
    SiftMatchGPU *matcher;

	vector<float> descriptors1, descriptors2;
    vector<SiftGPU::SiftKeypoint> keys1, keys2;    
    int num1, num2;

};

#endif // __SIFT_H