#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <vector>
#include <RX/mat3.h>
#include <RX/BBox.h>
#include <QImage>
#include <QThread>
#include <Phonon/MediaObject.h>
#include "Video.h"
#include "Region.h"
#include "HomographyObj.h"
#include "SiftObj.h"

// main resources
extern Video video;
extern Phonon::MediaObject *audio;

// secondary resources
extern QImage clickMap;
extern std::vector<Region> regions;

extern HomographyObj *homObj;
extern QThread *homThread;

extern SiftObj *siftObj;
extern QThread *siftThread;

#endif  // __GLOBALS_H