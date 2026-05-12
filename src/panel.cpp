#include "header/panel.h"
#include "header/actionbutton.h"
#include "header/gltrianglewidget.h"
#include "header/gltexturewidget.h"
#include "header/gltransformwidget.h"
#include "header/glcoordinatewidget.h"
#include <QVBoxLayout>
#include <QTimer>

Panel::Panel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Panel::triggerSignal(GLWidget *widget)
{
    emit reloadRequested(widget);
}

void Panel::setupUI()
{
    setStyleSheet("background-color: #2b2b2b;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    ActionButton *triangleBtn = new ActionButton("hello triangle", this, new GLTriangleWidget());
    ActionButton *textureBtn = new ActionButton("hello texture", this, new GLTextureWidget());
    ActionButton *transformBtn = new ActionButton("hello transform", this, new GLTransformWidget());
    ActionButton *coordinateBtn = new ActionButton("hello coordinate", this, new GLCoordinateWidget());

    layout->addWidget(triangleBtn);
    layout->addWidget(textureBtn);
    layout->addWidget(transformBtn);
    layout->addWidget(coordinateBtn);
    layout->addStretch();

    QTimer::singleShot(0, triangleBtn, &QPushButton::click);
}
