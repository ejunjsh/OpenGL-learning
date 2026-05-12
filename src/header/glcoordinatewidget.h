#pragma once

#include "glwidget.h"

class GLCoordinateWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit GLCoordinateWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene();
};
