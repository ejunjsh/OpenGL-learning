#pragma once

#include "glcamerawidget.h"

class GLTextureWidget : public GLCameraWidget
{
    Q_OBJECT
public:
    explicit GLTextureWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene();
};
