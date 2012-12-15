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
	connect(ui->actionLoadBox, SIGNAL(triggered()), this, SLOT(loadBox()));
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
	QString file = QFileDialog::getOpenFileName(this, "Load Video",QString());
	if(!file.isNull())
	{
		_decoderNormal.openFile(file);

		string folder = file.toStdString();
		folder = folder.substr(0, folder.find_last_of("/"));
		
		loadInfo(folder+"/_MoveInfo.txt");
		loadBoards(folder+"/_LocalBoards.txt");
		loadHomographies(folder+"/_LocalHoms.txt");
		_decoderAlpha.openFile(QString((folder+"/Alpha.avi").c_str()));
		
		_currentFrame = -1;
		ui->widget->setFrame(&_frame);
		ui->widget->setAlpha(&_alpha);
		ui->widget->setBoards(&_boards);
		ui->widget->setHomographies(&_homographies);
		ui->widget->setCurrentFrame(&_currentFrame);
				
		next();
	}
}

void MainWindow::loadInfo(std::string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames;
	int dimX, dimY;
	input >> dimX >> dimY;
}

void MainWindow::loadBoards(std::string filename)
{
	ifstream input(filename.c_str());
	
	int nFrames, nBoards;
	input >> nFrames >> nBoards;
	for(int i = 0; i < nFrames; ++i) 
	{
		vector<BBox> boards;
		for(int j = 0; j < nBoards; ++j) 
		{
			BBox b(4);
			double x, y;
			for(int k = 0; k < 4; ++k) {
				input >> x >> y;
				b.setX(k, x);
				b.setY(k, y);
			}
			boards.push_back(b);
		}
		_boards.push_back(boards);
	}
}

void MainWindow::loadHomographies(std::string filename)
{
	ifstream input(filename.c_str());
	
	int nFrames;
	input >> nFrames;
	for(int i = 0; i < nFrames; ++i) 
	{
		RX::mat3 m;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				double value;
				input >> value;
				m.set(j, k, value);
			}
		}
		_homographies.push_back(m);
	}
}

//  Display next frame
void MainWindow::next()
{
	if(_decoderNormal.seekNextFrame() && _decoderAlpha.seekNextFrame()) {
		_decoderNormal.getFrame(_frame);
		_decoderAlpha.getFrame(_alpha);
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
