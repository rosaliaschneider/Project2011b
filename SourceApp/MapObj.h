#ifndef __MAPOBJ_H
#define __MAPOBJ_H

#include <vector>
#include <RX/mat3.h>
#include <QObject>

class MapObj : public QObject 
{
    Q_OBJECT

public:
    MapObj();
    ~MapObj();

	// Sets
	void setFolder(std::string folder) { _folder = folder; }

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
	int _lastReady;
	std::string _folder;
};

#endif // __MAPOBJ_H