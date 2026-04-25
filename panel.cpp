#include "panel.h"
#include "actionbutton.h"
#include "glwidget.h"
#include "glwidgetex.h"
#include <QVBoxLayout>

Panel::Panel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Panel::setupUI()
{
    setStyleSheet("background-color: #2b2b2b;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    ActionButton *loadBtn = new ActionButton("加载 GLWidget");
    ActionButton *loadExBtn = new ActionButton("加载 GLWidgetEx");

    connect(loadBtn, &QPushButton::clicked, [this]() {
        GLWidget *newWidget = new GLWidget;
        emit reloadRequested(newWidget);
    });

    connect(loadExBtn, &QPushButton::clicked, [this]() {
        GLWidgetEx *newWidget = new GLWidgetEx;
        emit reloadRequested(newWidget);
    });

    layout->addWidget(loadBtn);
    layout->addWidget(loadExBtn);
    layout->addStretch();
}
