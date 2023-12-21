#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    auto graphics = new Qylon::GraphicsWidget;
//    graphics->initialize();
//    graphics->show();
    widget = new Qylon::GraphicsWidget;
    widget->initialize();
    widget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getImage()
{

}

