#pragma once

#include "glcamerabase.h"

class GLLightCastersDirectional : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLLightCastersDirectional(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
};
