#pragma once

#include <QWidget>

class GLWidget;

class Panel : public QWidget
{
    Q_OBJECT
public:
    explicit Panel(QWidget *parent = nullptr);
    void triggerSignal(GLWidget *widget);

signals:
    void reloadRequested(GLWidget *widget);

private:
    void setupUI();
};
