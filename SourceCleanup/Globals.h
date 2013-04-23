#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <vector>
#include <RX/mat3.h>
#include <RX/BBox.h>
#include <QImage>
#include <QThread>
#include <Phonon/MediaObject.h>
#include "Video.h"
#include "HomographyObj.h"
#include "SiftObj.h"

// resources
extern Video video;
extern Video videoMask;

// info
extern QString filename;

// threads
extern HomographyObj *homObj;
extern SiftObj *siftObj;
extern QThread *siftThread;
extern QThread *homThread;

#endif  // __GLOBALS_H