#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>

class QCheckBox;

class GLBlending : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLBlending(QWidget *parent = nullptr);
    ~GLBlending() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void onGrassToggled(bool checked);
    void onWindowToggled(bool checked);
    void onNoDiscardToggled(bool checked);
    void onNoSortToggled(bool checked);

private:
    void setupMenu();
    void drawCube(const QMatrix4x4 &projection, const QMatrix4x4 &view,
                  const QVector3D &pos);

    GLuint m_cubeVAO = 0, m_cubeVBO = 0;
    GLuint m_planeVAO = 0, m_planeVBO = 0;
    GLuint m_quadVAO = 0, m_quadVBO = 0;

    GLuint m_cubeTexture = 0;
    GLuint m_floorTexture = 0;
    GLuint m_grassTexture = 0;
    GLuint m_windowTexture = 0;

    // program 0: m_program（父类）— 立方体 + 地板
    // program 1: discard — 草（二进制透明）
    // program 2: noDiscard — 草（不 discard，显示全部）
    // program 3: window — 半透明窗口（混合）
    QOpenGLShaderProgram *m_discardProgram = nullptr;
    QOpenGLShaderProgram *m_noDiscardProgram = nullptr;
    QOpenGLShaderProgram *m_windowProgram = nullptr;

    QCheckBox *m_grassCheck = nullptr;
    QCheckBox *m_windowCheck = nullptr;
    QCheckBox *m_noDiscardCheck = nullptr;
    QCheckBox *m_noSortCheck = nullptr;
    bool m_showGrass = true;
    bool m_showWindows = false;
    bool m_noDiscard = false;
    bool m_noSort = false;

    // 草的位置
    std::vector<QVector3D> m_grassPositions;
    // 窗口位置
    std::vector<QVector3D> m_windowPositions;
};
