#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class GLLightCasters : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLLightCasters(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupMenu();
    void drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view);

    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    GLuint m_diffuseMap = 0;
    GLuint m_specularMap = 0;

    QOpenGLShaderProgram m_program;        // directional light
    QOpenGLShaderProgram m_pointProgram;   // point light
    QOpenGLShaderProgram m_spotProgram;       // spot light (hard edge)
    QOpenGLShaderProgram m_spotSoftProgram;   // spot light (soft edge)
    QOpenGLShaderProgram m_lightProgram;      // light cube

    QVector3D m_lightPos = QVector3D(1.2f, 1.0f, 2.0f);

    int m_sceneIndex = 0;  // 0=directional, 1=point, 2=spot, 3=spot_soft
};
