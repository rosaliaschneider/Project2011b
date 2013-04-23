#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <QMainWindow>
#include <QTimer>
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

	void loadInfo(std::string filename);
	void loadHoms(std::string filename);
	void process();

	void saveBoards(std::string filename);
	void saveGlobalHoms(std::string filename);
	
public slots:
    void load();
    void play();
	void stop();
	void next();
	void quit();

	void setScale(double scale);

	void moveFrame();
	void moveLeft(int pixels);
	void moveRight(int pixels);
	void moveUp(int pixels);
	void moveDown(int pixels);

private:
    Ui::MainWindow *ui;

	QTimer _timer;
	QImage _frame;
	std::string _folder;
	std::vector<RX::mat3> _homographies;
	std::vector<RX::mat3> _globalHomographies;
	std::vector< std::vector<BBox> > _boards;

	std::vector<BBox> _boardPositions;
	std::vector< std::vector<int> > _boardFrames;

	int _startingFrame;
	int _currentFrame, _numFrames;
	int _numBoards;
	RX::VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
