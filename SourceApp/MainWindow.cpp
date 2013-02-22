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
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));

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
	QString folder = QFileDialog::getExistingDirectory(this, "Load Video",QString());
	if(!folder.isNull())
	{
		video.load(folder.toStdString());
		
		audio = new Phonon::MediaObject();
		audio->setCurrentSource(Phonon::MediaSource(folder+"/Video.avi"));

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
		homObj->setFolder(folder.toStdString());
		homObj->moveToThread(homThread);
		connect(homThread, SIGNAL(started()), homObj, SLOT(process()));
		connect(homObj, SIGNAL(finished()), homThread, SLOT(quit()));
		connect(homObj, SIGNAL(finished()), homObj, SLOT(deleteLater()));
		connect(homThread, SIGNAL(finished()), homThread, SLOT(deleteLater()));
		homThread->start();

	}
}

//  Display next frame
void MainWindow::next()
{
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