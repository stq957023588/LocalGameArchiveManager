#include "SqlUtils.h"
#include <QApplication>
#include "MainWindow.h"
#include "ApplicationInitUtils.h"
#include <iostream>
using namespace std;
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    connectSqliteDb();
    init();
    MainWindow w;
    w.show();

    return a.exec();
}