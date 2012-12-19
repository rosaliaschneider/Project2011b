#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSound>
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
		loadBoards((folder+"/FinalBoards.txt").toStdString());
		loadRegions((folder+"/Regions.txt").toStdString());
		loadIntervals((folder+"/Intervals.txt").toStdString());
		loadFinalFrame((folder+"/FinalFrame.png").toStdString());

		_media = new Phonon::MediaObject();
		_media->setCurrentSource(Phonon::MediaSource(folder+"/FinalVideo.avi"));

		ui->widget_2->setRegions(&_regions);
		ui->widget_2->setBoards(&_finalBoards);
		ui->widget_2->setFrame(&_frame);
		ui->widget_2->setVideoPlayer(ui->widget);
	}
}

void MainWindow::loadFinalFrame(std::string filename)
{
	_frame = QImage(filename.c_str());
}

void MainWindow::loadBoards(std::string filename)
{
	std::ifstream input(filename);

	int nboards, nframes;
	input >> nframes >>  nboards;

	_finalBoards.clear();
	_finalBoards.resize(nboards);

	vector<Board> boards(nboards, nframes);
	for(int j = 0; j < nframes; ++j) 
	{
		for(int i = 0; i < nboards; ++i) 
		{
			for(int k = 0; k < 4; ++k)
			{
				double x, y;
				input >> x >> y;
				boards[i].setPos(x, y, k, j);
			}
		}
	}	
	for(int i = 0; i < boards.size(); ++i)
		_finalBoards[i] = boards[i];
}

void MainWindow::loadRegions(std::string filename)
{
	std::ifstream input(filename);

	int nregions;
	input >> nregions;

	_regions.clear();
	_regions.resize(nregions);

	for(int i = 0; i < nregions; ++i) 
	{
		int startingFrame, nboxes;
		input >> startingFrame >> nboxes;
		_regions[i].setStartingFrame(startingFrame);

		for(int j = 0; j < nboxes; ++j) 
		{
			BBox b;
			for(int k = 0; k < 4; ++k)
			{
				double x, y;
				input >> x >> y;
				b.points[k] = RX::vec2(x, y);
			}
			_regions[i].addBox(b);
		}
	}	
}

void MainWindow::loadIntervals(std::string filename)
{
	std::ifstream input(filename);

	int nboards, nintervals;
	input >>  nboards;

	for(int i = 0; i < nboards; ++i) 
	{
		input >> nintervals;
		for(int j = 0; j < nintervals; ++j) 
		{
			Interval interval;
			input >> interval.begin >> interval.end >> interval.timeMsec;
			_finalBoards[i].addInterval(interval);
		}
	}
}

//  Display next frame
void MainWindow::next()
{
}

void MainWindow::play()
{
	ui->widget->play(_media->currentSource());
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::stop()
{
	ui->widget->pause();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}
