#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include "Define.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(250);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::quit()
{
    close();
}

void MainWindow::load()
{
	// Prompt a video to load
	QString file = QFileDialog::getOpenFileName(this, "Load Video", QString());
	if(!file.isNull())
	{
		_decoderNormal.openFile(file);

		std::string folder = file.toStdString();
		folder = folder.substr(0, folder.find_last_of("/"));
		
		loadInfo(folder+"/MoveInfo.txt");		
		_decoderAlpha.openFile(QString((folder+"/Alpha.avi").c_str()));

		_currentFrame = -1;
		ui->widget->setFrame(&_frame);
		ui->widget->setReplace(&_replace);
		ui->widget->setCurrentFrame(&_currentFrame);
				
		next();
	}
}

void MainWindow::loadInfo(std::string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames;
	int dimX, dimY, nBoards, nMovements;
	input >> dimX >> dimY;
	input >> nBoards;
}

void MainWindow::loadBoards(std::string filename)
{
	std::ifstream input(filename.c_str());

	int nBoards;
	input >> _numFrames;
	input >> nBoards;
	for(int i = 0; i < _numFrames; ++i) {
		for(int j = 0; j < nBoards; ++j) {

		}
	}

}

//  Display next frame
void MainWindow::next()
{
	if(_decoderNormal.seekNextFrame() && _decoderAlpha.seekNextFrame())
	{
		_decoderNormal.getFrame(_frame);
		_decoderAlpha.getFrame(_replace);
		++_currentFrame;
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
