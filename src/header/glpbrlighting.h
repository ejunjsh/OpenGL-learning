#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <memory>

class Mesh;

class GLPbrLighting : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLPbrLighting(QWidget *parent = nullptr);
    ~GLPbrLighting() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    std::unique_ptr<Mesh> m_sphere;

    QOpenGLShaderProgram m_program;
};
