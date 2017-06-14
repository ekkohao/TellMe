#include "mainwindow.h"
#include <QApplication>
#include <QFile>
//#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFile qssFile(":/style/main.qss");

    qssFile.open(QFile::ReadOnly);
    QString qss;
    if (qssFile.isOpen()) {
//      qDebug() << "open success";
        qss = qssFile.readAll();
//      qDebug() << qss;
        w.setStyleSheet(qss);
        qssFile.close();
    }

    w.show();

    return a.exec();
}
