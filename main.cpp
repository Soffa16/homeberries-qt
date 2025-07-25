#include <QApplication>
#include "include/MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow window;
    return a.exec();
}
