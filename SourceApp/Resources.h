#ifndef __RESOURCES_H
#define __RESOURCES_H

#include <string>
#include <QImage>
#include <Phonon/MediaObject.h>
#include "VideoFromImages.h"

class Resources
{
public:
	void load(std::string folder);

	// Gets
	Phonon::MediaSource audioSource();
	QImage map() { return _map; }
	QImage currentFrame() { return _videoFromImages.frame(); }

private:
	void loadMap(std::string filename);

	// video from images to display on main window
	VideoFromImages _videoFromImages;
	// phonon object to seek and play sound accordingly
	Phonon::MediaObject *audio;
	// click map
	QImage _map;
};

#endif // __RESOURCES_H