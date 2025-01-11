// Github Repository : https://github.com/kianmajl/Library
// by: Kian Majlessi

#include "src/authentication.h"


#include <QApplication>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << Q_FUNC_INFO << QDir::currentPath();
    QDir::setCurrent("C:/Users/Imran/Desktop/LibraryCurrent/");

    QApplication a(argc, argv);
    authentication auth;

    auth.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    auth.show();

    return a.exec();
}
