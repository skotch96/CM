#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  mServer = new CMServer(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{
  mServer->start(ui->lineEdit->text(), ui->lineEdit_2->text().toInt());
  ui->pushButton->setEnabled(false);
}
