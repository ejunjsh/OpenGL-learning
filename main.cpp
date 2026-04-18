#include <QApplication>
#include <QSurfaceFormat>
#include "glwidget.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(fmt);
    QApplication app(argc, argv);
    GLWidget w;
    w.resize(960, 640);
    w.show();
    w.setFocus();
    return app.exec();
}