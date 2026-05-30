#include "header/glmodelloading.h"
#include <QMatrix4x4>

GLModelLoading::GLModelLoading(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLModelLoading");
}

GLModelLoading::~GLModelLoading()
{
    makeCurrent();
    m_model.reset();
    doneCurrent();
}

void GLModelLoading::initializeGL()
{
    GLBase::initializeGL();

    // Compile model loading shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/model_loading/model_loading.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/model_loading/model_loading.frag"))
    {
        qFatal("Failed to compile model loading shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link model loading shader program");
    }

    // Load model
    m_model = std::make_unique<Model>("objects/backpack/backpack.obj");
}

void GLModelLoading::paintGL()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);

    // Model matrix — center and scale the backpack
    QMatrix4x4 model;
    model.translate(QVector3D(0.0f, 0.0f, 0.0f));
    model.scale(1.0f);
    m_program.setUniformValue("model", model);

    // Draw the loaded model (iterates over all meshes)
    m_model->Draw(m_program);
}
