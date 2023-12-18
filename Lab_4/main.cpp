#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Базовые растровые алгоритмы");
    w.resize(900,900);
    w.show();
    return a.exec();
}
