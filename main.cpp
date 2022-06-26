#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    //w.resize(600,400);
    w.resize(w.p_background_label->size());

    return a.exec();
}
