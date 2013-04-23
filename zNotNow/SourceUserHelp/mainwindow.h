#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <RX/VideoDecoder.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void saveInfo();

public slots:
    void load();
    void play();
	void stop();
	void next();
	void previous();
	void quit();
	void selectBoardAndPoint(int board, int point);
	void startMovement();
	void endMovement();

	void setScale(double scale);
private:
    Ui::MainWindow *ui;

	QTimer _timer;
	QImage _frame;
	int _currentFrame;

	RX::VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
