#pragma once

#include <QPushButton>
#include "glwidget.h"

class ActionButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ActionButton(const QString &text, QWidget *parent = nullptr, GLWidget *glWidget = nullptr);

signals:
    void reloadRequested(GLWidget *widget);

private:
    GLWidget *m_glWidget = nullptr;
};