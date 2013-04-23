#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <RX/mat3.h>
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

public slots:
    void load();
	void loadWithMap();
	void exportNotes();
    void play();
	void stop();
	void next();
	void previous();
	void quit();
	void goToFrame(int frame);
	void moveContent();

	void horizontalScroll(int value);
	void verticalScroll(int value);

private:
	void loadBoards(std::string filename);
	void loadRegions(std::string filename);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
