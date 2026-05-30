#pragma once

#include "glcamerabase.h"
#include <QVector3D>
#include <QSpinBox>
#include <QOpenGLShaderProgram>

struct CubeData
{
    QVector3D position;
    QVector3D rotation;      // current rotation angles (degrees)
    QVector3D rotationDir;   // fixed rotation direction
};

class GLCameraX : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLCameraX(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene(int cubeCount);

    GLuint m_cubeVAO = 0;
    GLuint m_cubeVBO = 0;
    GLuint m_containerTex = 0;
    GLuint m_faceTex = 0;

    QOpenGLShaderProgram m_program;
    QVector<CubeData> m_cubes;
    QSpinBox *m_cubeCountSpinBox;
};
