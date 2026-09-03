#include <QApplication>
#include "mainwindow.h"
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Prime Number Tutor");
    QApplication::setOrganizationName("we6jbo");
    MainWindow w; w.show();
    return app.exec();
}
