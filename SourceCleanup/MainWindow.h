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
	void quit();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
