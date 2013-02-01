#include "Globals.h" 

// main resources
Video video;
Phonon::MediaObject *audio;

// secondary resources
QImage clickMap;
std::vector<Region> regions;

HomographyObj *homObj;
QThread *homThread;

SiftObj *siftObj;
QThread *siftThread;

