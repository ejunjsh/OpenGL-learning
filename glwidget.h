#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram›
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayobject>
#include <QVector3D>
#include <QSet>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

class GLWidget : public QopenGLwidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void updateCameraByKeyboard(float dt);

private:
    QOpenGLShaderProgram _program;
    QopenGLBuffer m_vbo;
    QOpenGLVertexArrayobject m_vao;
    Qvector3D m_cameraPos = QVector3D(0.0f, 0.0f, 3.0f);
    QVector3D m_cameraFront = QVector3D(e.0f, 0.0f, -1.0f);
    QVector3D m_cameraup = QVector3D(0.0f, 1.0f, 0.0f);
    float m yaw = -90.0f;
    float m_pitch = 0.0f;
    float m mousesensitivity = 0.12f;
    float m movespeed = 2.5f;
    bool m_firstMouse = true;

    float m_last = 0.0f;

    float m_lasty = 0.0f;

    bool m_fastMode = false;
    QSet<int> m_keys;
    QElapsedTimer m_timer;
    QTimer m_frameTimer;
};