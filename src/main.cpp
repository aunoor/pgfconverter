#include <QtGui>
#include <QObject>
#include "mainwindow.h"
#include "globaldefs.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationVersion(VERSION);
    a.setApplicationName("PGFConverter");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    MainWindow w;
    w.show();

    return a.exec();
}
