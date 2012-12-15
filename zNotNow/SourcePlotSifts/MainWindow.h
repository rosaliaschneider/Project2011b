#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <RX/mat3.h>
#include <QMainWindow>
#include <QTimer>
#include "BBox.h"
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

	void loadInfo(std::string filename);
	void loadHoms(std::string filename);
	void loadSifts(std::string filename);
	
public slots:
    void load();
    void play();
	void stop();
	void next();
	void quit();
	
	void nearPoint(int i);

private:
    Ui::MainWindow *ui;
	
	QString _folder;
	QTimer _timer;
	QImage _frame1, _frame2;
	vector<RX::mat3> _homographies;
	vector<RX::vec3> _sifts1, _sifts2;
	vector<bool> _inliers;

	bool _showMatch;
	int _numInliers, _numCorresps;

	int _currentFrame, _numFrames;
	VideoDecoder _decoder;
};

#endif // MAINWINDOW_H
