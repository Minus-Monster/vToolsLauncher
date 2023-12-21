#include "mainwindow.h"

#include <QApplication>
#include <Qylon.h>
#include "Modules/GraphicsWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    Qylon::GraphicsWidget *widget = new Qylon::GraphicsWidget;
    widget->initialize();
    widget->setSizeIncrement(400, 300);
    widget->show();

    Qylon::Qylon q;
    auto val = q.addVTools();
    QObject::connect(val, &Qylon::vTools::finished, [=](){
        qDebug() << "get image";
        widget->setImage(val->getImage());
    });

    val->loadRecipe("/home/minwoo/Downloads/vitdor.precipe");
    val->startRecipe();


    return a.exec();
}
