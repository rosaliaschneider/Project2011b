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

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
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
		QString folder = filename.left(filename.lastIndexOf("/"));

		// Create folder for frames
		if(!QDir(folder+"/FFrames").exists())
			QDir().mkdir(folder+"/FFrames");
		if(!QDir(folder+"/Frames").exists())
			QDir().mkdir(folder+"/Frames");

		if(!QDir(folder+"/Masks").exists())
			QDir().mkdir(folder+"/Masks");
		if(!QDir(folder+"/MMasks").exists())
			QDir().mkdir(folder+"/MMasks");

		ui->wdgOpenGL->setFolder((folder+"/Frames").toStdString());

		video.start(filename.toStdString(), (folder+"/FFrames").toStdString());
		videoMask.start((folder+"/Mask.avi").toStdString(), (folder+"/Masks").toStdString());
		videoMask.extrapolateMasks((folder+"/MMasks").toStdString());

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

	}	
}
