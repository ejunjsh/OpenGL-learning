#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class GLLightingMaps : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLLightingMaps(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupMenu();
    void drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view);

private:
    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    GLuint m_diffuseMap = 0;

    QOpenGLShaderProgram m_lightingProgram;
    QOpenGLShaderProgram m_lightProgram;

    QVector3D m_lightPos = QVector3D(1.2f, 1.0f, 2.0f);
};
