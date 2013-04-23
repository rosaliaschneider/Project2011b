#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <RX/mat3.h>
#include <RX/VideoDecoder.h>
#include <QMainWindow>
#include <QTimer>
#include "Sift.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void updateInterface();

public slots:
    void load();
	void loadImage();
	void loadSifts();
    void quit();

	void play();
	void pause();
	void next();

	void nearPoint(int i);

private:
    Ui::MainWindow *ui;
	
	QString _folder;
	
	bool _showMatch, _usingMap;
	int _currentFrame;

	Sift _sift;
	RX::VideoDecoder _decoder;
	QTimer _timer;
	
};

#endif // MAINWINDOW_H
