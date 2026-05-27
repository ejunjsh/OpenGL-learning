#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>

class GLLightCastersDirectional : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLLightCastersDirectional(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    GLuint m_cubeVAO = 0;
    GLuint m_vbo = 0;
    GLuint m_diffuseMap = 0;
    GLuint m_specularMap = 0;

    QOpenGLShaderProgram m_program;
};
