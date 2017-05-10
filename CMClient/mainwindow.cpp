#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    apiClient = new CMApiClient(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnCall_clicked()
{
  apiClient->startCall(0);
  ui->btnEndCall->setEnabled(true);
  ui->btnCall->setEnabled(false);
}

void MainWindow::on_btnEndCall_clicked()
{
  apiClient->endCall();
  ui->btnCall->setEnabled(true);
  ui->btnEndCall->setEnabled(false);
}

void MainWindow::on_connectBtn_clicked()
{
  apiClient->connectToHost(ui->textBoxHost->text(), ui->textBoxPort->text().toInt());
}
