#include "mainwindow.h"

#include <QApplication>
#include <Qylon.h>

MainWindow* window = nullptr;
void setDebugMessage(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    window->setMessage(msg+"\n");
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(Resources);

    Qylon::Qylon q;
    auto vTools = q.addVTools();

    MainWindow w;
    window = &w;
    // qInstallMessageHandler(setDebugMessage);

    w.setVTools(vTools);
    w.resize(1000, 800);


    w.show();


    return a.exec();
}
