#ifndef __SIFTMAPOBJ_H
#define __SIFTMAPOBJ_H

#include <vector>
#include <RX/mat3.h>
#include <QObject>
#include "SiftPoint.h"

class SiftMapObj : public QObject 
{
    Q_OBJECT

public:
    SiftMapObj();
    ~SiftMapObj();

	// Sets
	void setFirstFrameFromMatch(int file, int index);
	
	// Gets
	const int lastReady() const { return _lastReady; }

	const SiftPoint siftPoints(int file, int sift);
	const double siftPoints(int file, int sift, int xy);
	const int siftFrame(int file, int sift);

	const int numPoints(int file) const { return _sifts[file].size(); }
	const int numMatches(int file) const { return _matches[file].size(); }
	const std::pair<int, int> match(int file, int index) const 
	{ 
		Q_ASSERT(file < _matches.size());
		return _matches[file][index];
	}

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	std::vector< std::vector<SiftPoint> > _sifts;
    std::vector< std::vector< std::pair<int, int> > > _matches;
	int _lastReady;
};

#endif // __SIFTMAPOBJ_H