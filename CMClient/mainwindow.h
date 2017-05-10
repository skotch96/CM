#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cmapiclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
  void on_connectBtn_clicked();

  void on_btnCall_clicked();

  void on_btnEndCall_clicked();

private:
    Ui::MainWindow *ui;
    CMApiClient *apiClient;
};

#endif // MAINWINDOW_H
