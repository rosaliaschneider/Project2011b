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

	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(35);
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

		_decoder.openFile(folder+"/Video.avi");

		loadHoms((folder+"/Homs.txt").toStdString());
		loadInfo((folder+"/MoveInfo.txt").toStdString());
		process();
				
		_currentFrame = -1;
		ui->widget->setCurrentFrame(&_currentFrame);
		ui->widget->setFrame(&_frame);
		ui->widget->setHomographies(&_homographies);
		ui->widget->setGlobalHomographies(&_globalHomographies);
		ui->widget->setBoards(&_boards);
		
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

	_checkPositions.resize(_numBoards);
	_checkFrames.resize(_numBoards);

	for(int i = 0; i < _numBoards; ++i) 
	{
		BBox b(4);
		double x, y;
		for(int k = 0; k < 4; ++k) {
			input >> x >> y;
			b.setX(k, x);
			b.setY(k, y);
		}
		_checkPositions[i] = b;

		for(int j = 0; j < 4; ++j) 
		{
			int frame;
			input >> frame;
			_checkFrames[i].push_back(frame);
		}

		int movement;
		input >> movement;
	}
}

void MainWindow::process()
{
	// global homographies initial estimation
	vector<RX::mat3> globalHomographies;
	globalHomographies.push_back(_homographies[0]);

	for(int i = 1; i < _numFrames; ++i)
		globalHomographies.push_back(globalHomographies[i-1] * _homographies[i]);
	
	for(int i = 0; i < _numBoards; ++i)
	{
		RX::vec3 p1 = globalHomographies[_checkFrames[i][0]] * RX::vec3(_checkPositions[i].getPoint(0), 1);
		RX::vec3 p2 = globalHomographies[_checkFrames[i][1]] * RX::vec3(_checkPositions[i].getPoint(1), 1);
		RX::vec3 p3 = globalHomographies[_checkFrames[i][2]] * RX::vec3(_checkPositions[i].getPoint(2), 1);
		RX::vec3 p4 = globalHomographies[_checkFrames[i][3]] * RX::vec3(_checkPositions[i].getPoint(3), 1);
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
