#pragma once

#include <QWidget>

class GLWidget;

class Panel : public QWidget
{
    Q_OBJECT
public:
    explicit Panel(QWidget *parent = nullptr);

signals:
    void reloadRequested(GLWidget *widget);

private:
    void setupUI();
};
