#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSound>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow), _sound(NULL)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(&_timer, SIGNAL(timeout()), this, SLOT(next()));

	_timer.setInterval(80);

}

MainWindow::~MainWindow()
{
    delete ui;
	if(_sound)
		delete _sound;
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
		_sound = new QSound(folder+"\\Sound.wav");
		loadBackground((folder+"\\background.png").toStdString());
		loadBackgroundF((folder+"\\backgroundF.png").toStdString());
		//loadIntervals(folder.toStdString());
		loadBoards((folder+"\\_LocalBoards.txt").toStdString());
		//loadPBBox((folder+"\\_BBox.txt").toStdString());
		loadHomographies((folder+"\\_LocalHoms.txt").toStdString());

		_decoder.load(folder.toStdString(), _numFrames);

		ui->widget->setFrame(&_frame);
		ui->widget->setBackground(&_background, &_backgroundF);
		ui->widget->setCurrentFrame(&_currentFrame);
		ui->widget->setBoards(&_boards);
		ui->widget->setIntervals(&_intervals);
		ui->widget->setPBBox(&_pbbox);
		ui->widget->setHomographies(&_homographies);
		ui->widget->setImages();

		_currentFrame = -1;
		next();
	}
}

void MainWindow::loadBackground(std::string filename)
{
	_background.load(filename.c_str());
}

void MainWindow::loadBackgroundF(std::string filename)
{
	_backgroundF.load(filename.c_str());
}

void MainWindow::loadBoards(std::string filename)
{
	_boards.clear();
	std::ifstream input(filename);

	int nboards;
	input >> _numFrames >> nboards;
	_boards.resize(_numFrames);
	for(int i = 0; i < _numFrames; ++i) {
		vector<Board> boards;
		for(int j = 0; j < nboards; ++j) {
			Board board;
			double x, y;
			input >> x >> y;
			board.p1.x = x; board.p1.y = y;
			input >> x >> y;
			board.p2.x = x; board.p2.y = y;
			input >> x >> y;
			board.p3.x = x; board.p3.y = y;
			input >> x >> y;
			board.p4.x = x; board.p4.y = y;
			
			boards.push_back(board);
		}
		_boards[i] = boards;
	}	
}

void MainWindow::loadIntervals(std::string folder)
{
	_intervals.clear();
	std::ifstream input(folder+"\\_Intervals.txt");

	int nboards;
	input >> nboards;
	for(int i = 0; i < nboards; ++i) {
		vector<Interval> intervals;
		int nImages;
		input >> nImages;
		for(int j = 0; j < nImages; ++j) {
			char buf[20];
			sprintf(buf, "\\board_%d_%d.png", i, j);
			string file = folder + buf;

			Interval interval;
			input >> interval.begin >> interval.end;
			interval.image.load(file.c_str());
			intervals.push_back(interval);
		}
		_intervals.push_back(intervals);
	}	
}

void MainWindow::loadHomographies(std::string filename)
{
	ifstream input(filename.c_str());

	_homographies.clear();
	int n;
	input >> n;
	_homographies.resize(n);
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
		_homographies[i] = m;
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
	_sound->play();
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::stop()
{
	_timer.stop();
	_sound->stop();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}
