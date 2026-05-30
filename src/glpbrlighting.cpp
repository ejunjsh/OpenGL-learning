#include "header/glpbrlighting.h"
#include <QMatrix4x4>
#include <QtMath>
#include <vector>
#include <cmath>

GLPbrLighting::GLPbrLighting(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLPbrLighting");
}

GLPbrLighting::~GLPbrLighting()
{
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
    doneCurrent();
}

static void generateSphereData(std::vector<float> &vertices, std::vector<unsigned int> &indices,
                               unsigned int xSegments = 64, unsigned int ySegments = 64)
{
    constexpr float PI = 3.14159265359f;
    for (unsigned int y = 0; y <= ySegments; ++y)
    {
        for (unsigned int x = 0; x <= xSegments; ++x)
        {
            float xSegment = static_cast<float>(x) / xSegments;
            float ySegment = static_cast<float>(y) / ySegments;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            // position
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            // normal (same as position for unit sphere)
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            // texCoords
            vertices.push_back(xSegment);
            vertices.push_back(ySegment);
        }
    }

    for (unsigned int y = 0; y < ySegments; ++y)
    {
        for (unsigned int x = 0; x < xSegments; ++x)
        {
            unsigned int topLeft     = y       * (xSegments + 1) + x;
            unsigned int topRight    = y       * (xSegments + 1) + x + 1;
            unsigned int bottomLeft  = (y + 1) * (xSegments + 1) + x;
            unsigned int bottomRight = (y + 1) * (xSegments + 1) + x + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

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

    // Generate sphere geometry
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphereData(vertices, indices);

    m_indexCount = static_cast<unsigned int>(indices.size());

    // Setup VAO/VBO/EBO — 8 floats per vertex: pos(3) + normal(3) + texCoord(2)
    constexpr int stride = 8 * sizeof(float);

    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(float)));

    m_ebo.create();
    m_ebo.bind();
    m_ebo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(unsigned int)));

    // location 0: aPos (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // location 1: aNormal (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // location 2: aTexCoords (vec2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    m_vao.release();
    m_vbo.release();
    m_ebo.release();
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

            m_vao.bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(m_indexCount), GL_UNSIGNED_INT, nullptr);
            m_vao.release();
        }
    }

    // Render light source spheres
    m_vao.bind();
    for (int i = 0; i < 4; ++i)
    {
        QMatrix4x4 model;
        model.translate(lightPositions[i]);
        model.scale(0.5f);
        m_program.setUniformValue("model", model);
        m_program.setUniformValue("normalMatrix", model.normalMatrix());

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    }
    m_vao.release();
}
