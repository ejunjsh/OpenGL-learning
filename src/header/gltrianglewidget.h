#pragma once

#include "glwidget.h"

class GLTriangleWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit GLTriangleWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene();
};
