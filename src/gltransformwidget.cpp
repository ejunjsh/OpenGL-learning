#include "header/gltransformwidget.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include "header/mesh.h"

GLTransformWidget::GLTransformWidget(QWidget *parent)
    : GLWidget(parent)
{
    setName("GLTransformWidget");
}

void GLTransformWidget::initializeGL()
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
        // 传递纹理 uniform
        m_program.setUniformValue("container", 0);
        m_program.setUniformValue("face", 1);

        m_rootObject->setPosition(QVector3D(0.5f, -0.5f, 0.0f));
        // 更新旋转角度（使用父类的 elapsedTime）
        m_rootObject->setRotation(QVector3D(0.0f, 0.0f, elapsedTime() * 45.0f));
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

    // 加载纹理
    Texture containerTex;
    containerTex.id = loadTexture(":/textures/container.jpg");
    containerTex.name = "container";

    Texture faceTex;
    faceTex.id = loadTexture(":/textures/awesomeface.png", true);
    faceTex.name = "face";

    std::vector<Texture> textures = {containerTex, faceTex};

    auto mesh = std::make_shared<Mesh>(vertices, indices, textures);

    // 绑定纹理到纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTex.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, faceTex.id);

    auto quad = std::make_shared<Object3D>("Quad");
    quad->addMesh(mesh);

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(quad);
}
