#pragma once

#include "glwidget.h"
#include <QVector3D>

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
    QVector<QVector3D> m_rotationDirections;  // 每个立方体的固定旋转方向
};
