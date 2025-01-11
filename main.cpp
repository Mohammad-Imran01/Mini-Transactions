#include "src/authentication.h"


#include <QApplication>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << Q_FUNC_INFO << QDir::currentPath();

    QApplication a(argc, argv);
    authentication auth;

    auth.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    auth.show();

    return a.exec();
}
