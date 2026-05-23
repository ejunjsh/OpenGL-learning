#include <QApplication>
#include <QSurfaceFormat>
#include <QWidget>
#include <QHBoxLayout>
#include "header/glbase.h"
#include "header/panel.h"

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
    mainWindow.setWindowTitle("OpenGL Learning");
    mainWindow.setFixedSize(1200, 640);

    QHBoxLayout *layout = new QHBoxLayout(&mainWindow);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    Panel *panel = new Panel;
    layout->addWidget(panel, 1);

    QObject::connect(panel, &Panel::reloadRequested, [&](GLBase *newWidget) {
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem *item = layout->itemAt(i);
            if (item->widget()) {
                QWidget *widget = item->widget();
                GLBase *glWidget = qobject_cast<GLBase*>(widget);
                if (glWidget && glWidget != newWidget) {
                    layout->removeWidget(glWidget);
                    glWidget->hide();
                }
            }
        }
        layout->insertWidget(0, newWidget);
        newWidget->show();
        newWidget->setFocus();
    });

    mainWindow.show();
    return app.exec();
}
