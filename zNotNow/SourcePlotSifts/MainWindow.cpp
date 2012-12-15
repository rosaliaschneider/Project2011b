#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include "Define.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent), ui(new Ui::MainWindow), _showMatch(false), _numInliers(0), _numCorresps(0)
{
    ui->setupUi(this);
	ui->btStop->setVisible(false);

	connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->btPlay, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->btStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->btForwardOne, SIGNAL(clicked()), this, SLOT(next()));
	connect(ui->widget, SIGNAL(nearPoint(int)), this, SLOT(nearPoint(int)));
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
	QString folder = QFileDialog::getExistingDirectory(this, "Load Video",QString());
	if(!folder.isNull())
	{
		_folder = folder;
		loadInfo((folder+"\\_MoveInfo.txt").toStdString());
		loadHoms((folder+"\\_Homs.txt").toStdString());
		_decoder.load(folder.toStdString(), _numFrames);

		_currentFrame = -1;
		ui->widget->setFrame1(&_frame1);
		ui->widget->setFrame2(&_frame2);
		ui->widget->setSifts1(&_sifts1);
		ui->widget->setSifts2(&_sifts2);
		ui->widget->setCurrentFrame(&_currentFrame);
				
		next();
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

//  Display next frame
void MainWindow::next()
{
	if(_currentFrame != -1) 
	{
		_frame1 = _frame2;
	}

	if(_decoder.getNextFrame(&_frame2))
	{
		++_currentFrame;

		// Load sift points
		string file = _folder.toStdString();
		char buf[20];
		sprintf(buf, "\\state_%d.txt", _currentFrame-1);
		file += buf;

		ifstream input(file.c_str());
		int n;
		input >> n;
		_sifts1.clear();
		_sifts2.clear();
		for(int i = 0; i < n; ++i)
		{
			double x1, x2, y1, y2;
			input >> x1 >> x2 >> y1 >> y2;

			_sifts1.push_back(RX::vec3(x1, y1, 1));
			_sifts2.push_back(RX::vec3(x2, y2, 1));
		}
		ui->widget->setSifts1(&_sifts1);
		ui->widget->setSifts2(&_sifts2);
		_numCorresps = _sifts1.size();

		// Load inliers
		//string file2 = _folder.toStdString();
		//char buf2[20];
		//sprintf(buf2, "\\inliers_%d.txt", _currentFrame-1);
		//file2 += buf2;
		//
		//ifstream input2(file2.c_str());
		//_inliers.clear();
		//input2 >> _numInliers;
		//for(int i = 0; i < n; ++i)
		//{
		//	bool b;
		//	input2 >> b;
		//	_inliers.push_back(b); 
		//}
		//ui->widget->setInliers(&_inliers);
		//
		//char buf3[20];
		//sprintf(buf3, "%d", _numInliers);
		//ui->textInliers->setText(buf3);
		//sprintf(buf3, "%d", _numCorresps);
		//ui->textPCorresp->setText(buf3);
		//
		//// homography
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(0, 0));
		//ui->m11->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(0, 1));
		//ui->m12->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(0, 2));
		//ui->m13->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(1, 0));
		//ui->m21->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(1, 1));
		//ui->m22->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(1, 2));
		//ui->m23->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(2, 0));
		//ui->m31->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(2, 1));
		//ui->m32->setText(buf3);
		//sprintf(buf3, "%.3f", _homographies[_currentFrame].at(2, 2));
		//ui->m33->setText(buf3);
	}

}

void MainWindow::nearPoint(int i)
{
	char buf[20];
	sprintf(buf, "%.3f", _sifts1[i].x);
	ui->textPx->setText(buf);
	sprintf(buf, "%.3f", _sifts1[i].y);
	ui->textPy->setText(buf);
	sprintf(buf, "%.3f", _sifts2[i].x);
	ui->textPCx->setText(buf);
	sprintf(buf, "%.3f", _sifts2[i].y);
	ui->textPCy->setText(buf);
	sprintf(buf, "%d", i);
	ui->textPid->setText(buf);
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
