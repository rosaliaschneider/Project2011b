#ifndef __VIDEO_H
#define __VIDEO_H

#include <string>
#include <QImage>
#include <QTimer>

class Video : public QObject
{
	Q_OBJECT

public:
	Video();
	~Video();

	void load(std::string folder);
	QImage frame() { return _frame; }
	int frameNumber() { return _frameNumber; }
	int totalFrames() { return _totalFrames; }

	int width() { return _frame.width(); }
	int height() { return _frame.height(); }

public slots:
	void goToFrame(int number);
	void next();
	void play();
	void pause();

private:
	int _frameNumber, _totalFrames;
	QImage _frame;
	std::string _folder;
	QTimer _timer;
};

#endif // __VIDEOFROMIMAGES_H