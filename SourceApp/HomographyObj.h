#ifndef __HOMOGRAPHYOBJ_H
#define __HOMOGRAPHYOBJ_H

#include <vector>
#include <RX/mat3.h>
#include <QObject>
#include "SiftPoint.h"

class HomographyObj : public QObject 
{
    Q_OBJECT

public:
    HomographyObj();
    ~HomographyObj();

	// Sets
	void setFolder(std::string folder) { _folder = folder; }

	// Gets
	const int lastReady() const { return _lastReady; }
	const RX::mat3 hom(int index) const { return _homographies[index]; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	std::vector<SiftPoint> _globalSifts;
    std::vector<RX::mat3> _homographies;
	int _lastReady;
	std::string _folder;
};

#endif // __HOMOGRAPHYOBJ_H