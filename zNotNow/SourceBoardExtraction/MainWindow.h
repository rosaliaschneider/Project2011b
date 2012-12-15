#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <RX/mat3.h>
#include <QMainWindow>
#include <QTimer>
#include "VideoDecoder.h"
#include "Board.h"
#include "BBox.h"

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
	void loadInfo(std::string filename);
	void loadBoards(std::string filename);
	void loadHomographies(std::string filename);
	void loadPBBox(std::string filename);

    Ui::MainWindow *ui;

	QTimer _timer;
	QImage _frame;
	int _currentFrame, _numFrames;
	vector<RX::mat3> _homographies;
	vector< vector<Board> > _boards;
	vector<BBox> _pbbox;
	vector<Movement> _movement;
	vector<bool> _currMoving;

	VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
