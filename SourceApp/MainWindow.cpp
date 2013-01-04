#include <QProgressDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSound>
#include "MainWindow.h"
#include "Globals.h"
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
		//QProgressDialog progress("Processing...", "Loading Video", 0, 100, this);
		//progress.setWindowModality(Qt::WindowModal);

		info.load(folder.toStdString());
		//progress.setValue(30);
		resources.load(folder.toStdString());
		//progress.setValue(100);
	}
}

//  Display next frame
void MainWindow::next()
{

}

void MainWindow::play()
{
	ui->wdgVideo->play(resources.audioSource());
	ui->btStop->setVisible(true);
	ui->btPlay->setVisible(false);
}

void MainWindow::stop()
{
	ui->wdgVideo->pause();
	ui->btStop->setVisible(false);
	ui->btPlay->setVisible(true);
}
