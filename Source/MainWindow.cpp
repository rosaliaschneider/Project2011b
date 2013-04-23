#include <QProgressDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSound>
#include "MainWindow.h"
#include "Globals.h"
#include "HomographyObj.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionLoadWithMap, SIGNAL(triggered()), this, SLOT(loadWithMap()));
	connect(ui->actionExportNotes, SIGNAL(triggered()), this, SLOT(exportNotes()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(ui->btBackwardOne, SIGNAL(clicked()), this, SLOT(previous()));
	connect(ui->btMoveContent, SIGNAL(clicked()), this, SLOT(moveContent()));

	connect(ui->wdgOpenGL, SIGNAL(goToFrame(int)), this, SLOT(goToFrame(int)));

	connect(ui->horScroll, SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll(int)));
	connect(ui->vertScroll, SIGNAL(valueChanged(int)), this, SLOT(verticalScroll(int)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::quit()
{
    close();
}

void MainWindow::load()
{
	// Prompt a video to load
	filename = QFileDialog::getOpenFileName(this, "Load Video",QString());
	if(!filename.isNull())
	{
		folder = filename.left(filename.lastIndexOf("/"));

		// Create folder for frames
		if(!QDir(folder+"/Frames").exists())
			QDir().mkdir(folder+"/Frames");

		// Create folder for maps
		if(!QDir(folder+"/Maps").exists())
			QDir().mkdir(folder+"/Maps");
		
		audio = new Phonon::MediaObject();
		audio->setCurrentSource(Phonon::MediaSource(filename));
		ui->wdgVideo->show();

		video.start(folder.toStdString());

		// Resize every global vector
		_globalSifts.resize(video.totalFrames());
		_sifts.resize(video.totalFrames());
		_matches.resize(video.totalFrames());
		_bboxes.resize(video.totalFrames());

		siftThread = new QThread();
		siftObj = new SiftObj();
		siftObj->moveToThread(siftThread);
		connect(siftThread, SIGNAL(started()), siftObj, SLOT(process()));
		connect(siftObj, SIGNAL(finished()), siftThread, SLOT(quit()));
		connect(siftObj, SIGNAL(finished()), siftObj, SLOT(deleteLater()));
		connect(siftThread, SIGNAL(finished()), siftThread, SLOT(deleteLater()));
		siftThread->start();

		homThread = new QThread();
		homObj = new HomographyObj();
		homObj->moveToThread(homThread);
		connect(homThread, SIGNAL(started()), homObj, SLOT(process()));
		connect(homObj, SIGNAL(finished()), homThread, SLOT(quit()));
		connect(homObj, SIGNAL(finished()), homObj, SLOT(deleteLater()));
		connect(homThread, SIGNAL(finished()), homThread, SLOT(deleteLater()));
		homThread->start();

		mapThread = new QThread();
		mapObj = new MapObj();
		mapObj->initialize();
		
		mapObj->moveToThread(mapThread);
		connect(mapThread, SIGNAL(started()), mapObj, SLOT(process()));
		connect(mapObj, SIGNAL(finished()), mapThread, SLOT(quit()));
		connect(mapObj, SIGNAL(finished()), mapObj, SLOT(deleteLater()));
		connect(mapThread, SIGNAL(finished()), mapThread, SLOT(deleteLater()));
		mapThread->start();
	}
}

void MainWindow::loadWithMap()
{
	// Prompt a video to load
	filename = QFileDialog::getOpenFileName(this, "Load Video",QString());
	if(!filename.isNull())
	{
		folder = filename.left(filename.lastIndexOf("/")); 

		// Create folder for frames
		if(!QDir(folder+"/Frames").exists())
			QDir().mkdir(folder+"/Frames");

		audio = new Phonon::MediaObject();
		audio->setCurrentSource(Phonon::MediaSource(filename));

		video.start(folder.toStdString());

		// Create map
		pdfHandler.loadPDF((folder+"/Slides.pdf").toStdString(), video.width(), video.height());
		clickMap.createMap(pdfHandler.numPages(), pdfHandler.pageWidth(), pdfHandler.pageHeight());
		for(int i = 0; i < pdfHandler.numPages(); ++i)
			clickMap.setPage(i, pdfHandler.getPage(i));

		// Resize every global vector
		_globalSifts.resize(video.totalFrames());
		_sifts.resize(video.totalFrames());
		_matches.resize(video.totalFrames());

		siftThread = new QThread();
		siftMapObj = new SiftMapObj();
		siftMapObj->moveToThread(siftThread);
		connect(siftThread, SIGNAL(started()), siftMapObj, SLOT(process()));
		connect(siftMapObj, SIGNAL(finished()), siftThread, SLOT(quit()));
		connect(siftMapObj, SIGNAL(finished()), siftMapObj, SLOT(deleteLater()));
		connect(siftThread, SIGNAL(finished()), siftThread, SLOT(deleteLater()));
		siftThread->start();
		
		homThread = new QThread();
		homMapObj = new HomographyMapObj();
		homMapObj->moveToThread(homThread);
		connect(homThread, SIGNAL(started()), homMapObj, SLOT(process()));
		connect(homMapObj, SIGNAL(finished()), homThread, SLOT(quit()));
		connect(homMapObj, SIGNAL(finished()), homMapObj, SLOT(deleteLater()));
		connect(homThread, SIGNAL(finished()), homThread, SLOT(deleteLater()));
		homThread->start();
	}
}

//  Export notes
void MainWindow::exportNotes()
{
	QString filename = QFileDialog::getSaveFileName(this, "Export Notes", QString(), QString("*.pdf"));
	if(!filename.isNull())
		clickMap.exportNotes(filename.toStdString());
}

//  Display next frame
void MainWindow::next()
{
	stop();
	video.next();
}

//  Display previous frame
void MainWindow::previous()
{
	stop();
	video.previous();
}

void MainWindow::play()
{
	ui->wdgVideo->play(audio->currentSource());
	video.play();
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::stop()
{
	ui->wdgVideo->pause();
	video.pause();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}

void MainWindow::goToFrame(int frame)
{
	video.goToFrame(frame);
	ui->wdgVideo->seek(frame*100);
}

void MainWindow::moveContent()
{
	homObj->moveToRight(video.frameNumber());
}

void MainWindow::horizontalScroll(int value)
{
	stop();
	ui->wdgOpenGL->moveHorizontally(value);
}

void MainWindow::verticalScroll(int value)
{
	stop();
	ui->wdgOpenGL->moveVertically(value);
}