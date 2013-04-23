#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QThread>
#include "Define.h"
#include "Globals.h"
#include "SiftObj.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow), _showMatch(false), _usingMap(false)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoadVideo, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionLoadImage, SIGNAL(triggered()), this, SLOT(loadImage()));
	connect(ui->actionLoadSifts, SIGNAL(triggered()), this, SLOT(loadSifts()));
	connect(ui->widget, SIGNAL(nearPoint(int)), this, SLOT(nearPoint(int)));

	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
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
	QString filename = QFileDialog::getOpenFileName(this, "Load Video",QString());
	if(!filename.isNull())
	{
		siftObj = new SiftObj();
		siftObj->setFilename(filename.toStdString());

		_currentFrame = 1;
		updateInterface();
	}
}

void MainWindow::loadImage()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load Image",QString());
	if(!filename.isNull())
	{
		image = QImage(filename);
		ui->widget->setFrame1(&image);
	}
}

void MainWindow::loadSifts()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load Sifts",QString());
	if(!filename.isNull())
	{
		ifstream input(filename.toAscii());
		int n;
		input >> n;
		for(int i = 0; i < n; ++i)
		{
			SiftPoint p;
			input >> p.pos.x >> p.pos.y;
			sifts.push_back(p);
		}
		for(int i = 0; i < n; ++i)
		{
			string info;
			input >> info;
			sifts[i].info = info;
		}
		ui->widget->setSifts1(&sifts);
	}
}

void MainWindow::play()
{
	_timer.start();
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::pause()
{
	_timer.stop();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}


void MainWindow::next()
{
	//if(!_usingMap)
	//	_sift.copySecondToFirst();
	//
	//_decoder.seekNextFrame();
	//_decoder.getFrame(_frame2);
	//_frame2.save("saveme.png");
	//_sift.second(_frame2.width(), _frame2.height(), (char*)(_frame2.bits()), GL_RGB, GL_UNSIGNED_BYTE);
	//_sift.match(&_matches, &_sifts1, &_sifts2);
	//
	//++_currentFrame;
	//updateInterface();
}

void MainWindow::nearPoint(int i)
{
	//char buf[20];
	//sprintf(buf, "%.3f", _sifts1[i].x);
	//ui->textPx->setText(buf);
	//sprintf(buf, "%.3f", _sifts1[i].y);
	//ui->textPy->setText(buf);
	//sprintf(buf, "%.3f", _sifts2[i].x);
	//ui->textPCx->setText(buf);
	//sprintf(buf, "%.3f", _sifts2[i].y);
	//ui->textPCy->setText(buf);
	//sprintf(buf, "%d", i);
	//ui->textPid->setText(buf);
}

void MainWindow::updateInterface()
{
	//char buf[20];
	//sprintf(buf, "%d", _matches.size());
	//ui->textPCorresp->setText(buf);
}