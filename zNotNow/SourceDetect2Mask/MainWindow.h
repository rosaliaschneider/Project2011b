#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <vector>
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
	void loadBoards(std::string filename);
	void loadHomographies(std::string filename);

public slots:
    void load();
    void play();
	void stop();
	void next();
	void quit();

private:
    Ui::MainWindow *ui;
	
	QTimer _timer;
	QString _folder;
	QImage _frame, _replace;
	int _currentFrame, _numFrames;

	RX::VideoDecoder _decoderNormal, _decoderAlpha;

	std::vector< std::vector<BBox> > _boards;
};

#endif // MAINWINDOW_H
