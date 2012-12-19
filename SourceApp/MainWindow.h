#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <phonon/mediaobject.h>
#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include "Board.h"
#include "Region.h"
#include "Interval.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void load();
    void play();
	void stop();
	void next();
	void quit();

private:
	void loadBoards(std::string filename);
	void loadRegions(std::string filename);
	void loadIntervals(std::string filename);
	void loadFinalFrame(std::string filename);

    Ui::MainWindow *ui;

	Phonon::MediaObject *_media;

	QImage _frame;
	std::vector<Board> _finalBoards;
	std::vector<Region> _regions;
};

#endif // MAINWINDOW_H
