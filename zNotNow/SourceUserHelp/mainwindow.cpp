#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->btStart, SIGNAL(clicked()), this, SLOT(startMovement()));
	connect(ui->btEnd, SIGNAL(clicked()), this, SLOT(endMovement()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(ui->btBackwardOne, SIGNAL(clicked()), this, SLOT(previous()));
	connect(ui->spinScale, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	connect(ui->glWidget, SIGNAL(selectedBoard(int)), ui->cmbBoards, SLOT(setCurrentIndex(int)));
	connect(ui->glWidget, SIGNAL(selectedPoint(int)), ui->cmbPoints, SLOT(setCurrentIndex(int)));
	connect(ui->glWidget, SIGNAL(selectedBoardAndPoint(int, int)), this, SLOT(selectBoardAndPoint(int, int)));

	_timer.setInterval(30);
}

MainWindow::~MainWindow()
{
	saveInfo();
    delete ui;
}

void MainWindow::quit()
{
    close();
}

void MainWindow::load()
{
    // Prompt a video to load
	QString filename = QFileDialog::getOpenFileName(this, "Load Video");
	if(filename.isNull())
		return;
	if(!_decoder.openFile(filename)) {
		QMessageBox::critical(this, "Error", "Error loading the video");
		return;
	}
	_currentFrame = -1;

	QString folder = filename.left(filename.lastIndexOf("/"));
	ui->glWidget->setFolder(folder.toStdString());
	ui->glWidget->setNumberOfBoards(9);
	ui->glWidget->setFrame(&_frame);
	ui->glWidget->setCurrentFrame(&_currentFrame);

	next();
}

//  Display next frame
void MainWindow::next()
{
	if(_decoder.seekNextFrame())
	{
		_decoder.getFrame(_frame);
		++_currentFrame;
	}
	char buf[10];
	sprintf(buf, "%d", _currentFrame);
	ui->txtCurrentFrame->setText(buf);
}

void MainWindow::previous()
{
	if(_decoder.seekPreviousFrame())
	{
		_decoder.getFrame(_frame);
		--_currentFrame;
	}
}

void MainWindow::play()
{
	_timer.start();
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::stop()
{
	_timer.stop();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}

void MainWindow::startMovement()
{
	ui->glWidget->startMovement(ui->cmbBoards->currentIndex()-1, _currentFrame);
}

void MainWindow::endMovement()
{
	ui->glWidget->endMovement(ui->cmbBoards->currentIndex()-1, _currentFrame);
}

void MainWindow::saveInfo()
{
	ui->glWidget->saveInfo("/Info.txt");
}

void MainWindow::selectBoardAndPoint(int board, int point)
{
}

void MainWindow::setScale(double scale)
{
	ui->glWidget->setScale(scale);
}
