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

	// Gets
	const int lastReady() const { return _lastReady; }
	const RX::mat3 hom(int index) const { return _homographies[index]; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	Homography _homography;
    std::vector<RX::mat3> _homographies;
	int _frame, _nextFrame, _lastReady;
};

#endif // __HOMOGRAPHYOBJ_H