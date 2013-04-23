#include "Globals.h" 

// resources
Video video;
Video videoMask;

// info
QString filename;

// threads
HomographyObj *homObj;
SiftObj *siftObj;
QThread *siftThread;
QThread *homThread;
