#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class GLMultipleLights : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLMultipleLights(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupMenu();
    void drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view,
                       const QVector3D &position, const QVector3D &color);

    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    GLuint m_diffuseMap = 0;
    GLuint m_specularMap = 0;

    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_lightProgram;

    int m_sceneIndex = 0;  // 0=Default, 1=Desert, 2=Factory, 3=Horror, 4=Biochemical Lab
};
