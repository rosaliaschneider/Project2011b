#ifndef __HOMOGRAPHYMAPOBJ_H
#define __HOMOGRAPHYMAPOBJ_H

#include <vector>
#include <fstream>
#include <map>
#include <RX/mat3.h>
#include <QObject>
#include "Homography.h"
#include "SiftPoint.h"

class HomographyMapObj : public QObject 
{
    Q_OBJECT

public:
    HomographyMapObj();
    ~HomographyMapObj();

	void moveToRight(int index);

	// Gets
	const int lastReady() const { return _lastReady; }
	const RX::mat3 hom(int index) const { return _homographies[index]; }

	const int numGlobalSifts(int frame) { return _globalSifts[frame].size(); }
	const double globalSiftX(int frame, int index) { return _globalSifts[frame][index].pos.x; }
	const double globalSiftY(int frame, int index) { return _globalSifts[frame][index].pos.y; }
	const int globalSiftFrame(int frame, int index) { return _globalSifts[frame][index].firstFrame; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	std::vector< std::vector<SiftPoint> > _globalSifts;
	std::map< std::pair<int, int>, bool > _hasSifts;

	Homography _homography;
    std::vector<RX::mat3> _homographies;

	int _frame, _nextFrame, _lastReady;
	std::string folder;

	// provisory
	std::ofstream log;
};

#endif // __HOMOGRAPHYMAPOBJ_H