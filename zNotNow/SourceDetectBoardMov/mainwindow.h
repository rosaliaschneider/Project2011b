#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <RX/VideoDecoder.h>
#include <QMainWindow>
#include <QTimer>
#include "Movement.h"

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

public slots:
    void load();
	void loadBoards();
	void saveBoards();
    void play();
	void stop();
	void next();
	void quit();

protected:
	void keyPressEvent(QKeyEvent *ev);

private:
    Ui::MainWindow *ui;

	QString _folder;
	QTimer _timer;
	QImage _frame;
	int _numFrames;
	int _currentFrame;
	int _dimX, _dimY;
	
	bool _stopped, _fixing;
	int _nextMovement;
	std::vector<Movement> _movements;
	std::vector< std::vector<BBox> > _boards;

	RX::VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
