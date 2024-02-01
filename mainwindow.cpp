#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Basler vLauncher");
    setWindowIcon(QIcon(":/Resources/Icon.png"));

    connect(ui->actionLoad_Recipe, &QAction::triggered, this, [=](){
        auto file = QFileDialog::getOpenFileName(this, "Load a pylon recipe", QDir::currentPath(), "*.precipe" );
        vTools->loadRecipe(file);
    });
    connect(ui->actionContinuous, &QAction::triggered, this, [=](){
        vTools->startRecipe();
    });
    connect(ui->actionStop, &QAction::triggered, this, [=](){
        vTools->stopRecipe();
    });
    connect(ui->actionSerial, &QAction::triggered, this, [=](){
        if(this->serial == nullptr) serial = new Qylon::SerialCommunication;

        serial->getWidget()->show();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setWidget(QWidget *wg)
{
    widget = reinterpret_cast<Qylon::GraphicsWidget*>(wg);
    ui->formLayout->addRow(wg);
}

void MainWindow::setVTools(Qylon::vTools *vT)
{
    vTools = vT;
    QObject::connect(vTools, &Qylon::vTools::finished, this, [this](){
        this->widget->setImage(vTools->getImage());
        this->setMessage(vTools->getOutputText());
    });
}

void MainWindow::setMessage(QString message)
{
    if(message.isEmpty()) return;

    QString output("[" + QTime::currentTime().toString() + "] ");
    ui->textEdit->append(output + message);
    if(serial != nullptr) serial->sendData(message);
}

