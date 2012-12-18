#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include "Homography.h"
#include "Define.h"
#include "Checkpoint.h"
#include "mainwindow.h"
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
	connect(ui->btMove, SIGNAL(clicked()), this, SLOT(moveFrame()));

	connect(ui->spinScale, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
	connect(ui->spinLeft,  SIGNAL(valueChanged(int)), this, SLOT(moveLeft(int)));
	connect(ui->spinRight, SIGNAL(valueChanged(int)), this, SLOT(moveRight(int)));
	connect(ui->spinUp,    SIGNAL(valueChanged(int)), this, SLOT(moveUp(int)));
	connect(ui->spinDown,  SIGNAL(valueChanged(int)), this, SLOT(moveDown(int)));

	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(50);
}

MainWindow::~MainWindow()
{
	saveBoards(_folder+"/Boards.txt");
	saveGlobalHoms(_folder+"/GlobalHoms.txt");
    delete ui;
}

void MainWindow::quit()
{
    close();
}

void MainWindow::load()
{
	// Prompt a video to load
	QString folder = QFileDialog::getExistingDirectory(this, "Load Video");
	if(!folder.isNull())
	{
		_folder = folder.toStdString();

		loadHoms((folder+"/Homs.txt").toStdString());
		loadInfo((folder+"/MoveInfo.txt").toStdString());
		process();
				
		_currentFrame = -1;
		ui->widget->setCurrentFrame(&_currentFrame);
		ui->widget->setFrame(&_frame);
		ui->widget->setHomographies(&_homographies);
		ui->widget->setGlobalHomographies(&_globalHomographies);
		ui->widget->setBoards(&_boards);
		
		_decoder.openFile(folder+"/Video.avi");
		next();
	}
}

void MainWindow::loadHoms(std::string filename)
{
	std::ifstream input(filename.c_str());

	input >> _numFrames;
	for(int i = 0; i < _numFrames; ++i)
	{
		RX::mat3 m;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				double d;
				input >> d;
				m.set(j, k, d);
			}
		}
		_homographies.push_back(m);
	}
}

void MainWindow::loadInfo(std::string filename)
{
	int dimX, dimY;

	std::ifstream input(filename.c_str());

	input >> _numFrames;
	input >> dimX >> dimY;
	input >> _numBoards;

	// allocate boards
	_boards.resize(_numBoards);
	for(int i = 0; i < _numBoards; ++i)
		_boards[i].resize(_numFrames);

	_boardPositions.resize(_numBoards);
	_boardFrames.resize(_numBoards);

	for(int i = 0; i < _numBoards; ++i) 
	{
		BBox b(4);
		double x, y;
		for(int k = 0; k < 4; ++k) {
			input >> x >> y;
			b.setX(k, x);
			b.setY(k, y);
		}
		_boardPositions[i] = b;

		for(int j = 0; j < 4; ++j) 
		{
			int frame;
			input >> frame;
			_boardFrames[i].push_back(frame);
		}

		int movement;
		input >> movement;
	}
}

void MainWindow::process()
{
	// global homographies initial estimation
	_globalHomographies.push_back(_homographies[0]);

	for(int i = 1; i < _numFrames; ++i)
		_globalHomographies.push_back(_globalHomographies[i-1] * _homographies[i]);
	
	for(int i = 0; i < _numBoards; ++i)
	{
		RX::vec3 p1 = _globalHomographies[_boardFrames[i][0]] * RX::vec3(_boardPositions[i].getPoint(0), 1);
		RX::vec3 p2 = _globalHomographies[_boardFrames[i][1]] * RX::vec3(_boardPositions[i].getPoint(1), 1);
		RX::vec3 p3 = _globalHomographies[_boardFrames[i][2]] * RX::vec3(_boardPositions[i].getPoint(2), 1);
		RX::vec3 p4 = _globalHomographies[_boardFrames[i][3]] * RX::vec3(_boardPositions[i].getPoint(3), 1);
		p1.divideByZ();
		p2.divideByZ();
		p3.divideByZ();
		p4.divideByZ();

		for(int j = 0; j < _numFrames; ++j)
			_boards[i][j] = BBox(p1.xy(), p2.xy(), p3.xy(), p4.xy());
	}
}

void MainWindow::saveBoards(std::string filename)
{
	ofstream output(filename.c_str());
	output << _numFrames << endl;
	output << _numBoards << endl;
	for(int i = 0; i < _numFrames; ++i)
	{
		for(int j = 0; j < _numBoards; ++j)
		{
			for(int k = 0; k < 4; ++k)
				output << _boards[j][i].getX(k) << " " << _boards[j][i].getY(k) << " ";
			output << endl;
		}
	}
}

void MainWindow::saveGlobalHoms(std::string filename)
{
	ofstream output(filename.c_str());
	output << _numFrames << endl;
	for(int i = 0; i < _numFrames; ++i)
	{
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				output << _globalHomographies[i].at(j, k) << " ";
			}
			output << endl;
		}
	}
}

//  Display next frame
void MainWindow::next()
{
	if(_decoder.seekNextFrame() && _currentFrame < _numFrames-1)
	{
		_decoder.getFrame(_frame);

		if(_currentFrame >= 0)
 			_globalHomographies.push_back(_globalHomographies[_currentFrame] * _homographies[_currentFrame+1]);
		else
			_globalHomographies.push_back(_homographies[0]);

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

void MainWindow::setScale(double scale)
{
	ui->widget->setScale(scale);
}

void MainWindow::moveFrame()
{
	ui->widget->startMovingFrame();
}

void MainWindow::moveLeft(int pixels)
{
	ui->widget->moveLeft(pixels);
}

void MainWindow::moveRight(int pixels)
{
	ui->widget->moveRight(pixels);
}

void MainWindow::moveUp(int pixels)
{
	ui->widget->moveUp(pixels);
}

void MainWindow::moveDown(int pixels)
{
	ui->widget->moveDown(pixels);
}


