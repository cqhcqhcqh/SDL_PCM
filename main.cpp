#include "mainwindow.h"

#include <QApplication>

#include <QDebug>
extern "C" {
   #include <SDL2/SDL.h>
}
int main(int argc, char *argv[])
{
    SDL_version version;
    SDL_GetVersion(&version);
    qDebug() << "SDL Version: Major: " << version.major
             << "min: " << version.minor
             << "patch: " << version.patch;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
