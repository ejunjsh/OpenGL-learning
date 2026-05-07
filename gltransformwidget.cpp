#include "gltransformwidget.h"
#include <QMatrix4x4>
#include "mesh.h"

GLTransformWidget::GLTransformWidget(QWidget *parent)
    : GLWidget(parent)
{
    setName("GLTransformWidget");
}

void GLTransformWidget::initializeGL()
{
    GLWidget::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/transform.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/transform.frag"))
    {
        qFatal("Failed to compile transform shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link transform shader program");
    }

    setupScene();
}

void GLTransformWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    if (m_rootObject)
    {
        m_program.setUniformValue("uColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));
        m_rootObject->draw(m_program);
    }

    m_program.release();
}

void GLTransformWidget::setupScene()
{
    std::vector<Vertex> vertices = {
        Vertex(QVector3D(0.5f, 0.5f, 0.0f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D(0.5f, -0.5f, 0.0f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, 0.0f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, 0.5f, 0.0f), QVector2D(0.0f, 1.0f))
    };

    std::vector<unsigned int> indices = {
        0, 1, 3,
        1, 2, 3
    };

    auto mesh = std::make_shared<Mesh>(vertices, indices);

    // 原始位置
    auto cube = std::make_shared<Object3D>("Cube");
    cube->addMesh(mesh);
    cube->setPosition(QVector3D(-1.5f, 0.0f, 0.0f));

    // 平移
    auto translated = std::make_shared<Object3D>("Translated");
    translated->addMesh(mesh);
    translated->setPosition(QVector3D(1.5f, 0.0f, 0.0f));

    // 旋转
    auto rotated = std::make_shared<Object3D>("Rotated");
    rotated->addMesh(mesh);
    rotated->setPosition(QVector3D(0.0f, -1.5f, 0.0f));
    rotated->setRotation(QVector3D(0.0f, 0.0f, 45.0f));

    // 缩放
    auto scaled = std::make_shared<Object3D>("Scaled");
    scaled->addMesh(mesh);
    scaled->setPosition(QVector3D(0.0f, 1.5f, 0.0f));
    scaled->setScale(QVector3D(1.5f, 1.5f, 1.0f));

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(cube);
    m_rootObject->addChild(translated);
    m_rootObject->addChild(rotated);
    m_rootObject->addChild(scaled);
}
