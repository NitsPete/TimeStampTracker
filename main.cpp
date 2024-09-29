#include "mainwindow.h"

#include <QApplication>
#include <QCursor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    QPixmap pixmap(":/images/mouse-cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(64, 64);
    QCursor customCursor(scaledPixmap);
    w.setCursor(customCursor);

    w.showFullScreen();
    return a.exec();
}
