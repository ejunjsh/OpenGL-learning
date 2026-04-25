#pragma once

#include <QPushButton>

class ActionButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ActionButton(const QString &text, QWidget *parent = nullptr);
};
