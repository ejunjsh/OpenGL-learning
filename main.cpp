#include <QApplication>
#include <QSurfaceFormat>
#include <QWidget>
#include <QHBoxLayout>
#include "glwidget.h"
#include "glwidgetex.h"
#include "panel.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(fmt);
    QApplication app(argc, argv);

    QWidget mainWindow;
    mainWindow.setWindowTitle("opengl-learning");
    mainWindow.setFixedSize(960, 640);

    QHBoxLayout *layout = new QHBoxLayout(&mainWindow);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    GLWidget *glWidget = new GLWidget;
    glWidget->setFixedSize(640, 640);
    layout->addWidget(glWidget);

    Panel *panel = new Panel;
    layout->addWidget(panel, 1);

    QObject::connect(panel, &Panel::reloadRequested, [&](GLWidget *newWidget) {
        delete glWidget;
        glWidget = newWidget;
        layout->insertWidget(0, glWidget);
        glWidget->setFocus();
        glWidget->setFixedSize(640, 640);
    });

    mainWindow.show();
    glWidget->setFocus();
    return app.exec();
}