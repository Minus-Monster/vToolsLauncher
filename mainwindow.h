#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Modules/GraphicsWidget.h"
#include "Processing/vTools.h"
#include "Processing/SerialCommunication.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setWidget(QWidget *wg);
    void setVTools(Qylon::vTools *vT);

public slots:
    void setMessage(QString message);

private:
    Ui::MainWindow *ui;
    Qylon::vTools *vTools;
    Qylon::GraphicsWidget *widget;
    Qylon::SerialCommunication *serial = nullptr;
};
#endif // MAINWINDOW_H
