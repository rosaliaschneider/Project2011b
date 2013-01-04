#include "Resources.h"

void Resources::load(std::string folder)
{
	loadMap(folder+"/Map.png");

	audio = new Phonon::MediaObject();
	audio->setCurrentSource(Phonon::MediaSource((folder+"/Video.avi").c_str()));

	_videoFromImages.createFromVideo(folder, "/Video.avi");

}

void Resources::loadMap(std::string filename)
{
	_map = QImage(filename.c_str());
}

Phonon::MediaSource Resources::audioSource()
{
	return audio->currentSource();
}
