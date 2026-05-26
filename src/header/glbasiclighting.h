#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class GLBasicLighting : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLBasicLighting(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupMenu();
    void drawDiffuse(const QMatrix4x4 &projection, const QMatrix4x4 &view);
    void drawSpecular(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &viewPos);
    void drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view);

private:
    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    QOpenGLShaderProgram m_lightProgram;
    QOpenGLShaderProgram m_specularProgram;
    QVector3D m_lightPos = QVector3D(1.2f, 1.0f, 2.0f);

    // 场景索引: 0 = Diffuse, 1 = Specular
    int m_sceneIndex = 0;
};
