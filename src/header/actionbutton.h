#pragma once

#include <QPushButton>
#include <functional>
#include "glbase.h"

class ActionButton : public QPushButton
{
    Q_OBJECT
public:
    using Factory = std::function<GLBase*()>;
    explicit ActionButton(const QString &text, QWidget *parent = nullptr, Factory factory = nullptr);

signals:
    void reloadRequested(GLBase *widget);

private:
    Factory m_factory;
    GLBase *m_glWidget = nullptr;
};
