#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <RX/mat3.h>
#include "BBox.h"
#include "Board.h"
#include "Interval.h"
#include "VideoDecoder.h"

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
	void loadIntervals(std::string folder);
	void loadPBBox(std::string filename);
	void loadHomographies(std::string filename);
	void loadBackground(std::string filename);
	void loadBackgroundF(std::string filename);

    Ui::MainWindow *ui;

	QSound *_sound;
	QTimer _timer;
	QImage _frame, _background, _backgroundF;
	vector< vector<Interval> > _intervals;

	int _currentFrame, _numFrames;
	vector<RX::mat3> _homographies;
	vector< vector<Board> > _boards;
	vector< BBox > _pbbox;
	
	VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
