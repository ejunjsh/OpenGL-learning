#pragma once

#include "glcamerabase.h"
#include "model.h"

#include <QOpenGLShaderProgram>

class GLModelLoading : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLModelLoading(QWidget *parent = nullptr);
    ~GLModelLoading() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    QOpenGLShaderProgram m_program;
    std::unique_ptr<Model> m_model;
};
