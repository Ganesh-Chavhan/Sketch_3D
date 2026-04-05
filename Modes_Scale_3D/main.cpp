
#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include <QSurfaceFormat>

int main(int argc, char** argv)
{
    // Request OpenGL 2.1 (compatible with most systems)
    QSurfaceFormat fmt;
    fmt.setVersion(2, 1);
    fmt.setProfile(QSurfaceFormat::NoProfile);
    fmt.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setStyle("Fusion");

    MainWindow w;
    w.show();

    return app.exec();
}