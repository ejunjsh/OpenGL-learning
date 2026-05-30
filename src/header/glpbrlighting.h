#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

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
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ebo{QOpenGLBuffer::IndexBuffer};
    unsigned int m_indexCount = 0;

    QOpenGLShaderProgram m_program;
};
