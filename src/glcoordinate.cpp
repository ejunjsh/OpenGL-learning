#include "header/glcoordinate.h"
#include <QMatrix4x4>

GLCoordinate::GLCoordinate(QWidget *parent)
    : GLBase(parent)
{
    setName("GLCoordinate");
}

void GLCoordinate::initializeGL()
{
    GLBase::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/triangle.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/triangle.frag"))
    {
        qFatal("Failed to compile shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link shader program");
    }
}

void GLCoordinate::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
