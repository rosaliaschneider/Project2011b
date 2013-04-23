#ifndef __MAP_H
#define __MAP_H

#include <QImage>

class Map
{
public:
	Map();
	~Map();

	// Create
	void createMap(int numPages, int pageWidth, int pageHeight);
	void setPage(int index, QImage &page);
	void exportNotes(std::string filename);
	
	// Sets
	void addImage(int img);

	// Gets
	QImage image(int frameNumber=-1);

private:
	QImage _map;
	bool _isSet;
	std::vector<int> _availableMaps;
};

#endif // __MAP_H