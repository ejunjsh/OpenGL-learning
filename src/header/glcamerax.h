#pragma once

#include "glcamerabase.h"
#include <QVector3D>
#include <QSpinBox>

class GLCameraX : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLCameraX(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene(int cubeCount);
    QVector<QVector3D> m_rotationDirections;  // 每个立方体的固定旋转方向
    QSpinBox *m_cubeCountSpinBox;
};
