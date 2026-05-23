#pragma once

#include <QWidget>

class GLBase;

class Panel : public QWidget
{
    Q_OBJECT
public:
    explicit Panel(QWidget *parent = nullptr);
    void triggerSignal(GLBase *widget);

signals:
    void reloadRequested(GLBase *widget);

private:
    void setupUI();
};
