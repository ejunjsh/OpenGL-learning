#include "gltrianglewidget.h"
#include <QMatrix4x4>
#include "mesh.h"

GLTriangleWidget::GLTriangleWidget(QWidget *parent)
    : GLWidget(parent)
{
}

void GLTriangleWidget::initializeGL()
{
    GLWidget::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/triangle.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/triangle.frag"))
    {
        qFatal("Failed to compile shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link shader program");
    }

    setupScene();
}

void GLTriangleWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("uColor", QVector3D(0.95f, 0.45f, 0.2f));

    if (m_rootObject)
    {
        m_rootObject->draw(m_program);
    }

    m_program.release();
}

void GLTriangleWidget::setupScene()
{
    std::vector<Vertex> vertices = {
        Vertex(QVector3D(0.0f, 0.6f, 0.0f)),
        Vertex(QVector3D(-0.6f, -0.6f, 0.0f)),
        Vertex(QVector3D(0.6f, -0.6f, 0.0f))
    };

    std::vector<unsigned int> indices = {0, 1, 2};

    auto mesh = std::make_shared<Mesh>(vertices, indices);
    auto triangle = std::make_shared<Object3D>("Triangle");
    triangle->addMesh(mesh);
    triangle->setPosition(QVector3D(0.0f, 0.0f, 0.0f));

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(triangle);
}
