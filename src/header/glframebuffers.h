#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class QCheckBox;

class GLFramebuffers : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLFramebuffers(QWidget *parent = nullptr);
    ~GLFramebuffers() override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void setupMenu();
    void setupFramebuffer(int w, int h);
    void setupMirrorFramebuffer(int w, int h);
    void drawCube(const QVector3D &pos);
    void drawScene(const QMatrix4x4 &projection, const QMatrix4x4 &view);

    // ---- 场景几何及纹理 ----
    GLuint m_cubeVAO = 0, m_cubeVBO = 0;
    GLuint m_planeVAO = 0, m_planeVBO = 0;
    GLuint m_cubeTexture = 0;
    GLuint m_floorTexture = 0;

    QOpenGLShaderProgram *m_sceneProgram = nullptr;

    // ---- 主 Framebuffer ----
    //   FBO + 颜色纹理附件 (可被 shader 采样) + 深度/模板 RBO (只写)
    GLuint m_fbo = 0;
    GLuint m_texColorBuffer = 0;
    GLuint m_rbo = 0;
    int m_fboWidth = 0;   // 当前 FBO 宽度
    int m_fboHeight = 0;  // 当前 FBO 高度

    // ---- 后视镜 Framebuffer ----
    GLuint m_mirrorFbo = 0;
    GLuint m_mirrorTexColor = 0;
    GLuint m_mirrorRbo = 0;
    int m_mirrorWidth = 0;
    int m_mirrorHeight = 0;

    // ---- 屏幕四边形 ----
    GLuint m_quadVAO = 0, m_quadVBO = 0;
    QOpenGLShaderProgram *m_screenProgram = nullptr;

    // ---- 后视镜四边形 ----
    GLuint m_mirrorQuadVAO = 0, m_mirrorQuadVBO = 0;

    // ---- UI ----
    QCheckBox *m_effectNormal    = nullptr;
    QCheckBox *m_effectInvert    = nullptr;
    QCheckBox *m_effectSimpleGray = nullptr;
    QCheckBox *m_effectWeightedGray = nullptr;
    QCheckBox *m_effectSharpen   = nullptr;
    QCheckBox *m_effectBlur      = nullptr;
    QCheckBox *m_effectEdge      = nullptr;
    QCheckBox *m_mirrorToggle    = nullptr;

    int m_currentEffect = 0;
    bool m_showMirror = false;
};
