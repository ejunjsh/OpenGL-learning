#include "panel.h"
#include "actionbutton.h"
#include "gltrianglewidget.h"
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

    ActionButton *loadBtn = new ActionButton("hello triangle", this, new GLTriangleWidget());
    ActionButton *loadExBtn = new ActionButton("load GLWidgetEx", this, new GLTriangleWidget());

    layout->addWidget(loadBtn);
    layout->addWidget(loadExBtn);
    layout->addStretch();

    QTimer::singleShot(0, loadBtn, &QPushButton::click);
}
