#include "header/glpbrlighting.h"
#include "header/spheremesh.h"
#include <QMatrix4x4>
#include <QtMath>

GLPbrLighting::GLPbrLighting(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLPbrLighting");
}

GLPbrLighting::~GLPbrLighting() = default;

void GLPbrLighting::initializeGL()
{
    GLBase::initializeGL();

    // Compile PBR shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pbr/lighting.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/pbr/lighting.frag"))
    {
        qFatal("Failed to compile PBR lighting shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link PBR lighting shader program");
    }

    // Generate sphere mesh and setup OpenGL resources
    m_sphere = generateSphere();
    m_sphere->setup();
}

void GLPbrLighting::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D viewPos = m_camera->getPosition();

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("camPos", viewPos);

    // Light positions and colors
    const QVector3D lightPositions[] = {
        QVector3D(-10.0f,  10.0f, 10.0f),
        QVector3D( 10.0f,  10.0f, 10.0f),
        QVector3D(-10.0f, -10.0f, 10.0f),
        QVector3D( 10.0f, -10.0f, 10.0f),
    };
    const QVector3D lightColors[] = {
        QVector3D(300.0f, 300.0f, 300.0f),
        QVector3D(300.0f, 300.0f, 300.0f),
        QVector3D(300.0f, 300.0f, 300.0f),
        QVector3D(300.0f, 300.0f, 300.0f),
    };

    for (int i = 0; i < 4; ++i)
    {
        m_program.setUniformValue(QString("lightPositions[%1]").arg(i).toUtf8(), lightPositions[i]);
        m_program.setUniformValue(QString("lightColors[%1]").arg(i).toUtf8(), lightColors[i]);
    }

    // Material defaults
    m_program.setUniformValue("albedo", 0.5f, 0.0f, 0.0f);
    m_program.setUniformValue("ao", 1.0f);

    // Render rows*columns of spheres with varying metallic/roughness
    const int nrRows = 7;
    const int nrColumns = 7;
    const float spacing = 2.5f;

    for (int row = 0; row < nrRows; ++row)
    {
        m_program.setUniformValue("metallic", (float)row / (float)nrRows);
        for (int col = 0; col < nrColumns; ++col)
        {
            float roughness = qBound(0.05f, (float)col / (float)nrColumns, 1.0f);
            m_program.setUniformValue("roughness", roughness);

            QMatrix4x4 model;
            model.translate(QVector3D(
                (col - (nrColumns / 2)) * spacing,
                (row - (nrRows / 2)) * spacing,
                0.0f
            ));
            m_program.setUniformValue("model", model);
            m_program.setUniformValue("normalMatrix", model.normalMatrix());

            m_sphere->draw(m_program);
        }
    }

    // Render light source spheres
    for (int i = 0; i < 4; ++i)
    {
        QMatrix4x4 model;
        model.translate(lightPositions[i]);
        model.scale(0.5f);
        m_program.setUniformValue("model", model);
        m_program.setUniformValue("normalMatrix", model.normalMatrix());

        m_sphere->draw(m_program);
    }
}
