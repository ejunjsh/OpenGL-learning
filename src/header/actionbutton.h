#pragma once

#include <QPushButton>
#include <functional>
#include "glwidget.h"

class ActionButton : public QPushButton
{
    Q_OBJECT
public:
    using Factory = std::function<GLWidget*()>;
    explicit ActionButton(const QString &text, QWidget *parent = nullptr, Factory factory = nullptr);

signals:
    void reloadRequested(GLWidget *widget);

private:
    Factory m_factory;
    GLWidget *m_glWidget = nullptr;
};