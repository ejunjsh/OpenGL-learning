#pragma once

#include "glbase.h"
#include "camera.h"
#include <QVector3D>
#include <QVector>
#include <QSet>

class GLCamera : public GLBase
{
    Q_OBJECT
public:
    explicit GLCamera(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void updateCamera(float dt) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupMenu();
    void drawScene1();
    void drawScene2();
    void drawScene3();
    void drawScene4();

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_texture1 = 0;
    GLuint m_texture2 = 0;
    QVector<QVector3D> m_cubePositions;

    // 场景索引: 0 = 旋转相机, 1 = WASD 相机, 2 = 鼠标视角+滚轮相机, 3 = Camera 类
    int m_sceneIndex = 0;

    // Camera 类实例 (场景3)
    Camera m_camera;

    // 相机状态 (场景1 & 2 共用)
    QVector3D m_cameraPos   = QVector3D(0.0f, 0.0f,  3.0f);
    QVector3D m_cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
    QVector3D m_cameraUp    = QVector3D(0.0f, 1.0f,  0.0f);
    QSet<int> m_keys;
    float m_lastFrameTime = 0.0f;

    // 鼠标视角控制 (场景2)
    bool m_mousePressed = false;
    bool m_firstMouse = true;
    float m_lastX = 0.0f;
    float m_lastY = 0.0f;
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov = 45.0f;
};
