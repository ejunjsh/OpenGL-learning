#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class GLColor : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLColor(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    QOpenGLShaderProgram m_lightProgram;
    QVector3D m_lightPos = QVector3D(1.2f, 1.0f, 2.0f);
};
