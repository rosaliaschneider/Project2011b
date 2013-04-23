#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <vector>
#include <RX/mat3.h>
#include <RX/BBox.h>
#include <RX/Quad.h>
#include <QImage>
#include <QThread>
#include <Phonon/MediaObject.h>
#include "PDFHandler.h"
#include "Video.h"
#include "Map.h"
#include "HomographyObj.h"
#include "SiftObj.h"
#include "HomographyMapObj.h"
#include "SiftMapObj.h"
#include "MapObj.h"

// resources
extern Video video;
extern Phonon::MediaObject *audio;
extern PDFHandler pdfHandler;
extern Map clickMap;

// info
extern QString folder;
extern QString filename;

// threads
extern MapObj *mapObj;

extern HomographyObj *homObj;
extern SiftObj *siftObj;
extern HomographyMapObj *homMapObj;
extern SiftMapObj *siftMapObj;

extern QThread *siftThread;
extern QThread *homThread;
extern QThread *mapThread;

// globals
extern std::vector<SiftPoint> _globalSifts;
extern std::vector< std::vector<SiftPoint> > _sifts;
extern std::vector< std::vector< std::pair<int, int> > > _matches;

// label drawing
extern std::vector< RX::BBox<RX::vec2> > _bboxes;
extern RX::Quad<RX::vec2> _labelQuad;

#endif  // __GLOBALS_H