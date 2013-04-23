#ifndef __PDFHANDLER_H
#define __PDFHANDLER_H

#include <string>
#include <QImage>
#include <poppler-qt4.h>

class PDFHandler
{
public:
	PDFHandler();
	~PDFHandler();

	void loadPDF(std::string filename, int pageWidth, int pageHeight);

	// Gets
	int numPages();
	int pageWidth() { return _pageWidth; }
	int pageHeight() { return _pageHeight; }
	QImage getPage(int pageNumber);

private:
	Poppler::Document* document;
	int _pageWidth, _pageHeight;
};

#endif //__PDFHANDLER_H