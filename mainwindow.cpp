#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QTime>
#include <QGuiApplication>
#include <QStyleHints>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget = ui->widget;
    widget->initialize();

    setWindowTitle("Basler vLauncher");
    setWindowIcon(QIcon(":/Resources/Icon.png"));

    console = new Qylon::Console(this);
    ui->dockWidget_results->setWidget(console);

    ui->actionResults->setChecked(true);
    connect(ui->actionResults, &QAction::triggered, this, [=](bool on){
        ui->dockWidget_results->setHidden(!on);
    });
    connect(ui->dockWidget_results, &QDockWidget::visibilityChanged, this, [=](bool on){
        ui->actionResults->setChecked(on);
    });

    connect(ui->actionLoad_Recipe, &QAction::triggered, this, [=](){
        QMessageBox *box = new QMessageBox(this);
        box->setWindowTitle("Basler vLauncher");
        box->setIcon(QMessageBox::Information);
        box->setText("Loading a recipe..");
        box->setStandardButtons(QMessageBox::NoButton);

        auto file = QFileDialog::getOpenFileName(this, "Load a pylon recipe", QDir::currentPath(), "*.precipe" );
        if(file.isEmpty()) return;

        box->show();
        QEventLoop loop;
        QTimer::singleShot(500, &loop, &QEventLoop::quit);
        loop.exec();

        auto val = vTools->loadRecipe(file);
        if(val){
            box->setStandardButtons(QMessageBox::Ok);
            box->setText("Recipe is successfully loaded.");
            this->ui->actionContinuous->setEnabled(true);
            this->ui->actionSerial->setEnabled(true);
            this->ui->actionRecipeConfiguration->setEnabled(true);
        }else{
            box->setStandardButtons(QMessageBox::Ok);
            box->setIcon(QMessageBox::Critical);
            box->setText("Recipe loading is failed. \n" + vTools->getLastError());
        }
        widget->setCrossHair(false);
        widget->reset();
    });
    connect(ui->actionContinuous, &QAction::triggered, this, [=](){
        widget->setFPSEnable(true);
        this->ui->actionContinuous->setEnabled(false);
        this->ui->actionStop->setEnabled(true);
        vTools->startRecipe();
    });
    connect(ui->actionStop, &QAction::triggered, this, [=](){
        vTools->stopRecipe();
        this->ui->actionContinuous->setEnabled(true);
        this->ui->actionStop->setEnabled(false);
        widget->setFPSEnable(false);
    });
    connect(ui->actionRecipeConfiguration, &QAction::triggered, this, [=](){
        vTools->getWidget()->show();
    });
    connect(ui->actionSerial, &QAction::triggered, this, [=](){
        if(this->serial == nullptr) serial = new Qylon::SerialCommunication;

        serial->getWidget()->setParent(this, Qt::WindowType::Window);
        serial->getWidget()->show();
    });
    connect(ui->actionAbout_vLauncher, &QAction::triggered, this, [=](){
        QMessageBox about(this);
        about.setWindowTitle("Basler vLauncher");
        about.setTextFormat(Qt::RichText);
        about.setTextInteractionFlags(Qt::TextBrowserInteraction);
        about.setText("This program is designed to execute recipes from pylon vTools. <br>"
                      "All rights to this program are reserved by Basler AG and Basler Korea. <br><br>"
                      "By using this program, users are deemed to agree to the software usage terms of Basler AG and Basler Korea. "
                      "Unauthorized reproduction, distribution, modification, or commercial use is strictly prohibited. <br><br>"
                      "Please feel free to send any comments or suggestions regarding this program to Minu at <a href='mailto:minu.park@baslerweb.com'>minu.park@baslerweb.com</a>.<br><br>"
                      "Your feedback will help us provide a better program.");
        about.exec();
    });
    connect(ui->actionAbout_Qt, &QAction::triggered, this, [=](){
        QMessageBox::aboutQt(this, "Basler vLauncher");
    });


#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
    if(scheme == Qt::ColorScheme::Light){
        ui->menubar->setStyleSheet("QMenuBar{ background-color:white;}"
                                   "QMenuBar::item{ background-color:white;}"
                                   );
        ui->toolBar->setStyleSheet("QToolBar{background-color: rgb(255, 255, 255);}"
                                   "QToolButton{margin-left:5px;}");
    }
#else
    ui->menubar->setStyleSheet("QMenuBar{ background-color:white;}"
                               "QMenuBar::item{ background-color:white;}"
                               );
    ui->toolBar->setStyleSheet("QToolBar{background-color: rgb(255, 255, 255);}"
                               "QToolButton{margin-left:5px;}");
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setVTools(Qylon::vTools *vT)
{
    vTools = vT;
    QObject::connect(vTools, &Qylon::vTools::finishedProcessing, this, [this](){
        QMutexLocker locker(&mutex);
        widget->clear();
        auto result = vTools->getResult();

        auto images = result.images;
        auto items = result.items;
        auto strings = result.strings;

        if(!images.isEmpty()){
            auto outImg = vTools->getSelectedImage(images);
            widget->setImage(outImg);
        }
        if(!items.isEmpty()){
            for(int i=0; i<items.size(); ++i){
                auto item = items.at(i).second;
                QGraphicsItem* currentItem = const_cast<QGraphicsItem*>(item);
                widget->drawGraphicsItem(currentItem);
            }
        }
        if(!strings.isEmpty()){
            setMessage(vTools->getParseredString(strings));
        }
    });
}

void MainWindow::setMessage(QString message)
{
    if(message.isEmpty()) return;

    QString output("[" + QTime::currentTime().toString() + "] ");
    console->append(output+message);

    if(serial != nullptr){
        if(serial->isOpen()) serial->sendData(message);
    }
}

