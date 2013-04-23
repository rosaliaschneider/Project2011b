#include <RX/Quad.h>
#include "Globals.h" 

// resources
Video video;
Phonon::MediaObject *audio;
PDFHandler pdfHandler;
Map clickMap;

// info
QString folder;
QString filename;

// threads
MapObj *mapObj;
HomographyObj *homObj;
SiftObj *siftObj;
HomographyMapObj *homMapObj;
SiftMapObj *siftMapObj;

QThread *siftThread;
QThread *homThread;
QThread *mapThread;

// globals
std::vector<SiftPoint> _globalSifts;
std::vector< std::vector<SiftPoint> > _sifts;
std::vector< std::vector< std::pair<int, int> > > _matches;

// label drawing
std::vector< RX::BBox<RX::vec2> > _bboxes;
RX::Quad<RX::vec2> _labelQuad;
