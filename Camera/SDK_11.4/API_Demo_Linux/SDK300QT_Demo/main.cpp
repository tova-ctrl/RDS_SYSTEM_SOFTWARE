#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    std::cout << "STK API Demo" << std::endl;

    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion")); // set it to look the same across platforms.

    MainWindow w;
    w.show();

    // Execute the application
    return a.exec();
}
