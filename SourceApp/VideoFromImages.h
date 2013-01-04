#ifndef __VIDEOFROMIMAGES_H
#define __VIDEOFROMIMAGES_H

#include <string>
#include <QImage>

class VideoFromImages
{
public:
	VideoFromImages();
	~VideoFromImages();

	void createFromVideo(std::string folder, std::string filename);
	void goToFrame(int number);
	void next();

	QImage frame() { return _frame; }

private:
	int _frameNumber;
	QImage _frame;

	std::string _folder;
};

#endif // __VIDEOFROMIMAGES_H