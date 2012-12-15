#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
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
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(50);

}

MainWindow::~MainWindow()
{
	ui->widget->saveIntervals("_Intervals.txt");
    delete ui;
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
		loadInfo((folder+"\\_MoveInfo.txt").toStdString());
		_decoder.load(folder.toStdString(), _numFrames);

		_currentFrame = -1;
		
		loadBoards((folder+"\\_Boards.txt").toStdString());
		loadHomographies((folder+"\\_Homs.txt").toStdString());
		loadPBBox((folder+"\\_BBox.txt").toStdString());

		ui->widget->setFrame(&_frame);
		ui->widget->setCurrentFrame(&_currentFrame);

		ui->widget->setBoards(&_boards);
		ui->widget->setHomographies(&_homographies);
		ui->widget->setPBBox(&_pbbox);
		ui->widget->setIntervals();

		next();
	}
}

void MainWindow::loadInfo(std::string filename)
{
	std::ifstream input(filename);
	input >> _numFrames;
}

void MainWindow::loadBoards(std::string filename)
{
	_boards.clear();
	std::ifstream input(filename);

	int nboards;
	input >> _numFrames >> nboards;
	for(int i = 0; i < _numFrames; ++i) {
		vector<Board> boards;
		for(int j = 0; j < nboards; ++j) {
			Board board;
			double x, y;
			input >> x >> y;
			board._p1.x = x; board._p1.y = y;
			input >> x >> y;
			board._p2.x = x; board._p2.y = y;
			input >> x >> y;
			board._p3.x = x; board._p3.y = y;
			input >> x >> y;
			board._p4.x = x; board._p4.y = y;
			boards.push_back(board);
		}
		_boards.push_back(boards);
	}	
}

void MainWindow::loadHomographies(std::string filename)
{
	ifstream input(filename.c_str());

	_homographies.clear();
	int n;
	input >> n;

	for(int i = 0; i < n; ++i)
	{
		RX::mat3 m;
		double dummy;
		for(int j = 0; j < 3; ++j) {
			for(int k = 0; k < 3; ++k) {
				input >> dummy;
				m.set(j, k, dummy);
			}
		}
		_homographies.push_back(m);
	}
}

void MainWindow::loadPBBox(std::string filename)
{
	_pbbox.clear();
	std::ifstream input(filename);

	input >> _numFrames;
	for(int i = 0; i < _numFrames; ++i) {
		BBox b;
		for(int j = 0; j < 4; ++j)
		{
			double x, y;
			input >> x >> y;
			b.p[j].x = x; b.p[j].y = y;
		}
		_pbbox.push_back(b);
	}	
}

//  Display next frame
void MainWindow::next()
{
	if(_decoder.getNextFrame(&_frame))
		++_currentFrame;
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



