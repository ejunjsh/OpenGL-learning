#pragma once

#include "glcamerawidget.h"

class GLTransformWidget : public GLCameraWidget
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
