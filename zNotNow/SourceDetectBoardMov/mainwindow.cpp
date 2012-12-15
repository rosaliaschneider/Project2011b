#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QKeyEvent>
#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow), _stopped(true), _fixing(false), _nextMovement(0)
{
	ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionLoadBoards, SIGNAL(triggered()), this, SLOT(loadBoards()));
	connect(ui->actionSaveBoards, SIGNAL(triggered()), this, SLOT(saveBoards()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(50);

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
		_decoder.openFile(file);

		string folder = file.toStdString();
		_folder = QString((folder.substr(0, folder.find_last_of("."))).c_str());
		
		loadInfo((_folder+"/_MoveInfo.txt").toStdString());
	  	
		ui->glWidget->setFrame(&_frame);
		ui->glWidget->setCurrentFrame(&_currentFrame);

		next();
	}
}

//  Display next frame
void MainWindow::next()
{
	//if(_fixing) {
	//	_decoder.getNextFrame(&_frame);
	//	++_currentFrame;
	//	if(_currentFrame == _movements[_nextMovement].end())
	//	{
	//		_fixing = false;
	//		++_nextMovement;
	//	}
	//}
	//else {
	//	if(_nextMovement >= _movements.size()) return;
	//	_decoder.getFrame(_movements[_nextMovement].start(), &_frame);
	//	_currentFrame = _movements[_nextMovement].start();
	//	_fixing = true;
	//	stop();
	//}
	if(_decoder.seekNextFrame())
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
	_stopped = false;
}

void MainWindow::stop()
{
	_timer.stop();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
	_stopped = true;
}

void MainWindow::loadInfo(std::string filename)
{
	// load board info from files
	std::ifstream input(filename.c_str());
	int nBoards, nBoardMovements, nMovements;
	input >> _numFrames;
	input >> _dimX >> _dimY;
	input >> nBoards;

	_boards.push_back(vector<BBox>());
	for(int i = 0; i < nBoards; ++i) {
		RX::vec2 p[4];
		for(int j = 0; j < 4; ++j) {
			input >> p[j].x >> p[j].y;
		}
		bool isBoardValid = false;
		for(int j = 0; j < 4; ++j) {
			int f;
			input >> f;
			if(f != -1) isBoardValid = true;
		}
		_boards[0].push_back(BBox(p[0], p[1], p[2], p[3]));
		input >> nMovements;
		for(int j = 0; j < nMovements; ++j) {
			int start, end;
			input >> start >> end;
			_movements.push_back(Movement(start, end, i));
		}
	}
	for(int i = 1; i < _numFrames; ++i)
		_boards.push_back(_boards[0]);

	ui->glWidget->setMovements(&_movements);
	ui->glWidget->setBoardsPerFrame(&_boards);
}

void MainWindow::loadBoards()
{
	// load board info from files
	//std::ifstream input((_folder+"/_LocalBoards.txt").toStdString());
	//
	//int size;
	//input >> size;
	//
	//_movements.clear();
	//for(int i = 0; i < size; ++i) {
	//	
	//	int start, end, board;
	//	input >> start >> end >> board;
	//
	//	Movement m = Movement(start, end, board);
	//	for(int j = m.start(); j <= m.end(); ++j)
	//	{
	//		double x, y;
	//		for(int k = 0; k < 4; ++k) {
	//			input >> x >> y;
	//			m.setX(j, k, x);
	//			m.setY(j, k, y);
	//		}
	//
	//	}
	//	_movements.push_back(m);
	//}
	//
	//ui->glWidget->setMovements(&_movements);
}

void MainWindow::saveBoards()
{
	// save board info to files
	std::ofstream output((_folder+"/_LocalBoards.txt").toStdString());
	output << _boards.size() << endl;
	output << _boards[0].size() << endl;
	for(int i = 0; i < _boards.size(); ++i) 
	{
		for(int j = 0; j < _boards[i].size(); ++j) 
		{
			output << _boards[i][j].getX(0) << " "  << _boards[i][j].getY(0) << endl;
			output << _boards[i][j].getX(1) << " "  << _boards[i][j].getY(1) << endl;
			output << _boards[i][j].getX(2) << " "  << _boards[i][j].getY(2) << endl;
			output << _boards[i][j].getX(3) << " "  << _boards[i][j].getY(3) << endl;
		}
	}
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
	switch(ev->key())
	{
	case Qt::Key_Space:
		if(_stopped)
			play();
		else
			stop();
		break;
	}
}