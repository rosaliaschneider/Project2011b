#ifndef __HOMOGRAPHYOBJ_H
#define __HOMOGRAPHYOBJ_H

#include <vector>
#include <fstream>
#include <map>
#include <RX/mat3.h>
#include <QObject>
#include "Homography.h"
#include "SiftPoint.h"

class HomographyObj : public QObject 
{
    Q_OBJECT

public:
    HomographyObj();
    ~HomographyObj();

	void moveToRight(int index);

	// Gets
	const int lastReady() const { return _lastReady; }
	const RX::mat3 hom(int index) const { return _homographies[index]; }

	const int numGlobalSifts() { return _globalSifts.size(); }
	const double globalSiftX(int index) { return _globalSifts[index].pos.x; }
	const double globalSiftY(int index) { return _globalSifts[index].pos.y; }
	const int globalSiftFrame(int index) { return _globalSifts[index].firstFrame; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	std::vector<SiftPoint> _globalSifts;
	std::map< std::pair<int, int>, bool > _hasSifts;

	Homography _homography;
    std::vector<RX::mat3> _homographies;

	int _frame, _nextFrame, _lastReady;

	// provisory
	std::ofstream log;
};

#endif // __HOMOGRAPHYOBJ_H