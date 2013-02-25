#ifndef __VIDEOOBJ_H
#define __VIDEOOBJ_H

#include <vector>
#include <RX/mat3.h>
#include <QObject>

class VideoObj : public QObject 
{
    Q_OBJECT

public:
    VideoObj();
    ~VideoObj();

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

#endif // __VIDEOOBJ_H