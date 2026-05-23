#pragma once

#include "glbase.h"
#include "camera.h"
#include <QSet>
#include <QElapsedTimer>
#include <QTimer>

class GLCameraBase : public GLBase
{
    Q_OBJECT
public:
    explicit GLCameraBase(QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    virtual QString getHelpText() const override;

protected:
    void updateCamera(float dt) override;

protected:
    std::unique_ptr<Camera> m_camera;

private:
    void initCameraMembers();
    bool m_firstMouse = true;
    float m_lastX = 0.0f;
    float m_lastY = 0.0f;

    bool m_mousePressed = false;
    QSet<int> m_keys;
};
