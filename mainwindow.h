#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "Modules/Console.h"
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
    void setVTools(Qylon::vTools *vT);

public slots:
    void setMessage(QString message);

private:
    Ui::MainWindow *ui;
    QSplitter *splitter;
    Qylon::vTools *vTools;
    QList<Qylon::GraphicsWidget*> widgets;
    Qylon::GraphicsWidget* currentWidget;

    Qylon::Console *console;
    Qylon::SerialCommunication *serial = nullptr;

    QMutex mutex;
};
#endif // MAINWINDOW_H
