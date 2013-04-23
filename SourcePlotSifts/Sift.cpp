#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <RX/vec2.h>
#include "SiftPoint.h"
#include "Sift.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <gl/glew.h>
#endif

using std::vector;
using std::iostream;
using std::fstream;
using std::endl;
using RX::vec2;

Sift::Sift()
{
}

Sift::~Sift()
{
	delete sift;
    delete matcher;
}

void Sift::initialize()
{
    sift = new SiftGPU;
    matcher = new SiftMatchGPU(4096);

    char * argv[] = {"-fo", "-1",  "-v", "1"};
    int argc = sizeof(argv)/sizeof(char*);
    sift->ParseParam(argc, argv);

    if(sift->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED) return;

	matcher->VerifyContextGL(); //must call once
}

void Sift::first(int width, int height, char *bits, int format, int type, int frame, vector<SiftPoint> *siftPoints)
{
	if(sift->RunSIFT(width, height, bits, format, type))
    {
        num1 = sift->GetFeatureNum();
        keys1.resize(num1);    
		descriptors1.resize(128*num1);
        sift->GetFeatureVector(&keys1[0], &descriptors1[0]);
    }
	siftPoints->resize(num1);
	for(int i = 0; i < num1; ++i)
		(*siftPoints)[i] = SiftPoint(keys1[i].x, keys1[i].y, frame);
}

void Sift::second(int width, int height, char *bits, int format, int type, int frame, vector<SiftPoint> *siftPoints)
{
	if(sift->RunSIFT(width, height, bits, format, type))
    {
        num2 = sift->GetFeatureNum();
        keys2.resize(num2);    
		descriptors2.resize(128*num2);
        sift->GetFeatureVector(&keys2[0], &descriptors2[0]);
    }

	siftPoints->resize(num2);
	for(int i = 0; i < num2; ++i)
		(*siftPoints)[i] = SiftPoint(keys2[i].x, keys2[i].y, frame);
}

void Sift::copySecondToFirst()
{
	num1 = num2;
	keys1 = keys2;
	descriptors1 = descriptors2;
}

void Sift::printFirst(std::string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "w");

	fprintf(file, "%d\n", num1);
	for(int i = 0; i < 128*num1; ++i)
		fprintf(file, "%lf\n", descriptors1[i]);
	for(int i = 0; i < num1; ++i) {
		fprintf(file, "%lf %lf %lf %lf\n", keys1[i].x ,keys1[i].y, keys1[i].s, keys1[i].o);
	}
}

void Sift::printSecond(std::string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "w");

	fprintf(file, "%d\n", num2);
	for(int i = 0; i < 128*num2; ++i)
		fprintf(file, "%lf\n", descriptors2[i]);
	for(int i = 0; i < num2; ++i) {
		fprintf(file, "%lf %lf %lf %lf\n", keys2[i].x ,keys2[i].y, keys2[i].s, keys2[i].o);
	}
}

void Sift::match(vector<pair<int, int> > *matches, vector<SiftPoint> *_sifts, const vector<SiftPoint> &_oldSifts)
{
	matcher->SetDescriptors(0, num1, &descriptors1[0]); //image 1
    matcher->SetDescriptors(1, num2, &descriptors2[0]); //image 2

	// match and get result.    
    int (*match_buf)[2] = new int[num1][2];
    // use the default thresholds. Check the declaration in SiftGPU.h
    int num_match = matcher->GetSiftMatch(num1, match_buf);
        
    // enumerate all the feature matches

    for(int i  = 0; i < num_match; ++i) {
        matches->push_back(pair<int, int>(match_buf[i][0], match_buf[i][1]));
		(*_sifts)[match_buf[i][1]].firstFrame = _oldSifts[match_buf[i][0]].firstFrame;

	}
    
	delete[] match_buf;
}  