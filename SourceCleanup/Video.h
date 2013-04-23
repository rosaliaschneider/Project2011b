#ifndef __VIDEO_H
#define __VIDEO_H

#include <string>
#include <RX/BBox.h>
#include <RX/VideoDecoder.h>
#include <QImage>
#include <QTimer>
#include <QDir>

class Video : public QObject
{
	Q_OBJECT

public:
	Video();
	~Video();

	void start(std::string filename, std::string folder);

	QImage frame() { return _frame; }
	QImage frame(int frameNumber);
	int frameNumber() { return _frameNumber; }
	bool started() { return _started; }

	int width() { return _width; }
	int height() { return _height; }
	bool done() { return _done; }
	int totalFrames() { return _totalFrames; }

	void extrapolateMasks(std::string folder);

public slots:
	void goToFrame(int number);
	void next();
	void previous();
	void play();
	void pause();

private:
	bool _started, _done;
	int _frameNumber, _totalFrames;
	int _width, _height;
	QImage _frame;
	QTimer _timer;

	std::string _filename, _folder;
	RX::VideoDecoder _decoder;
};

#endif // __VIDEOFROMIMAGES_H