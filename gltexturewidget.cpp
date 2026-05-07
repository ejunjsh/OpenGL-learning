#include "gltexturewidget.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QImage>
#include "mesh.h"

GLTextureWidget::GLTextureWidget(QWidget *parent)
    : GLWidget(parent)
{
    setName("GLTextureWidget");
}

unsigned int GLTextureWidget::loadTexture(const QString &path, bool flipVertically, bool flipHorizontally)
{
    QImage image(path);
    if (image.isNull()) {
        qWarning() << "Failed to load texture:" << path;
        return 0;
    }
    image = image.convertToFormat(QImage::Format_RGBA8888);

    if (flipVertically) {
        image = image.mirrored(false, true);  // 垂直翻转
    }
    if (flipHorizontally) {
        image = image.mirrored(true, false);   // 水平翻转
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
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
    quad->setPosition(QVector3D(0.0f, 0.0f, 0.0f));

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(quad);
}
