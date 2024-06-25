#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QTime>
#include "Processing/ImageTools.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Basler vLauncher");
    setWindowIcon(QIcon(":/Resources/Icon.png"));

    // ui->dockWidget_results->setHidden(true);
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
            box->setText("Recipe is successfully loaded");
            this->ui->actionContinuous->setEnabled(true);
            this->ui->actionSerial->setEnabled(true);
            this->ui->actionRecipeConfiguration->setEnabled(true);
        }else{
            box->setStandardButtons(QMessageBox::Ok);
            box->setText("Recipe loading is failed. Check the resources that are needed to run");
        }
        widget->clear();
        widget->setImage(QImage());
        widget->setLogo(true);

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
        widget->removeAllGraphicsItem();
        if(vTools->getImages().size()!=0){
            auto outImg = vTools->getImage();
            widget->setImage(outImg);
        }
        if(vTools->getItems().size() !=0){
            for(int i=0; i<vTools->getItems().size(); ++i){
                auto item = vTools->getItems().at(i).second;
                widget->drawGraphicsItem(item);
            }
        }

        for(int i=0; i<vTools->getOutputText().size(); ++i){
            setMessage(vTools->getOutputText().at(i));
        }
    });
}

void MainWindow::setMessage(QString message)
{
    if(message.isEmpty()) return;

    QString output("[" + QTime::currentTime().toString() + "] ");
    ui->textEdit->append(output + message);
    ui->textEdit->moveCursor(QTextCursor::End);
    if(serial != nullptr) serial->sendData(message);
}

