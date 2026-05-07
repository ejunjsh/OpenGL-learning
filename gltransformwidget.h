#pragma once

#include "glwidget.h"

class GLTransformWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit GLTransformWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene();
};
