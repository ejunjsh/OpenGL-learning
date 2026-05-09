#include "header/gltexturewidget.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include "header/mesh.h"

GLTextureWidget::GLTextureWidget(QWidget *parent)
    : GLWidget(parent)
{
    setName("GLTextureWidget");
}

void GLTextureWidget::initializeGL()
{
    GLWidget::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag"))
    {
        qFatal("Failed to compile texture shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link texture shader program");
    }

    setupScene();
}

void GLTextureWidget::paintGL()
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
        m_rootObject->draw(m_program);
    }

    m_program.release();
}

void GLTextureWidget::setupScene()
{
    std::vector<Vertex> vertices = {
        Vertex(QVector3D(0.5f, 0.5f, 0.0f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D(0.5f, -0.5f, 0.0f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, 0.0f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, 0.5f, 0.0f), QVector2D(0.0f, 1.0f))
    };

    std::vector<unsigned int> indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // 加载 container.jpg 纹理
    Texture containerTex;
    containerTex.id = loadTexture(":/textures/container.jpg");
    containerTex.name = "container";

    Texture faceTex;
    faceTex.id = loadTexture(":/textures/awesomeface.png",true);
    faceTex.name = "face";

    std::vector<Texture> textures = {containerTex, faceTex};

    auto mesh = std::make_shared<Mesh>(vertices, indices, textures);
    auto quad = std::make_shared<Object3D>("Quad");
    quad->addMesh(mesh);

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(quad);
}
