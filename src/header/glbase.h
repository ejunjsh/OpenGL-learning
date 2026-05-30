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


class GLBase : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLBase(QWidget *parent = nullptr);
    ~GLBase() override;
    void setName(const QString &name);
    float elapsedTime() const { return m_elapsedTime; }
    void stopRendering();
    void startRendering();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    unsigned int loadTexture(const QString &path, bool flipVertically = false, bool flipHorizontally = false);
    void resizeEvent(QResizeEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    virtual void updateCamera(float dt);

private:
    void toggleMenu();

protected:
    QOpenGLShaderProgram m_program;

    float m_elapsedTime = 0.0f;

private:
    QTimer m_frameTimer;
    QElapsedTimer m_timer;
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
    QLabel *m_helpLabel = nullptr;
    bool m_helpVisible = false;
    void toggleHelp();

protected:
    QFrame *getMenuPanel() const { return m_menuPanel; }
    virtual QString getHelpText() const;
};
