#include <QApplication>
#include <QLabel>
#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow tools;
    tools.setWindowTitle("Tools");
    tools.show();
    return app.exec();
}
