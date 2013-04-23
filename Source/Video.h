#ifndef __VIDEO_H
#define __VIDEO_H

#include <string>
#include <RX/BBox.h>
#include <RX/VideoDecoder.h>
#include <QImage>
#include <QTimer>

class Video : public QObject
{
	Q_OBJECT

public:
	Video();
	~Video();

	void start(std::string folder);
	QImage labels() { return _labels; }
	QImage frame() { return _frame; }
	QImage frame(int frameNumber);
	int frameNumber() { return _frameNumber; }
	int maxFrameSeen() { return _maxFrameSeen; }
	bool started() { return _started; }

	int width() { return _width; }
	int height() { return _height; }
	int totalFrames() { return _totalFrames; }

public slots:
	void goToFrame(int number);
	void next();
	void previous();
	void play();
	void pause();

private:
	void markLabels();

	bool _started;
	int _frameNumber, _maxFrameSeen;
	int _width, _height;
	int _totalFrames;
	QImage _frame;
	QTimer _timer;
	RX::VideoDecoder _decoder;

	QImage _labels;
};

#endif // __VIDEOFROMIMAGES_H