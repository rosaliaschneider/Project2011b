#include "PDFHandler.h"

using namespace std;

PDFHandler::PDFHandler()
: document(NULL)
{
}

PDFHandler::~PDFHandler()
{
	if(document)
		delete document;
}

void PDFHandler::loadPDF(string filename, int pageWidth, int pageHeight)
{
	document = Poppler::Document::load(filename.c_str());
	if (!document || document->isLocked()) {
	  // ... error message ....
	  delete document;
	  return;
	}

	_pageWidth = pageWidth;
	_pageHeight = pageHeight;
}

int PDFHandler::numPages() 
{ 
	if(document == NULL)
		return 0;
	return document->numPages(); 
}

QImage PDFHandler::getPage(int pageNumber)
{
	// Access page of the PDF file
	Poppler::Page* pdfPage = document->page(pageNumber);
	
	// Generate a QImage of the rendered page
	QImage image = pdfPage->renderToImage(96, 96, 0, 0, _pageWidth, _pageHeight);
	if (!image.isNull()) {
	  delete pdfPage;
	  return image;
	}
}