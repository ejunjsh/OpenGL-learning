#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QSet>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QHideEvent>
#include <QShowEvent>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <memory>

#include "camera.h"
#include "object3d.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;
    void setName(const QString &name);
    float elapsedTime() const { return m_elapsedTime; }
    void stopRendering();
    void startRendering();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    unsigned int loadTexture(const QString &path, bool flipVertically = false, bool flipHorizontally = false);
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void updateCamera(float dt);
    void toggleMenu();

protected:
    QOpenGLShaderProgram m_program;
    std::unique_ptr<Camera> m_camera;
    std::shared_ptr<Object3D> m_rootObject;
    float m_elapsedTime = 0.0f;

private:
    bool m_firstMouse = true;
    float m_lastX = 0.0f;
    float m_lastY = 0.0f;

    bool m_mousePressed = false;
    QSet<int> m_keys;
    QElapsedTimer m_timer;
    QTimer m_frameTimer;
    QLabel *m_fpsLabel;
    QLabel *m_nameLabel;
    int m_frameCount = 0;
    QElapsedTimer m_fpsTimer;

    // 右上角菜单
    QLabel *m_menuButton;
    QFrame *m_menuPanel;
    bool m_menuVisible = false;
    bool m_hasMenuContent = false;

    // 右下角帮助
    QLabel *m_helpButton;
    QFrame *m_helpPanel;
    bool m_helpVisible = false;
    void toggleHelp();

protected:
    QFrame *getMenuPanel() const { return m_menuPanel; }
    virtual QString getHelpText() const;
};