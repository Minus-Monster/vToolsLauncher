#include "mainwindow.h"

#include <QApplication>
#include <Qylon.h>
#include "Modules/GraphicsWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    Qylon::GraphicsWidget *widget = new Qylon::GraphicsWidget;
    widget->initialize();

    Qylon::Qylon q;
    auto vTools = q.addVTools();

    MainWindow w;
    w.setWidget(widget);
    w.setVTools(vTools);
    w.resize(1000, 800);
    w.show();


    return a.exec();
}
