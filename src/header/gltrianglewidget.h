#pragma once

#include "glcamerawidget.h"

class GLTriangleWidget : public GLCameraWidget
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
